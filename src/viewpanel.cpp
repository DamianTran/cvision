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

namespace cvis
{

CVViewPanel::CVViewPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                         const sf::Vector2f& size, bool fitToWindow, const sf::Vector2f& position):
    CVTextBox(parentView, position, parentView->getWidth(), parentView->getHeight(), backgroundColor, backgroundColor, 0.0f),
    bFitWindow(fitToWindow),
    bTransduceFade(false),
    bTransduceFocus(false),
    bDragAndDrop(false),
    bOutOfBoundsDraw(false),
    bOutOfBoundsUpdate(false)
{

    if(!isnan(size.x) && !isnan(size.y)) setSize(size);
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
        sf::FloatRect panelBounds;
        for(size_t i = 1; i < viewPanelElements.size(); ++i)
        {
            panelBounds = viewPanelElements[i]->getBounds();
            if(panelBounds.left < bounds.left) bounds.left = panelBounds.left;
            if(panelBounds.top < bounds.top) bounds.top = panelBounds.top;
            if(panelBounds.left + panelBounds.width > bounds.left + bounds.width)
                bounds.width = panelBounds.left + panelBounds.width - bounds.left;
            if(panelBounds.top + panelBounds.height > bounds.top + bounds.height)
                bounds.height = panelBounds.top + panelBounds.height - bounds.top;
        }

        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
        panel.front().setPosition(bounds.left, bounds.top);
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

    for(int i = viewPanelElements.size() - 1; i >= 0; --i){
        if(viewPanelElements[i]->shouldDelete())
        {
            removePanelElement(i);
        }
        else{
            if(bOutOfBoundsUpdate || View->contains(*viewPanelElements[i])){
                viewPanelElements[i]->update(event, mousePos);
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

void CVViewPanel::addPanelElement(CVElement* newElement, std::string newTag, const unsigned int& index)
{

    // Transfer flags

    newElement->viewPanel = this;
    newElement->setTag(newTag);

    if(index < numPanels())
    {
        viewPanelElements.insert(viewPanelElements.begin() + index, newElement);
        viewPanelTags.insert(viewPanelTags.begin() + index, newTag);
    }
    else
    {
        viewPanelTags.push_back(newTag);
        viewPanelElements.push_back(newElement);
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
    viewPanelTags.clear();
}

bool CVViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVTextBox::update(event, mousePos)) return false;

    if(bExpand)
    {
        updateBounds();
    }

    if(bFitWindow && event.viewResized)
    {
        setPosition(0.0f,0.0f);
        setSize(sf::Vector2f(event.viewBounds.width, event.viewBounds.height));
    }

    if(bFade && bTransduceFade)
    {
        for(auto& item : viewPanelElements)
        {
            item->setFade(targetAlpha, fadeRate);
        }
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
        viewPanelTags.erase(viewPanelTags.begin() + index);
    }
    else
    {
        throw std::out_of_range("In CVViewPanel::removePanelElement: index out of range of element list");
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

void CVViewPanel::removePanelElement(const std::string& tag)
{
    for(size_t i = 0; i < viewPanelElements.size(); ++i)
    {
        if(viewPanelTags[i] == tag)
        {
            removePanelElement(i);
            return;
        }
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
    float scaleFactorX = getSize().x/newSize.x,
          scaleFactorY = getSize().y/newSize.y;
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

CVElement* CVViewPanel::getTaggedElement(const std::string& tag)
{
    unsigned int index = 0;
    for(auto& T : viewPanelTags)
    {
        if(T == tag) return viewPanelElements[index];
        else ++index;
    }
    return nullptr;
}

bool CVViewPanel::elementExists(const std::string& tag)
{
    for(auto& T : viewPanelTags)
    {
        if(T == tag) return true;
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

}
