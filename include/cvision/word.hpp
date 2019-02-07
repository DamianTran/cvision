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

#ifndef CVIS_WORD
#define CVIS_WORD

#include "cvision/type.hpp"
#include "cvision/widgets.hpp"
#include "cvision/viewpanel.hpp"

namespace cvis{

class CVISION_API CVTypePanel: public CVViewPanel{
protected:
    std::vector<CVTypeBox> typeElements;
    std::vector<float> margins;
    std::vector<sf::RectangleShape> marginLines,
                                    highlight;

    float scrollBarPadding;

    ColorTheme highlightColors;

    std::vector<std::string> waitingHighlightText;
    cvis::CVScrollBar scrollBarY;

    bool bShowMarginLines,
        bCanEdit;

    inline sf::RectangleShape& left_margin_line(){ return marginLines[0]; }
    inline sf::RectangleShape& top_margin_line(){ return marginLines[1]; }
    inline sf::RectangleShape& right_margin_line(){ return marginLines[2]; }
    inline sf::RectangleShape& bottom_margin_line(){ return marginLines[3]; }

public:



    inline const float& left_margin(){ return margins[0]; }
    inline const float& top_margin(){ return margins[1]; }
    inline const float& right_margin(){ return margins[2]; }
    inline const float& bottom_margin(){ return margins[3]; }

    CVISION_API void set_left_margin(const float& newMargin);
    CVISION_API void set_right_margin(const float& newMargin);
    CVISION_API void set_top_margin(const float& newMargin);
    CVISION_API void set_bottom_margin(const float& newMargin);

    CVISION_API void show_margins();
    CVISION_API void hide_margins();

    inline void setMarginLines(const bool& state){ bShowMarginLines = state; }
    inline void setScrollBarPadding(const float& newPadding){ scrollBarPadding = newPadding; }

    CVISION_API bool setHighlight(const std::vector<std::string>& query);
    CVISION_API void setHighlightableStatus(const bool& status);

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API void addTextElement(const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f), textEntry textInfo = textEntry());
    CVISION_API void getText(std::vector<std::string>& output);
    CVISION_API void setText(const std::vector<std::string>& newText);

    CVISION_API void setTextSize(const unsigned int& newSize);
    CVISION_API void setTextColor(const sf::Color& newColor);

    CVISION_API void disable_edit();
    CVISION_API void enable_edit();
    inline const bool& can_edit() const{ return bCanEdit; }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    CVISION_API CVTypePanel(CVView* parentView,
                const textEntry& textInfo,
                const std::string& panelTag = "",
                const sf::Color& color = sf::Color::White,
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                const std::vector<float>& margins = std::vector<float>(4, 25.0f),
                const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

}



#endif // CVIS_WORD
