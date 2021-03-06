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

#include "cvision/panel.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include "hyper/algorithm.hpp"
#include "hyper/toolkit/string.hpp"

#include <boost/range/adaptor/reversed.hpp>

using namespace hyperC;

namespace cvis
{

CVBasicViewPanel::CVBasicViewPanel(CVView* parentView,
                                   const std::string& panelTag,
                                   const sf::Color& backgroundColor,
                                   const sf::Vector2f& size,
                                   const bool& bFitToWindow,
                                   const sf::Vector2f& position):
    CVViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position)
{



}

bool CVBasicViewPanel::draw(sf::RenderTarget* target)
{

    if(!CVShape::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    if(!bSpriteOnly)
    {
        for(auto& item : panel)
        {
            target->draw(item);
        }
    }

    if(bMasked) target->draw(shapeMask);

    for(auto& sprite : spriteList)
    {
        target->draw(sprite);
    }

    for(auto& text : displayText)
    {
        target->draw(text);
    }

    if(bReverseDrawOrder)
    {
        for(auto& panel : boost::adaptors::reverse(viewPanelElements))
        {
            if(bOutOfBoundsDraw || getBounds().intersects(panel->getBounds())) panel->draw(target);
        }
    }
    else
    {
        for(auto& panel : viewPanelElements)
        {
            if(bOutOfBoundsDraw || getBounds().intersects(panel->getBounds())) panel->draw(target);
        }
    }

    if(is_closable())
    {
        closeButton->draw(target);
    }

    if(!active)
    {
        target->draw(inactiveMask);
    }

    if(bHasShadow)
    {
        if(View->viewPort && View->viewPort->isOpen())
        {
            View->viewPort->draw(shadow);
        }
        else
        {
            target->draw(shadow);
        }
    }

    CV_DRAW_CLIP_END

    return true;
}

bool CVBasicViewPanel::update(CVEvent& event, const sf::Vector2f& mousePos)  // Disperse update function
{

    if(is_closable() && closeButton->getTrigger())
    {
        bDelete = true;
    }
    else
    {

        if(!CVViewPanel::update(event, mousePos) || !active) return false;

        updatePanels(event, mousePos);

    }

    return true;
}

}
