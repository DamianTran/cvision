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

class CVISION_API CVListPanel : public CVBasicViewPanel{
protected:

    float listPadding,
            scrollBarPadding,
            outerPadding,
            innerPadding,
            listItemHeight;

    CVScrollBar scrollBarY;

    bool bSelect;
    bool bListItemHighlight;

    std::vector<std::string> listTags;
    unsigned int listItemColorIndex;
    unsigned int listItemHighlightColorIndex;

public:

    std::vector<CVElement*> selected;

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    inline const float& getListPadding() const{ return listPadding; }
    inline const float& getScrollBarPadding() const{ return scrollBarPadding; }
    inline const float& getOuterPadding() const{ return outerPadding; }
    inline const float& getInnerPadding() const{ return innerPadding; }

    CVISION_API void setListPadding(const float& newPadding);
    CVISION_API void setScrollBarPadding(const float& newPadding);
    CVISION_API void setOuterPadding(const float& newPadding);
    CVISION_API void setInnerPadding(const float& newPadding);

    CVISION_API void setDragAndDrop(const bool& state);
    CVISION_API void setListHighlightable(const bool& state = true);

    inline const sf::Color& getListItemColor() const{ return colorTheme[listItemColorIndex]; }
    CVISION_API void setListItemColor(const sf::Color& newColor);
    inline const sf::Color& getListHighlightColor() const{ return colorTheme[listItemHighlightColorIndex]; }
    CVISION_API void setListHighlightColor(const sf::Color& newColor);

    CVISION_API void addTextEntry(const std::string& newText, const unsigned int& index = UINT_MAX);
    CVISION_API void addTextEntries(const std::vector<std::string>& newText);
    CVISION_API void setTextEntries(const std::vector<std::string>& newText);
    CVISION_API void removeTextEntry(const unsigned int& index);
    CVISION_API bool textEntryExists(const std::string& text) const;

    CVISION_API void setElementPadding(const float& newPadding);

    CVISION_API void setSelectionStatus(const bool& status);
    inline const bool& canSelect() const{ return bSelect; }
    CVISION_API void toggleSelection(CVElement* element);
    CVISION_API void toggleSelection(const std::string& tag);
    CVISION_API void setSelection(CVElement* element, const bool& state = true);
    CVISION_API void setSelection(const std::string& tag, const bool& state = true);
    CVISION_API std::vector<std::string> getSelection() const;

    CVISION_API void clearSelection();

    CVISION_API void addPanelElement(CVElement*, std::string newTag = "", const unsigned int& index = UINT_MAX);
    CVISION_API void removePanelElement(const unsigned int& index);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API CVListPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

}

#endif // CVIS_LIST_PANEL
