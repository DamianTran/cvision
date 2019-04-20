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

namespace cvis
{

CVNodePanel::CVNodePanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
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
                         panRate(25.0f),
                         nodeDistance((size.x + size.y)/2),
                         elasticCoef(5.0f),
                         pathThickness(2.0f),
                         anchorPoint(position){
    bTransduceFade = true; // Allow chaining of node panel fade effects
    bTransduceFocus = true;
    setDraggableStatus(true);
    setOutOfBoundsUpdate(true);
    setOutOfBoundsDraw(true);
}

void CVNodePanel::setNodeAlpha(const uint8_t& newAlpha){
    nodeAlpha = newAlpha;
}

std::string CVNodePanel::lastInteracted() const{
    if(interactionTrace.size() > 0){
        return interactionTrace.back();
    }
    return std::string();
}

bool CVNodePanel::captureInteraction(std::string* output){
    if(!interactionTrace.empty()){
        if(output) *output = interactionTrace.front();
        interactionTrace.erase(interactionTrace.begin());
        return true;
    }
    return false;
}

void CVNodePanel::logInteraction(const std::string& tag){
    interactionTrace.push_back(tag);
    if(interactionTrace.size() > maxTraceLength) interactionTrace.pop_back();
    bNodeInteracted = true;
}

void CVNodePanel::setHoverTag(const bool& status){
    bShowTagOnHover = status;

    if(bShowTagOnHover){
        for(size_t i = 0; i < numPanels(); ++i){
            if(i >= hoverTags.size()){
                hoverTags.emplace_back(viewPanelElements[i]->tag(), *mainApp()->fonts[textInfo.font], textInfo.fontSize);
            }
        }
    }
    else{
        hoverTags.clear();
    }
}

bool CVNodePanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVViewPanel::update(event, mousePos)) return false;

    if(bAutoPan && (nodeIndex != UINT_MAX)){
        if(numPanels() == 0){
            setPosition(anchorPoint);
        }
        else{
            move(panRate/250*(anchorPoint -
                              sf::Vector2f(viewPanelElements[nodeIndex]->getPosition().x,
                                           viewPanelElements[nodeIndex]->getPosition().y)));
        }
    }

    if(event.LMBreleased && (event.LMBreleaseFrames == 2)){
        if(center->getBounds().contains(event.LMBreleasePosition) &&
       event.captureMouse() && event.captureFocus()){
            setFocus(true);
            if(event.LMBreleased && (event.LMBholdTime < 0.15f)){
                bNodeOpened = !bNodeOpened;
            }
       }
    }
    else if(event.LMBhold && bounds.contains(event.lastFrameMousePosition) &&
            event.captureMouse()){
                setFocus(true);
    }

    if(hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) bNodeOpened = false;

    if(event.LMBhold ||
       (hasFocus() && ((event.LMBreleaseTime < 2.0f) || (event.timeLastKey < 2.0f)))){
        if(bNodeOpened){
            sf::Vector2f onscreen_arc = internal_arc(anchorPoint, View->getBounds(), nodeDistance);
            for(size_t i = 0; i < numPanels(); ++i){
                if(viewPanelElements[i] == center) continue;
                float angle = -(i*2*PI/(numPanels()-1));
                if(!View->getBounds().contains(radial_position(anchorPoint, nodeDistance, angle)))
                        angle = -(onscreen_arc.x + i*(onscreen_arc.y-onscreen_arc.x)/(numPanels()));
                viewPanelElements[i]
                    ->move(elasticCoef*event.lastFrameTime*(radial_position(anchorPoint, nodeDistance, // Move toward radial position from the anchorpoint at distance of
                            angle) // nodeDistance proportional to the elastic coefficient, at a fractional angle of the available arc
                           - viewPanelElements[i]->getPosition()));

                nodePaths[i].setSize(sf::Vector2f(getDistance(viewPanelElements[i]->getPosition(), anchorPoint), pathThickness));
                nodePaths[i].setPosition(anchorPoint);
                nodePaths[i].setRotation(get_angle(anchorPoint, viewPanelElements[i]->getPosition())*180/PI);
                nodePaths[i].setFillColor(viewPanelElements[i]->baseFillColor());

                if(viewPanelElements[i]->isType<CVNodePanel>()){    // Ensure other nodes unfold if path is selected
                    if(pathSelected[i]){ // Path selected vector only detects CVNodePanel class types
                        ((CVNodePanel*)viewPanelElements[i])->bNodeOpened = true;
                    }
                }
            }

        }
        else{
            for(size_t i = 0; i < numPanels(); ++i){
                if(viewPanelElements[i] == center) continue;
                viewPanelElements[i] // Shrink toward center
                    ->move(elasticCoef*event.lastFrameTime*(anchorPoint - viewPanelElements[i]->getPosition()));

                if(viewPanelElements[i]->isType<CVNodePanel>()){
                    if(((CVNodePanel*)viewPanelElements[i])->bNodeOpened){
                        ((CVNodePanel*)viewPanelElements[i])->bNodeOpened = false;
                    }
                }

                viewPanelElements[i]->setFade(0, -5);
                nodePaths[i].setSize(sf::Vector2f(getDistance(viewPanelElements[i]->getPosition(), anchorPoint), pathThickness));
                nodePaths[i].setPosition(anchorPoint);
                nodePaths[i].setFillColor(viewPanelElements[i]->baseFillColor());
            }
        }
    }

    for(size_t i = 0; i < numPanels(); ++i){
        if(i > 0){
            if(bNodeOpened){
                viewPanelElements[i]->setFade(nodeAlpha, 5);
            }
            else viewPanelElements[i]->setFade(0, -5);
        }

        if(viewPanelElements[i]->hasFocus()) setFocus(true);

        if(bShowTagOnHover){
            sf::Color tmp;
            if(bNodeOpened && !(viewPanelElements[i]->isType<CVNodePanel>() && ((CVNodePanel*)viewPanelElements[i])->isOpen())){
                sf::Color col;
                if((viewPanelElements[i]->getBounds().contains(mousePos) || altPressed())){
                    tmp = hoverTags[i].getFillColor(); // Transfer the element color hue but not alpha
                    col = viewPanelElements[i]->baseFillColor();
                    uint8_t alpha = tmp.a;
                    tmp = col;
                    if(alpha + 5 < nodeAlpha) tmp.a = alpha + 5;
                    else tmp.a = nodeAlpha;
                    hoverTags[i].setFillColor(tmp);
                }
                else{
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
            else{
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
        if(View->contains(*viewPanelElements[i])){
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
               viewPanelElements[i]->getBounds().contains(mousePos)){
                   if(viewPanelElements[i]->isType<CVNodePanel>()){
                          if(((CVNodePanel*)viewPanelElements[i])->isOpen()){
                                std::string interaction;
                                if(((CVNodePanel*)viewPanelElements[i])->captureInteraction(&interaction)){
                                    logInteraction(interaction);
                                    interactionCaptured = true;
                                }
                                if(((CVNodePanel*)viewPanelElements[i])
                                    ->center
                                        ->getBounds().contains(mousePos)) pathSelected[i] = false;
                          }
                          else{
                                pathSelected[i] = true;
    //                            setCenter(*viewPanelElements[i]);
                          }
                    }
                    else{
                        logInteraction(viewPanelTags[i]);
                        interactionCaptured = true;
                    }
            }

        }

    }

    return true;
}

void CVNodePanel::setNode(const int& index){
    if(index >= (int)numPanels()){
        nodeIndex = 0;
    }
    else if(index < 0){
        nodeIndex = numPanels() - 1;
    }
    else{
        nodeIndex = index;
    }
    bAutoPan = true;
}

void CVNodePanel::setNodeRadius(const float& newRadiusPx){
    nodeDistance = newRadiusPx;
}

bool CVNodePanel::draw(sf::RenderTarget* target){
    if(!CVTextBox::draw(target)) return false;
    for(auto& line : nodePaths){
        target->draw(line);
    }
    for(auto& item : viewPanelElements){
        item->draw(target);
    }
    for(auto& text : hoverTags){
        target->draw(text);
    }

    return true;
}

void CVNodePanel::setCenter(CVElement& element){
    center = &element;
    anchorPoint = element.getPosition() - element.getOrigin();
}

void CVNodePanel::addPanelElement(CVElement* newElement, std::string newTag){
    CVViewPanel::addPanelElement(newElement, newTag);
    nodePaths.emplace_back();
    nodePaths.back().setSize(sf::Vector2f(0.0f, pathThickness));
    nodePaths.back().setOrigin(sf::Vector2f(0.0f, pathThickness/2));
    if(center != nullptr)
        nodePaths.back().setFillColor(center->baseFillColor());
    else nodePaths.back().setFillColor(sf::Color::Transparent);

    if(bShowTagOnHover){
        hoverTags.emplace_back(newTag, *mainApp()->fonts[textInfo.font], textInfo.fontSize);
    }

    pathSelected.push_back(false);
}

void CVNodePanel::removePanelElement(const unsigned int& index){
    CVViewPanel::removePanelElement(index);
    nodePaths.erase(nodePaths.begin() + index);
    if(nodeIndex > (int)index) setNode(index-1);
    else setNode(index);

    pathSelected.erase(pathSelected.begin() + index);
}

void CVNodePanel::move(const sf::Vector2f& distance){
    CVBasicViewPanel::move(distance);
    anchorPoint += distance;
}

void CVNodePanel::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

}
