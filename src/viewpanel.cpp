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

#include "cvision/viewpanel.hpp"
#include "cvision/view.hpp"

using namespace std;

namespace cvis
{

CVViewPanel::CVViewPanel(CVView* parentView,
                         const string& panelTag,
                         const sf::Color& backgroundColor,
                         const sf::Vector2f& size,
                         const bool& fitToWindow,
                         const sf::Vector2f& position):
    CVTextBox(parentView,
              position,
              size.x,
              size.y,
              backgroundColor,
              backgroundColor,
              0.0f),
    bFitWindow(fitToWindow),
    bTransduceFade(false),
    bTransduceFocus(false),
    bDragAndDrop(false),
    bOutOfBoundsDraw(false),
    bOutOfBoundsUpdate(false),
    bFadeMembersOnly(false),
    bReverseDrawOrder(false)
{
    IDtag = panelTag;

    if(!isnan(size.x) && !isnan(size.y))
    {
        setSize(size);
    }
    else
    {
        setSize(parentView->getSize());
    }
}

CVViewPanel::~CVViewPanel()
{
    for(auto& item : viewPanelElements)
    {
        delete(item);
    }
}

void CVViewPanel::updateBounds()
{
    if(numPanels() > 0)
    {
        bounds = viewPanelElements.front()->getBounds();
        for(size_t i = 1; i < viewPanelElements.size(); ++i)
        {
            expandBounds(bounds, viewPanelElements[i]->getBounds());
        }

        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
        panel.front().setPosition(sf::Vector2f(bounds.left, bounds.top));
    }
    else
    {
        bounds.width = 0.0f;
        bounds.height = 0.0f;
        panel.front().setSize(sf::Vector2f(0.0f,0.0f));
    }
}

void CVViewPanel::updatePanels(CVEvent& event, const sf::Vector2f& mousePos)
{

    for(int i = viewPanelElements.size() - 1; (i >= 0) && !viewPanelElements.empty(); --i)
    {
        if(viewPanelElements[i]->shouldDelete())
        {
            removePanelElement(i);
        }
        else
        {
            if(bOutOfBoundsUpdate || View->contains(*viewPanelElements[i]))
            {
                if(!bClipBounds || panel.front().getGlobalBounds().intersects(viewPanelElements[i]->getBounds()))
                {
                    viewPanelElements[i]->update(event, mousePos);
                }
            }
        }
    }

}

void CVViewPanel::setExpand(const bool& state)
{
    bExpand = state;
    if(bExpand)
    {
        updateBounds();
    }
}

void CVViewPanel::addPanelElement(CVElement* newElement,
                                  const string& newTag,
                                  const unsigned int& index)
{

    // Transfer flags

    newElement->viewPanel = this;

    if(!newTag.empty())
    {
        newElement->setTag(newTag);
    }

    if(index < numPanels())
    {
        viewPanelElements.insert(viewPanelElements.begin() + index, newElement);
    }
    else
    {
        viewPanelElements.emplace_back(newElement);
    }

    if(bExpand)
    {
        updateBounds(); // Update the panel boundaries to accommodate if applicable
    }

}

void CVViewPanel::clear()
{
    for(auto& item : viewPanelElements)
    {
        delete(item);
    }
    viewPanelElements.clear();
}

bool CVViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(bFade && (bTransduceFade || (fadeLayers & CV_LAYER_MEMBERS)))
    {
        for(auto& item : viewPanelElements)
        {
            item->setFade(targetAlpha, fadeRate, fadeLayers);
        }
    }

    if(fadeLayers == CV_LAYER_MEMBERS)
    {
        bFade = false;
    }

    if(!CVTextBox::update(event, mousePos))
    {
        return false;
    }

    if(bExpand)
    {
        updateBounds();
    }

    if(bTransduceFocus)
    {
        for(auto& item : viewPanelElements)
        {
            item->setFocus(hasFocus());
        }
    }

    return true;
}

void CVViewPanel::removePanelElement(const unsigned int& index)
{
    if(index < numPanels())
    {
        delete(viewPanelElements[index]);
        viewPanelElements.erase(viewPanelElements.begin() + index);
    }
    else
    {
        throw out_of_range("In CVViewPanel::removePanelElement: index out of range of element list");
    }

    if(bExpand)
    {
        updateBounds();
    }
}

void CVViewPanel::removePanelElement(CVElement* element)
{
    for(size_t i = 0; i < viewPanelElements.size(); ++i)
    {
        if(viewPanelElements[i] == element)
        {
            removePanelElement(i);
            return;
        }
    }
}

