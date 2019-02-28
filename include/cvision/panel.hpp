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

#ifndef CVIS_PANEL
#define CVIS_PANEL

#include "cvision/viewpanel.hpp"

#include <chrono>

#define CV_CTRL_FWD                 "forward"
#define CV_CTRL_REV                 "back"

#define CV_ASSEMBLE_NONE            0b0
#define CV_ASSEMBLE_HORIZONTAL      0b1
#define CV_ASSEMBLE_VERTICAL        0b10

#define CV_PANEL_CONSTRUCTOR(class)     \
    class(cvis::CVView* parentView, const std::string& panelTag = "",                       \
          const sf::Color& backgroundColor = sf::Color::Transparent,                        \
          const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,      \
          const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f))

#define CV_DEFINE_PANEL_CONSTRUCT(class)    \
    class::class(CVView* parentView, const std::string& panelTag,               \
                 const sf::Color& backgroundColor, const sf::Vector2f& size,    \
                 bool bFitToWindow, const sf::Vector2f& position):              \
                    CVBasicViewPanel(parentView, panelTag, backgroundColor,     \
                                       size, bFitToWindow, position)

#define CV_PANEL_PARAMS \
    CVView* parentView, const std::string& panelTag,               \
    const sf::Color& backgroundColor, const sf::Vector2f& size,    \
    bool bFitToWindow, const sf::Vector2f& position

#define CV_PANEL_PARAM_NAMES \
    parentView, panelTag, backgroundColor,     \
    size, bFitToWindow, position

namespace cvis{

class CVEvent;
class CVView;

class CVISION_API CVBasicViewPanel: public CVViewPanel{
public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API CVBasicViewPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};


}

#endif // CVIS_PANEL
