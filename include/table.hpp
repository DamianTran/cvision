//
// CVision: a multi-platform graphics interface libary for C++
//
//////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2018 Damian Tran
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
// Redistribution of CVision is permitted under accordance with
// the GNU general public licence (GPL) version 3.0 and the
// terms and conditions specified in CVlicence.txt

#pragma once

#ifndef CVIS_TABLE
#define CVIS_TABLE

#include "cvision/panel.hpp"
#include "cvision/widgets.hpp"
#include "cvision/cvis_base.hpp"

namespace cvis{

class CVTablePanel : public CVBasicViewPanel{
protected:

    CVScrollBar scrollBarX,
                scrollBarY;

    std::vector<std::vector<CVTextBox*>> cells;

    size_t outerPanelCacheDist; // Distance in cells of the panel destruction boundary

    sf::Vector2f cellSize;

    float cellFontSize,
            headerFontSize,
            cellOutlineWidth;

    sf::Color cellFillColor,
                cellOutlineColor,
                cellHighlightColor;

    std::vector<float> margins;

    std::vector<unsigned int> view_coords;

public:

    inline const float& left_margin() const{ return margins[0]; }
    inline const float& top_margin() const{ return margins[1]; }
    inline const float& right_margin() const{ return margins[2]; }
    inline const float& bottom_margin() const{ return margins[3]; }

    void setCellFillColor(const sf::Color& newColor);
    void setCellOutlineColor(const sf::Color& newColor);
    void setCellHighlightColor(const sf::Color& newColor);

    void addCell(const std::string& text,
                 const unsigned int& x, const unsigned int& y);

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void expandCols(const int& y);
    void expandRows(const int& x);

    void removeRow(const unsigned int& y);
    void removeCol(const unsigned int& x);

    void insertRow(const unsigned int& y);
    void insertCol(const unsigned int& x);

    void move(const sf::Vector2f& distance);
    void move(const float& x, const float& y);

    void setPosition(const sf::Vector2f& newPos);
    void setPosition(const float& x, const float& y);

    CVTablePanel(CVView* parentView, const std::string& tag = "",
                 const sf::Color& bkgColor = sf::Color::Transparent,
                 const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                 const bool& bFitWindow = true,
                 const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};



}

#endif // CVIS_TABLE
