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
// warranties.  By using CVision, you ackowledge and agree
// to this disclaimer.  Use of CVision in Users's programs
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

#include "cvision/panel/toggle.hpp"

namespace cvis
{

CVTogglePanel::CVTogglePanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         active_index(0)
{



}

bool CVTogglePanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVTextBox::update(event, mousePos))
    {
        return false;
    }

    if(active_index < numPanels())
    {
        return viewPanelElements[active_index]->update(event, mousePos);
    }

    return false;
}

bool CVTogglePanel::draw(sf::RenderTarget* target)
{

    if(!CVTextBox::draw(target))
    {
        return false;
    }

    if(active_index < numPanels())
    {
        return viewPanelElements[active_index]->draw(target);
    }

    return false;
}

void CVTogglePanel::switch_to(const size_t& index)
{
    if(index < numPanels())
    {
        active_index = index;
    }

}

void CVTogglePanel::switch_to(const std::string& tag)
{

    for(size_t i = 0; i < numPanels(); ++i)
    {
        if(viewPanelTags[i] == tag)
        {
            switch_to(i);
            return;
        }
    }

}


}
