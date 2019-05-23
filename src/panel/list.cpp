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

#include "cvision/panel/list.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include <hyper/algorithm.hpp>

#include <boost/range/adaptor/reversed.hpp>

using namespace hyperC;
using namespace std;

namespace cvis
{

CVListPanel::CVListPanel(CVView* parentView, string panelTag, sf::Color backgroundColor,
                         const sf::Vector2f& size, bool bFitToWindow,
                         const sf::Vector2f& position):
    CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
    listPadding(12.0f),
    scrollBarPadding(24.0f),
    outerPadding(30.0f),
    innerPadding(12.0f),
    listItemHeight(40.0f*View->getViewScale()),
    scrollBarY(parentView,
               sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                            bounds.top + scrollBarPadding),
               sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                            bounds.top + bounds.height - scrollBarPadding),
               size.y, 5.0f, sf::Color(140,140,140), sf::Color::Transparent, 0.0f,
               1000.0f, CV_OBJ_ANIM_FADE_IN),
    listItemColorIndex(colorTheme.size()),
    listItemHighlightColorIndex(colorTheme.size() + 1),
    bSelect(false),
    bListItemHighlight(true)
{
    scrollBarY.setPanelSize(size.y - 2*outerPadding);
    scrollBarY.setPosition(sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                        bounds.top + scrollBarPadding));

    colorTheme.emplace_back(backgroundColor);
    darken(colorTheme.back(), 50);
    colorTheme.back().a += 100;

    colorTheme.emplace_back(colorTheme.back());
    brighten(colorTheme.back(), 50);

}

void CVListPanel::setSelectionStatus(const bool& status)
{
    bSelect = status;
    if(!status)
    {
        selected.clear();
    }
}

void CVListPanel::toggleSelection(CVElement* element)
{
    for(size_t i = 0; i < selected.size(); ++i)
    {
        if(selected[i] == element)
        {
            selected.erase(selected.begin() + i);
            element->highlight(false);
            return;
        }
    }
    selected.push_back(element);
    element->highlight(true);
}

void CVListPanel::toggleSelection(const string& tag)
{
    for(auto& item : viewPanelElements)
    {
        if(item->tag() == tag)
        {
            toggleSelection(item);
            return;
        }
    }
}

void CVListPanel::setSelection(CVElement* element, const bool& state)
{
    if(state)
    {
        if(anyEqual(element, selected)) return;
        selected.push_back(element);
        element->highlight(true);
    }
    else
    {
        for(size_t i = 0; i < selected.size(); ++i)
        {
            if(selected[i] == element)
            {
                selected.erase(selected.begin() + i);
                element->highlight(false);
                return;
            }
        }
    }
}

void CVListPanel::setSelection(const string& tag, const bool& state)
{
    for(auto& item : viewPanelElements)
    {
        if(item->tag() == tag)
        {
            setSelection(item);
            return;
        }
    }
}

vector<string> CVListPanel::getSelection() const
{
    vector<string> output;
    for(auto& element : selected)
    {
        output.emplace_back(element->tag());
    }
    return output;
}

void CVListPanel::clearSelection()
{
    for(auto& item : selected)
    {
        item->highlight(false);
    }
    selected.clear();
}

