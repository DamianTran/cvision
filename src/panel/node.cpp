/** /////////////////////////////////////////////////////////////
//
//  CVision: the flexible cascading-style GUI library for C++
//
// //////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2019 Damian Tran
//
// DESCRIPTION:
//
// CVision is a graphical user interface (GUI) library that
// attempts to simplify and speed up the process of desktop
// app design.  CVision incorporates a cascading structure
// scheme that resembles the following:
//
// App -> View -> Panel -> Element -> Primitives/Sprites
//
// The subsequent connection of each "leaf" of the hierarchy
// automatically ensures that the element will be updated,
// drawn to the renderer, and otherwise disposed of at
// the program's termination.
//
// LEGAL:
//
// Modification and redistribution of CVision is freely
// permissible under any circumstances.  Attribution to the
// Author ("Damian Tran") is appreciated but not necessary.
//
// CVision is an open source library that is provided to you
// (the "User") AS IS, with no implied or explicit
// warranties.  By using CVision, you acknowledge and agree
// to this disclaimer.  Use of CVision in the Users's programs
// or as a part of a derivative library is performed at
// the User's OWN RISK.
//
// ACKNOWLEDGEMENTS:
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
/////////////////////////////////////////////////////////////  **/


#include "cvision/view.hpp"
#include "cvision/app.hpp"
#include "cvision/panel/node.hpp"

#include <hyper/algorithm.hpp>

using namespace std;
using namespace hyperC;

namespace cvis
{

CVNodePanel::CVNodePanel(CVView* parentView, string panelTag, sf::Color backgroundColor,
                         const sf::Vector2f& size, bool bFitToWindow,
                         const sf::Vector2f& position):
    CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
    center(nullptr),
    nodeIndex(0),
    maxTraceLength(1000),
    nodeAlpha(255),
    bAutoPan(false),
    bNodeOpened(false),
    bNodeInteracted(false),
    bShowTagOnHover(false),
    bIsChild(false),
    panRate(25.0f),
    nodeDistance((size.x + size.y)/2),
    elasticCoef(0.1f),
    pathThickness(2.0f),
    fRepulsionCoef(0.0f),
    anchorPoint(position)
{
    bTransduceFade = true; // Allow chaining of node panel fade effects
    bTransduceFocus = true;
    setDraggableStatus(true);
    setOutOfBoundsUpdate(true);
    setOutOfBoundsDraw(true);
}

void CVNodePanel::setNodeAlpha(const uint8_t& newAlpha)
{
    nodeAlpha = newAlpha;
}

string CVNodePanel::lastInteracted() const
{
    if(interactionTrace.size() > 0)
    {
        return interactionTrace.back();
    }
    return string();
}

bool CVNodePanel::captureInteraction(string* output)
{
    if(!interactionTrace.empty())
    {
        if(output) *output = interactionTrace.front();
        interactionTrace.erase(interactionTrace.begin());
        return true;
    }
    return false;
}

void CVNodePanel::logInteraction(const string& tag)
{
    interactionTrace.push_back(tag);
    if(interactionTrace.size() > maxTraceLength) interactionTrace.pop_back();
    bNodeInteracted = true;
}

void CVNodePanel::setHoverTag(const bool& status)
{
    bShowTagOnHover = status;

    if(bShowTagOnHover)
    {
        for(size_t i = 0; i < numPanels(); ++i)
        {
            if(i >= hoverTags.size())
            {
                hoverTags.emplace_back(viewPanelElements[i]->tag(), *mainApp()->fonts[textInfo.font], textInfo.fontSize);
            }
        }
    }
    else
    {
        hoverTags.clear();
    }
}

bool CVNodePanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVViewPanel::update(event, mousePos))
    {
        return false;
    }

    // Pan on network node select

