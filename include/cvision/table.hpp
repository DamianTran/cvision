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

#ifndef CVIS_TABLE
#define CVIS_TABLE

#include "cvision/panel.hpp"
#include "cvision/widgets.hpp"


namespace cvis{

class CVTypeBox;
class CVTextBox;

class CVISION_API CVTablePanel : public CVBasicViewPanel{
protected:

    CVScrollBar scrollBarX,
                scrollBarY;

    std::vector<std::vector<CVTypeBox*>> cells;
    std::vector<CVTextBox*> headerBars,
                            rowBars;

    size_t outerPanelCacheDist; // Distance in cells of the panel destruction boundary

    sf::Vector2f cellSize,
                sizeOffset;

    float cellFontSize,
            headerFontSize,
            cellOutlineWidth;

    sf::Color cellFillColor,
                cellOutlineColor,
                cellHighlightColor,
                headerThemeColor;

    std::vector<float> margins;

    std::vector<unsigned int> view_coords;

    bool bHighlightCells;

    CVISION_API void updateExpand();

public:

    inline const float& left_margin() const{ return margins[0]; }
    inline const float& top_margin() const{ return margins[1]; }
    inline const float& right_margin() const{ return margins[2]; }
    inline const float& bottom_margin() const{ return margins[3]; }

    CVISION_API void setCellFillColor(const sf::Color& newColor);
    CVISION_API void setCellOutlineColor(const sf::Color& newColor);
    CVISION_API void setCellHighlightColor(const sf::Color& newColor);
    CVISION_API void setCellHighlightable(const bool& status = true);

    CVISION_API void setHeaderColor(const sf::Color& newColor);

    CVISION_API void addCell(const std::string& text,
                 const unsigned int& x, const unsigned int& y);
    CVISION_API void clear();

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API void expandCols(const int& y);
    CVISION_API void expandRows(const int& x);

    CVISION_API void removeRow(const unsigned int& y);
    CVISION_API void removeCol(const unsigned int& x);

    CVISION_API void insertRow(const unsigned int& y);
    CVISION_API void insertCol(const unsigned int& x);

    CVISION_API void move(const sf::Vector2f& distance);
    CVISION_API void move(const float& x, const float& y);

    CVISION_API void setPosition(const sf::Vector2f& newPos);
    CVISION_API void setPosition(const float& x, const float& y);

    CVISION_API CVTablePanel(CVView* parentView, const std::string& tag = "",
                 const sf::Color& bkgColor = sf::Color::Transparent,
                 const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                 const bool& bFitWindow = true,
                 const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};



}

#endif // CVIS_TABLE
