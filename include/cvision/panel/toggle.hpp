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

#ifndef CVIS_PANEL_TOGGLE
#define CVIS_PANEL_TOGGLE

#include "cvision/panel.hpp"

/** ========================================================================

    Panel with toggleable display elements

======================================================================== **/

namespace cvis
{

class CVISION_API CVTogglePanel : public CVBasicViewPanel
{
public:

    CVISION_API CVTogglePanel(CVView* parentView,
                              const std::string& panelTag = "",
                              const sf::Color& backgroundColor = sf::Color::Transparent,
                              const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                              const bool& bFitToWindow = true,
                              const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& position) override;

    CVISION_API void switch_to(const size_t& index);
    CVISION_API void switch_to(const std::string& tag);

    inline void setUpdateNonFocusedPanels(const bool& status) noexcept{ bUpdateNonFocused = status; }

    CVISION_API void addPanelElement(CVElement* newElement,
                                     const std::string& tag = "",
                                     const unsigned int& index = UINT_MAX) override;

    inline const size_t& getActiveIndex() const noexcept{ return active_index; }

protected:

    size_t  active_index;

    bool    bUpdateNonFocused;

};

}

#endif // CVIS_PANEL_TOGGLE