bool CVListPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos)) return false;

    if(bounds.contains(mousePos) && event.captureFocus())
    {
        setFocus(true);
        scrollBarY.setScrollable(true);
        event.releaseFocus();
    }
    else
    {
        setFocus(false);
        scrollBarY.setScrollable(false);
    }


    float listHeight = 0.0f;
    if(numPanels() > 0)
    {
        listHeight = viewPanelElements.back()->getBounds().top +
                     viewPanelElements.back()->getBounds().height -
                     viewPanelElements.front()->getBounds().top;
    }

    scrollBarY.setScrollMax(listHeight);
    scrollBarY.update(event, mousePos);

    if(listHeight > bounds.height)
    {

        float moveDist = outerPadding + bounds.top -
                         viewPanelElements.front()->getBounds().top
                         - scrollBarY.getScrollOffset();

        for(auto& item : viewPanelElements)
        {
            item->move(sf::Vector2f(0.0f, moveDist));
        }

    }
    else if(numPanels() > 0)
    {

        float moveDist = bounds.top + outerPadding - viewPanelElements.front()->getBounds().top;

        for(auto& item : viewPanelElements)
        {
            item->move(sf::Vector2f(0.0f, moveDist));
        }

    }

    if(bounds.contains(event.LMBpressPosition) && (event.LMBreleaseFrames == 1))
    {
        if(hasFocus())
        {
            for(auto& item : boost::adaptors::reverse(viewPanelElements))
            {
                if(item->getBounds().contains(mousePos) && event.captureMouse())
                {
                    clearSelection();
                    setSelection(item, true);
                }
            }
        }
        else setFocus(false);
    }

    return true;
}

bool CVListPanel::draw(sf::RenderTarget* target)
{
    if(!CVBasicViewPanel::draw(target)) return false;
    scrollBarY.draw(target);

    return true;
}

void CVListPanel::addTextEntry(const string& newText, const unsigned int& index)
{
    CVTextBox* newItem = new CVTextBox(View, sf::Vector2f(bounds.left + innerPadding,
                                       bounds.top + innerPadding),
                                       bounds.width - 2*outerPadding,
                                       listItemHeight, getListItemColor(),
                                       sf::Color::Transparent, 0.0f);
    newItem->setRounding(8.0f*View->getViewScale(), 10);
    newItem->setTextWrap(true);
    newItem->setExpand(true);

    newItem->addTextEntry(TextEntry(newText, textInfo.font, textInfo.fontSize, textInfo.alignment, textInfo.textColor),
                          sf::Vector2f(newItem->getBounds().left + innerPadding,
                                       newItem->getBounds().top + innerPadding));

    newItem->setMask(appTexture("gradient_linear"));
    newItem->setMaskColor(sf::Color(255,255,255,150));

    addPanelElement(newItem, newText, index);
    listTags.emplace_back(newText);
}

void CVListPanel::addTextEntries(const vector<string>& newText)
{
    for(auto& text : newText)
    {
        addTextEntry(text);
    }
}

void CVListPanel::setTextEntries(const vector<string>& newText)
{
    size_t i = 0;
    for(; (i < newText.size() && i < listTags.size()); ++i)
    {
        if(newText[i] != listTags[i])
        {
            listTags.insert(listTags.begin() + i, newText[i]);
            addTextEntry(newText[i], i);
            ++i;
        }
    }
    while(i < newText.size())
    {
        addTextEntry(newText[i]);
        ++i;
    }
    for(i = 0; i < listTags.size(); ++i)
    {
        if(!anyEqual(listTags[i], newText))
        {
            removeTextEntry(i);
        }
    }
}

void CVListPanel::removeTextEntry(const unsigned int& index)
{
    if(index < numPanels())
    {
        for(size_t i = index; i < numPanels(); ++i)
        {
            viewPanelElements[i]->move(0.0f, -viewPanelElements[index]->getBounds().height - listPadding);
        }
        removePanelElement(index);
    }
    else
    {
        removePanelElement(UINT_MAX);
    }
}

bool CVListPanel::TextEntryExists(const string& text) const
{
    return anyEqual(text, listTags);
}

void CVListPanel::setListPadding(const float& newPadding)
{
    for(size_t i = 1; i < viewPanelElements.size(); ++i)
    {
        viewPanelElements[i]->move(sf::Vector2f(newPadding - listPadding, 0.0f));
    }
    listPadding = newPadding;
}