void CVViewPanel::removePanelElement(const string& tag)
{
    for(size_t i = 0; i < viewPanelElements.size(); ++i)
    {
        if(viewPanelElements[i]->tag() == tag)
        {
            removePanelElement(i);
            return;
        }
    }
}

void CVViewPanel::detachPanelElement(const CVElement* element) noexcept
{

    for(size_t i = 0; i < viewPanelElements.size(); ++i)
    {

        if(viewPanelElements[i] == element)
        {
            viewPanelElements.erase(viewPanelElements.begin() + i);
            return;
        }

    }

    if(bExpand)
    {
        updateBounds();
    }

}

void CVViewPanel::move(const sf::Vector2f& distance)
{
    CVTextBox::move(distance);
    for(auto& panel : viewPanelElements)
    {
        panel->move(distance);
    }
}

void CVViewPanel::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

void CVViewPanel::setSize(const sf::Vector2f& newSize)
{
    float scaleFactorX = getSize().x/newSize.x;
    float scaleFactorY = getSize().y/newSize.y;

    for(auto& item : viewPanelElements)
    {
        sf::Vector2f dist = item->getPosition() - getPosition(),
                            scaleDist = dist;
        scaleDist.x *= scaleFactorX;
        scaleDist.y *= scaleFactorY;
        item->move(-(scaleDist - dist));
//        item->setSize(item->getSize().x*scaleDist.x,
//                      item->getSize().y*scaleDist.y);
    }

    CVBox::setSize(newSize);
}

void CVViewPanel::setPanelSize(const sf::Vector2f& newSize)
{

    bounds.width = newSize.x;
    bounds.height = newSize.y;

    fTrueSize = newSize;
    iDrawSize.x = std::round(newSize.x);
    iDrawSize.y = std::round(newSize.y);

    panel.front().setSize(newSize);

}

void CVViewPanel::setActiveState(const bool& state)
{

    CVElement::setActiveState(state);
    for(auto& element : viewPanelElements)
    {
        element->setActiveState(state);
    }

}

void CVViewPanel::fitElements(const bool& fitX,
                              const bool& fitY)
{
    if(!numPanels())
    {
        CVBox::setSize(sf::Vector2f(0,0));
    }
    else if(fitX || fitY)
    {
        sf::FloatRect newBounds = viewPanelElements.front()->getBounds();

        for(size_t i = 1; i < numPanels(); ++i)
        {
            expandBounds(newBounds, viewPanelElements[i]->getBounds());
        }

        sf::Vector2f newPosition = getPosition();
        sf::Vector2f newSize = getSize();

        if(fitX)
        {
            newPosition.x = newBounds.left;
            newSize.x = newBounds.width;
        }
        if(fitY)
        {
            newPosition.y = newBounds.top;
            newSize.y = newBounds.height;
        }

        CVBox::setPosition(newPosition);
        CVBox::setSize(newSize);
    }
}

CVElement* CVViewPanel::getTaggedElement(const string& tag)
{
    for(auto& element : viewPanelElements)
    {
        if(element->tag() == tag) return element;
    }
    return nullptr;
}

CVElement* CVViewPanel::getOwnedElementByID(const string& tag)
{

    if(tag.empty()) return nullptr;

    for(auto& element : viewPanelElements)
    {
        if(element->tag() == tag)
        {
            return element;
        }
        else if(dynamic_cast<CVViewPanel*>(element))
        {
            CVElement* output = nullptr;

            if(output = ((CVViewPanel*)element)->getOwnedElementByID(tag))
            {
                return output;
            }
        }
    }

    return nullptr;

}

bool CVViewPanel::elementExists(const string& tag)
{
    for(auto& panel : viewPanelElements)
    {
        if(panel->tag() == tag) return true;
    }
    return false;
}

void CVViewPanel::setHighlightableStatus(const bool& status)
{
    CVElement::setHighlightableStatus(status);
    for(auto& item : viewPanelElements)
    {
        item->setHighlightableStatus(status);
    }
}

void CVViewPanel::setFocus(const bool& state)
{
    CVElement::setFocus(state);
    if(bTransduceFocus && state)
    {
        for(auto& item : viewPanelElements)
        {
            item->setFocus(state);
        }
    }
}

void CVViewPanel::setHighlightColor(const sf::Color& color)
{
    highlightColor = color;
    for(auto& item : viewPanelElements)
    {
        item->setHighlightColor(color);
    }
}

bool CVViewPanel::fadeComplete() const noexcept
{
    if(!bFadeMembersOnly)
    {
        if(!CVTextBox::fadeComplete()) return false;
    }

    if(bTransduceFade || (fadeLayers & CV_LAYER_MEMBERS))
    {
        for(auto& element : viewPanelElements)
        {
            if(!element->fadeComplete()) return false;
        }
    }

    return true;
}

}
