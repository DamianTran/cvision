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

#pragma once

#ifndef CVIS_LIST_PANEL
#define CVIS_LIST_PANEL

#include "cvision/panel.hpp"
#include "cvision/widgets.hpp"

namespace cvis
{

/** Scrollable list panel

    New list items are automatically positioned below the last one,
    but not resized */

class CVISION_API CVListPanel : public CVBasicViewPanel
{
public:

    CVISION_API CVListPanel(CVView* parentView,
                            std::string panelTag = "",
                            sf::Color backgroundColor = sf::Color::Transparent,
                            const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                            bool bFitToWindow = true,
                            const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    std::vector<CVElement*> selected;

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    inline const float& getListPadding() const
    {
        return listPadding;
    }
    inline const float& getScrollBarPadding() const
    {
        return scrollBarPadding;
    }
    inline const float& getOuterPadding() const
    {
        return outerPadding;
    }
    inline const float& getInnerPadding() const
    {
        return innerPadding;
    }

    CVISION_API void setListPadding(const float& newPadding);
    CVISION_API void setScrollBarPadding(const float& newPadding);
    CVISION_API void setOuterPadding(const float& newPadding);
    CVISION_API void setInnerPadding(const float& newPadding);

    CVISION_API void setDragAndDrop(const bool& state);
    CVISION_API void setListHighlightable(const bool& state = true);

    inline const sf::Color& getListItemColor() const
    {
        return colorTheme[listItemColorIndex];
    }
    CVISION_API void setListItemColor(const sf::Color& newColor);
    inline const sf::Color& getListHighlightColor() const
    {
        return colorTheme[listItemHighlightColorIndex];
    }
    CVISION_API void setListHighlightColor(const sf::Color& newColor);

    CVISION_API void addTextEntry(const std::string& newText, const unsigned int& index = UINT_MAX);
    CVISION_API void addTextEntries(const std::vector<std::string>& newText);
    CVISION_API void setTextEntries(const std::vector<std::string>& newText);
    CVISION_API void removeTextEntry(const unsigned int& index);
    CVISION_API bool TextEntryExists(const std::string& text) const;

    CVISION_API void setElementPadding(const float& newPadding);

    CVISION_API void setSelectionStatus(const bool& status);
    inline const bool& canSelect() const
    {
        return bSelect;
    }
    CVISION_API void toggleSelection(CVElement* element);
    CVISION_API void toggleSelection(const std::string& tag);
    CVISION_API void setSelection(CVElement* element, const bool& state = true);
    CVISION_API void setSelection(const std::string& tag, const bool& state = true);
    CVISION_API std::vector<std::string> getSelection() const;

    CVISION_API void clearSelection();

    CVISION_API void addPanelElement(CVElement* element,
                                     const std::string& newTag = "",
                                     const unsigned int& index = UINT_MAX) override;
    CVISION_API void removePanelElement(const unsigned int& index);
    CVISION_API void removePanelElement(CVElement * element);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

protected:

    float       listPadding;
    float       scrollBarPadding;
    float       outerPadding;
    float       innerPadding;
    float       listItemHeight;

    CVScrollBar scrollBarY;

    bool        bSelect;
    bool        bListItemHighlight;

    std::vector<std::string> listTags;

    unsigned int listItemColorIndex;
    unsigned int listItemHighlightColorIndex;

};

/** Scrollable grid panel

    Adaptation of list panel where, similarly to the parent class,
    new items are automatically positioned after the last.  Here,
    new items are resized by default to fit (fGridItemWidth, listItemHeight), and
    both vertically and horizontally repositioned.  This behaviour
    can be overwritten by changing bResizeToGrid. */

class CVISION_API CVGridPanel : public CVListPanel
{
public:

    CVISION_API CVGridPanel(CVView * View,
                const std::string& tag,
                const sf::Vector2f& position,
                const sf::Vector2f& size,
                const size_t& gridWidth,
                const sf::Color& fillColor,
                const sf::Color& outlineColor,
                const float& outlineThickness);

    CVISION_API void addPanelElement(CVElement * newElement,
                                     const std::string& newTag = "",
                                     const unsigned int& index = UINT_MAX) override;
    CVISION_API void removePanelElement(CVElement * element) override;
    CVISION_API CVElement * getGridElement(const std::string& tag);

    CVISION_API sf::Vector2f getGridPosition(const sf::Vector2i& coords) const;
    CVISION_API sf::Vector2f getGridPosition(const size_t& index) const;
    inline sf::Vector2f getGridItemSize() const noexcept{ return sf::Vector2f(fGridItemWidth, listItemHeight); }
    inline void setGridItemsSquared() noexcept{ listItemHeight = fGridItemWidth; }

    CVISION_API void setGridWidth(const size_t& newWidth);
    CVISION_API void setGridItemSize(const sf::Vector2f& newSize);
    CVISION_API void setGridItemPadding(const float& newPadding);

    CVISION_API void setOuterPadding(const float& newPadding);

protected:

    size_t stGridWidth;

    float fGridItemPadding;
    float fGridItemWidth;

    bool bResizeToGrid;

    std::vector<CVBasicViewPanel*> rowPanels;

private:

    sf::Vector2i cPos;
    CVBasicViewPanel * cRowPanel;

};

}

#endif // CVIS_LIST_PANEL