void CVListPanel::setOuterPadding(const float& newPadding)
{
    sf::Vector2f moveDist(0.0f,0.0f);
    for(auto& item : viewPanelElements)
    {
        moveDist.x = 0.0f;
        moveDist.y = 0.0f;

        if(item->getBounds().left < bounds.left + newPadding)
            moveDist.x = bounds.left + newPadding - item->getBounds().left;
        if(item->getBounds().left + item->getBounds().width > bounds.left + bounds.width - newPadding)
            moveDist.x = bounds.left + bounds.width + newPadding - item->getBounds().left - item->getBounds().width;
        if(item->getBounds().top < bounds.top + newPadding)
            moveDist.y = bounds.top + newPadding - item->getBounds().top;
        if(item->getBounds().top + item->getBounds().height > bounds.top + bounds.height - newPadding)
            moveDist.y = bounds.top + bounds.height + newPadding - item->getBounds().left - item->getBounds().height;

        item->move(moveDist);
    }

    scrollBarY.move(sf::Vector2f(outerPadding - newPadding, 0.0f));
    outerPadding = newPadding;
}

void CVListPanel::setScrollBarPadding(const float& newPadding)
{
    scrollBarPadding = newPadding;
    scrollBarY.setAnchorPoints(sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                            bounds.top + scrollBarPadding),
                               sf::Vector2f(bounds.left + bounds.width - outerPadding/2,
                                            bounds.top + bounds.height - scrollBarPadding));
}

void CVListPanel::setInnerPadding(const float& newPadding)
{
    innerPadding = newPadding;
    for(auto& item : viewPanelElements)
    {
        if(item->isType<CVTextBox>())
        {
            ((CVTextBox*)item)->setTextPadding(innerPadding);
        }
    }
}

void CVListPanel::setListItemColor(const sf::Color& newColor)
{
    colorTheme[listItemColorIndex] = newColor;

    for(auto& panel : viewPanelElements)
    {
        if(dynamic_cast<CVBox*>(panel))
        {
            ((CVBox*)panel)->setFillColor(getListItemColor());
        }
    }
}

void CVListPanel::setListHighlightColor(const sf::Color& newColor)
{
    colorTheme[listItemHighlightColorIndex] = newColor;

    for(auto& panel : viewPanelElements)
    {
        panel->setHighlightColor(getListHighlightColor());
    }
}

void CVListPanel::setDragAndDrop(const bool& status)
{
    bDragAndDrop = status;
    for(auto& item : viewPanelElements)
    {
        item->setDragAndDrop(status);
    }
}

void CVListPanel::setListHighlightable(const bool& state)
{
    if(state != bListItemHighlight)
    {
        for(auto& item : viewPanelElements)
        {
            item->setHighlightableStatus(state);
        }
        bListItemHighlight = state;
    }
}

void CVListPanel::fitElements(const bool& fitX, const bool& fitY)
{

    sf::FloatRect elementBounds;

    if(numPanels())
    {
        elementBounds = viewPanelElements.front()->getBounds();
    }
    else
    {
        setPanelSize(0.0f, 0.0f);
        return;
    }

    for(size_t i = 1; i < numPanels(); ++i)
    {

        expandBounds(elementBounds, viewPanelElements[i]->getBounds());

    }

    sf::Vector2f newSize = getSize();

    if(fitX)
    {

        newSize.x = elementBounds.width + 2*outerPadding;

    }

    if(fitY)
    {

        newSize.y = elementBounds.height + 2*outerPadding;

    }

    setPanelSize(newSize);

}

