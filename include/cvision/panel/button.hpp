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

#ifndef CVIS_BUTTON_PANEL
#define CVIS_BUTTON_PANEL

#include "cvision/panel.hpp"

namespace cvis
{

class CVButton;

class CVISION_API CVButtonPanel: public CVBasicViewPanel{
protected:

    std::string interactTag;
    sf::Vector2i gridSize;

    void organizeGrid();
    sf::Vector2f getGridPosition(const sf::Vector2i& coord) const;

    std::vector<std::vector<CVButton*>> buttonGrid;

    float outerPadding,
            innerPadding;

public:

    std::string getInteraction();
    bool checkInteraction() const;

    void setGrid(const sf::Vector2i& size);
    void setGrid(const unsigned int& x, const unsigned int& y);

    void setOuterPadding(const float& value);
    void setInnerPadding(const float& value);

    void addButton(const sf::Vector2i& coord,
                   const std::string& tag = "",
                   const std::string& icon = "",
                   const std::string& text = "",
                   const unsigned int& numStates = 2,
                   const unsigned int& initialState = 0);

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    CVButtonPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_BUTTON_PANEL