//    if(bAutoPan && (nodeIndex != UINT_MAX))
//    {
//        if(numPanels() == 0)
//        {
//            setPosition(anchorPoint);
//        }
//        else
//        {
//            move(panRate/250*(anchorPoint -
//                              sf::Vector2f(viewPanelElements[nodeIndex]->getPosition().x,
//                                           viewPanelElements[nodeIndex]->getPosition().y)));
//        }
//    }

    if(event.LMBreleased && (event.LMBreleaseFrames == 2))
    {
        if(center->getBounds().contains(event.LMBreleasePosition) &&
                event.captureMouse() && event.captureFocus())
        {
            setFocus(true);
            if(event.LMBreleased && (event.LMBholdTime < 0.15f))
            {
                bNodeOpened = !bNodeOpened;
            }
        }
    }
    else if(event.LMBhold && bounds.contains(event.lastFrameMousePosition) &&
            event.captureMouse())
    {
        setFocus(true);
    }
    else if(!bIsChild &&
            ((hasFocus() &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) ||
        (event.LMBreleased &&
        (event.LMBreleaseFrames == 1) &&
        !bounds.contains(event.LMBreleasePosition))))
    {
        bNodeOpened = false;
    }

    for(size_t i = 0; i < numPanels(); ++i)
    {
        if(i > 0)
        {
            if(bNodeOpened)
            {
                viewPanelElements[i]->setFade(nodeAlpha, 5);
            }
            else viewPanelElements[i]->setFade(0, 5);
        }

        if(viewPanelElements[i]->hasFocus()) setFocus(true);

        if(bShowTagOnHover)
        {
            sf::Color tmp;
            if(bNodeOpened &&
               !(viewPanelElements[i]->isType<CVNodePanel>() &&
                 ((CVNodePanel*)viewPanelElements[i])->isOpen()))
            {
                sf::Color col;
                if((viewPanelElements[i]->getBounds().contains(mousePos) ||
                    altPressed()))
                {
                    tmp = hoverTags[i].getFillColor(); // Transfer the element color hue but not alpha
                    col = viewPanelElements[i]->baseFillColor();
                    uint8_t alpha = tmp.a;
                    tmp = col;
                    if(alpha + 5 < nodeAlpha) tmp.a = alpha + 5;
                    else tmp.a = nodeAlpha;
                    hoverTags[i].setFillColor(tmp);
                }
                else
                {
                    tmp = hoverTags[i].getFillColor();
                    col = viewPanelElements[i]->baseFillColor();
                    uint8_t alpha = tmp.a;
                    tmp = col;
                    if(alpha >= 2) tmp.a = alpha - 2;
                    else tmp.a = 0;
                    hoverTags[i].setFillColor(tmp);
                }

                hoverTags[i].setPosition(sf::Vector2f(viewPanelElements[i]->getBounds().left + viewPanelElements[i]->getBounds().width/2
                                                      - hoverTags[i].getGlobalBounds().width/2,
                                                      viewPanelElements[i]->getBounds().top + viewPanelElements[i]->getBounds().height +
                                                      hoverTags[i].getGlobalBounds().height + 12.0f));
            }
            else
            {
                tmp = hoverTags[i].getFillColor();
                tmp.a = 0;
                hoverTags[i].setFillColor(tmp);
            }
        }
    }

    bool interactionCaptured = false;
    int index = numPanels()-1;

    for(int i = viewPanelElements.size() - 1; (i >= 0) && !viewPanelElements.empty(); --i)
    {
        if(View->contains(*viewPanelElements[i]))
        {
            viewPanelElements[i]->update(event, mousePos);
        }

        if(viewPanelElements[i]->shouldDelete())
        {
            delete(viewPanelElements[i]);
            viewPanelElements.erase(viewPanelElements.begin() + i);
        }
        else
        {
            if(bNodeOpened && !interactionCaptured && event.LMBreleased &&
                    (event.LMBreleaseFrames == 1) &&
                    viewPanelElements[i]->getBounds().contains(mousePos))
            {
                if(viewPanelElements[i]->isType<CVNodePanel>())
                {
                    if(((CVNodePanel*)viewPanelElements[i])->isOpen())
                    {
                        string interaction;
                        if(((CVNodePanel*)viewPanelElements[i])->captureInteraction(&interaction))
                        {
                            logInteraction(interaction);
                            interactionCaptured = true;
                        }
                        if(((CVNodePanel*)viewPanelElements[i])
                                ->center
                                ->getBounds().contains(mousePos)) pathSelected[i] = false;
                    }
                    else
                    {
                        pathSelected[i] = true;
                        //                            setCenter(*viewPanelElements[i]);
                    }
                }
                else
                {
                    logInteraction(viewPanelTags[i]);
                    interactionCaptured = true;
                }
            }

        }

    }

    updatePhysics(event, mousePos);

    return true;
}