void CVListPanel::addPanelElement(CVElement* newElement,
                                  const string& newTag,
                                  const unsigned int& index)
{

    float outerPaddingActual = outerPadding;

    if(newElement->getSize().x + 2*outerPadding > bounds.width)
    {
        outerPaddingActual = (getSize().x - newElement->getSize().x)/2;
    }

    if(!numPanels())
    {
        newElement->setPosition(bounds.left + outerPaddingActual,
                                bounds.top);
    }
    else
    {
        if(index < numPanels())
        {
            newElement->setPosition(viewPanelElements[index]->getPosition());
            for(size_t i = index; i < numPanels(); ++i)
            {
                viewPanelElements[index]->anim_move(sf::Vector2f(0.0f, newElement->getBounds().height + listPadding), 1100);
            }
        }
        else
        {
            newElement->setPosition(bounds.left + outerPaddingActual,
                                    viewPanelElements.back()->getBounds().top +
                                    viewPanelElements.back()->getBounds().height + listPadding);
        }
    }

    if(bListItemHighlight)
    {
        newElement->setHighlightableStatus(true);
    }
    if(bDragAndDrop)
    {
        newElement->setDragAndDrop(true);
    }

    CVViewPanel::addPanelElement(newElement, newTag, index);
}

void CVListPanel::removePanelElement(const unsigned int& index)
{

    if(!numPanels()) return;

    if(index > numPanels())
    {
        float shiftY = viewPanelElements.back()->getBounds().height;
        remove(viewPanelElements.back(), selected);
        CVViewPanel::removePanelElement(numPanels() - 1);
    }
    else
    {
        float shiftY = viewPanelElements[index]->getBounds().height;
        remove(viewPanelElements[index], selected);
        CVViewPanel::removePanelElement(index);
        for(size_t i = index; i < numPanels(); ++i)
        {
            viewPanelElements[i]->move(sf::Vector2f(0.0f,-shiftY));
        }
    }
}

void CVListPanel::removePanelElement(CVElement * element)
{

    for(size_t i = 0; i < numPanels(); ++i)
    {
        if(viewPanelElements[i] == element)
        {
            removePanelElement(i);
            return;
        }
    }

}

void CVListPanel::move(const sf::Vector2f& distance)
{
    CVViewPanel::move(distance);
    scrollBarY.move(distance);
}

void CVListPanel::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

CVGridPanel::CVGridPanel(CVView * View,
                         const string& tag,
                         const sf::Vector2f& position,
                         const sf::Vector2f& size,
                         const size_t& gridWidth,
                         const sf::Color& fillColor,
                         const sf::Color& outlineColor,
                         const float& outlineThickness):
                             CVListPanel(View, tag,
                                         fillColor, size, false, position),
                             cPos(0, 0),
                             cRowPanel(nullptr),
                             stGridWidth(gridWidth),
                             fGridItemPadding(8.0f),
                             fGridItemWidth(size.x - 2*outerPadding),
                             bResizeToGrid(true)
{

    if(gridWidth)
    {
        fGridItemWidth = (size.x - 2*outerPadding - fGridItemPadding * (gridWidth - 1)) / gridWidth;
    }

    setOutlineThickness(outlineThickness);
    setOutlineColor(outlineColor);

}

sf::Vector2f CVGridPanel::getGridPosition(const sf::Vector2i& coords) const
{

    sf::Vector2f output;

    if(coords.y < rowPanels.size())
    {
        output.y = rowPanels[coords.y]->getPosition().y;
    }
    else
    {
        int y = coords.y;

        if(!rowPanels.empty())
        {
            output.y = rowPanels.back()->getPosition().y;
            y -= rowPanels.size();
        }
        else
        {
            output.y = getPosition().y + outerPadding;
        }

        output.y += y * (listItemHeight + listPadding);
    }

    output.x = getPosition().x + outerPadding + coords.x * (fGridItemWidth + fGridItemPadding);

    return output;

}

sf::Vector2f CVGridPanel::getGridPosition(const size_t& index) const
{

    if(index < cPos.y * stGridWidth + cPos.x)
    {
        return getGridPosition(sf::Vector2i(index % stGridWidth, index / stGridWidth));
    }
    else
    {
        return getGridPosition(sf::Vector2i(cPos.x, cPos.y));
    }

}

void CVGridPanel::setGridItemSize(const sf::Vector2f& newSize)
{

    listItemHeight = newSize.y;
    fGridItemWidth = newSize.x;

}

void CVGridPanel::setGridItemPadding(const float& newPadding)
{

    fGridItemPadding = newPadding;

}

void CVGridPanel::addPanelElement(CVElement * newElement,
                                  const string& newTag,
                                  const unsigned int& index)
{

    if(!cPos.x)
    {

        float posY = getPosition().y + outerPadding;
        if(cRowPanel)
        {
            posY = cRowPanel->getPosition().y + cRowPanel->getSize().y + listPadding;
        }

        CVBasicViewPanel * newRowPanel = new CVBasicViewPanel(View,
                                                              "",
                                                              sf::Color::Transparent,
                                                              sf::Vector2f(getSize().x - outerPadding * 2,
                                                                           listItemHeight),
                                                              false,
                                                              sf::Vector2f(getPosition().x + outerPadding, posY));

        cRowPanel = newRowPanel;
        rowPanels.emplace_back(cRowPanel);
        newRowPanel->setOutOfBoundsDraw(true);
        newRowPanel->setOutOfBoundsUpdate(true);

        CVListPanel::addPanelElement(newRowPanel);

    }

    if(bResizeToGrid)
    {
        newElement->setSize(sf::Vector2f(fGridItemWidth, listItemHeight));
    }

    newElement->setPosition(getGridPosition(index));
    cRowPanel->addPanelElement(newElement);

    ++cPos.x;
    if(cPos.x == stGridWidth)
    {
        cPos.x = 0;
        ++cPos.y;
    }

}

void CVGridPanel::removePanelElement(CVElement * newElement)
{

    bool bRemoved = false;

    for(size_t y = 0, i = 0, x; y < rowPanels.size(); ++y)
    {
        for(x = 0; x < rowPanels[y]->getElements().size(); ++i)
        {

            if(!bRemoved)
            {
                if(rowPanels[y]->getElements()[x] == newElement)
                {

                    rowPanels[y]->removePanelElement(newElement);

                    if(cPos.x)
                    {
                        --cPos.x;
                    }

                    bRemoved = true;

                }
                else
                {
                    ++x;
                }
            }
            else
            {

                CVElement * shiftElement = rowPanels[y]->getElements()[x];

                if(!x && y && (rowPanels[y-1]->numPanels() < stGridWidth))
                {

                    rowPanels[y]->detachPanelElement(shiftElement);
                    rowPanels[y-1]->addPanelElement(shiftElement);
                }
                else
                {
                    ++x;
                }

                shiftElement->setPosition(getGridPosition(i - 1));

            }

        }
    }

    if(rowPanels.back()->numPanels() == 0)
    {
        CVBasicViewPanel::removePanelElement(rowPanels.back());
        rowPanels.pop_back();

        if(cPos.y)
        {
            --cPos.y;
        }
    }
}

CVElement * CVGridPanel::getGridElement(const std::string& tag)
{

    for(auto& row : rowPanels)
    {
        for(auto& element : row->getElements())
        {
            if(element->tag() == tag)
            {
                return element;
            }
        }
    }

    return nullptr;

}

void CVGridPanel::setOuterPadding(const float& newPadding)
{

    outerPadding = newPadding;
    fGridItemWidth = (getSize().x - 2*outerPadding - (stGridWidth - 1) * fGridItemPadding)/stGridWidth;

    if(bResizeToGrid)
    {

        float lastY;
        for(size_t y = 0, y2, x; y < rowPanels.size(); ++y)
        {
            lastY = rowPanels[y]->getSize().y;
            for(x = 0; x < rowPanels[y]->numPanels(); ++x)
            {
                rowPanels[y]->getPanels()[x]->setSize(sf::Vector2f(fGridItemPadding, listItemHeight));
            }
            rowPanels[y]->fitElements(false, true);
            for(y2 = y + 1; y2 < rowPanels.size(); ++y2)
            {
                rowPanels[y2]->move(0.0f, rowPanels[y]->getSize().y - lastY);
            }
        }

    }

}

}