void CVNodePanel::updatePhysics(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(bNodeOpened)
    {

        std::vector<CVElement*> network_elements;
        getConnectedNodes(network_elements);

        double angle;
        float dist;
        float moveDist;

        for(size_t i = 0; i < numPanels(); ++i)
        {

            if(viewPanelElements[i] == center)
            {
                continue;
            }

            sf::Vector2f targetPosition = radial_position(center->getPosition(), nodeDistance, 2*PI*i/(numPanels() - 1));
            dist = getDistance(viewPanelElements[i]->getPosition(),
                               targetPosition);
            angle = get_angle(viewPanelElements[i]->getPosition(),
                              targetPosition);

            viewPanelElements[i]->move(components(dist * elasticCoef, angle));

            nodePaths[i].setSize(sf::Vector2f(getDistance(viewPanelElements[i]->getPosition(), center->getPosition()), pathThickness));
            nodePaths[i].setPosition(center->getPosition());
            nodePaths[i].setRotation(get_angle(center->getPosition(),
                                               viewPanelElements[i]->getPosition())*180/PI);
            nodePaths[i].setFillColor(viewPanelElements[i]->baseFillColor());

            if(viewPanelElements[i]->isType<CVNodePanel>())     // Ensure other nodes unfold if path is selected
            {
                if(pathSelected[i])  // Path selected vector only detects CVNodePanel class types
                {
                    ((CVNodePanel*)viewPanelElements[i])->bNodeOpened = true;
                }
            }
        }

        for(auto& element : network_elements)
        {

            if(element == center)
            {
                continue;
            }

            for(auto& o_element : network_elements)
            {

                if((o_element == center) ||
                   (element == o_element))
                {
                    continue;
                }

                dist = getDistance(element->getPosition(),
                                   o_element->getPosition());

                if(dist < nodeDistance)
                {

                    if(dist != 0.0f)
                    {
                        moveDist = dist * fRepulsionCoef;

                        angle = get_angle(element->getPosition(),
                                          o_element->getPosition());

                    }
                    else
                    {
                        angle = rand((long double)0.0, 2.0*PI);
                        moveDist = 1.0f;
                    }

                    o_element->move(components(moveDist, angle));

                }

            }

        }

    }
    else
    {

        // Shrink nodes toward center and fade out

        for(size_t i = 0; i < numPanels(); ++i)
        {

            if(viewPanelElements[i] == center) continue;

            float dist = getDistance(center->getPosition(), viewPanelElements[i]->getPosition());
            double angle = get_angle(viewPanelElements[i]->getPosition(), center->getPosition());

            viewPanelElements[i]->move(components(dist * elasticCoef, angle));

            if(viewPanelElements[i]->isType<CVNodePanel>())
            {
                if(((CVNodePanel*)viewPanelElements[i])->bNodeOpened)
                {
                    ((CVNodePanel*)viewPanelElements[i])->bNodeOpened = false;
                }
            }

            viewPanelElements[i]->setFade(0, 5);
            nodePaths[i].setSize(sf::Vector2f(getDistance(viewPanelElements[i]->getPosition(), center->getPosition()), pathThickness));
            nodePaths[i].setPosition(center->getPosition());
            nodePaths[i].setFillColor(viewPanelElements[i]->baseFillColor());

        }

    }

}

void CVNodePanel::updateBounds()
{

    if(center)
    {
        bounds = center->getBounds();
    }
    else if(!viewPanelElements.empty())
    {
        bounds = viewPanelElements.front()->getBounds();
    }

    for(auto& element : viewPanelElements)
    {
        expandBounds(bounds, element->getBounds());
    }

    expandBounds(bounds, 24.0f);
    panel.front().setSize(sf::Vector2f(bounds.width,
                                       bounds.height));
    panel.front().setPosition(sf::Vector2f(bounds.left,
                                           bounds.top));

}

void CVNodePanel::setNode(const int& index)
{
    if(index >= (int)numPanels())
    {
        nodeIndex = 0;
    }
    else if(index < 0)
    {
        nodeIndex = numPanels() - 1;
    }
    else
    {
        nodeIndex = index;
    }
    bAutoPan = true;
}

void CVNodePanel::setNodeRadius(const float& newRadiusPx)
{
    nodeDistance = newRadiusPx;
}

bool CVNodePanel::draw(sf::RenderTarget* target)
{
    if(!CVTextBox::draw(target)) return false;
    for(auto& line : nodePaths)
    {
        target->draw(line);
    }
    for(auto& item : viewPanelElements)
    {
        item->draw(target);
    }
    for(auto& text : hoverTags)
    {
        target->draw(text);
    }

    return true;
}

void CVNodePanel::setCenter(CVElement& element)
{
    center = &element;
    anchorPoint = element.getPosition() - element.getOrigin();
}

void CVNodePanel::addPanelElement(CVElement* newElement,
                                  const string& newTag,
                                  const unsigned int& index)
{
    CVViewPanel::addPanelElement(newElement, newTag, index);
    nodePaths.emplace_back();
    nodePaths.back().setSize(sf::Vector2f(0.0f, pathThickness));
    nodePaths.back().setOrigin(sf::Vector2f(0.0f, pathThickness/2));
    if(center != nullptr)
        nodePaths.back().setFillColor(center->baseFillColor());
    else nodePaths.back().setFillColor(sf::Color::Transparent);

    if(bShowTagOnHover)
    {
        hoverTags.emplace_back(newTag, *mainApp()->fonts[textInfo.font], textInfo.fontSize);
    }

    if(dynamic_cast<CVNodePanel*>(newElement))
    {
        ((CVNodePanel*)newElement)->bIsChild = true;
    }

    pathSelected.push_back(false);
}

void CVNodePanel::removePanelElement(const unsigned int& index)
{
    CVViewPanel::removePanelElement(index);
    nodePaths.erase(nodePaths.begin() + index);
    if(nodeIndex > (int)index) setNode(index-1);
    else setNode(index);

    pathSelected.erase(pathSelected.begin() + index);
}

void CVNodePanel::move(const sf::Vector2f& distance)
{
    CVBasicViewPanel::move(distance);
    anchorPoint += distance;
}

void CVNodePanel::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

void CVNodePanel::getConnectedNodes(vector<CVElement*>& output) noexcept
{

    for(auto& element : viewPanelElements)
    {

        if(dynamic_cast<CVNodePanel*>(element) &&
           ((CVNodePanel*)element)->isOpen())
        {
            ((CVNodePanel*)element)->getConnectedNodes(output);
        }
        else if(!anyEqual(element, output))
        {
            output.emplace_back(element);
        }

    }

}

}
