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

#include "cvision/shape.hpp"
#include "cvision/element.hpp"
#include "cvision/event.hpp"

namespace cvis
{

CVShape::CVShape(CVView* View):
    CVElement(View),
    bMasked(false)
{

}

void CVShape::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

void CVShape::move(const sf::Vector2f& offset)
{

    if(bStatic) return;

    for(auto& sprite : spriteList)
    {
        sprite.move(offset);
    }

    bounds.left += offset.x;
    bounds.top += offset.y;

    shapeMask.move(offset);
    inactiveMask.move(offset);

}

void CVShape::setMask(const sf::Texture* texture,
                      const sf::Color& fillColor)
{
    shapeMask.setTexture(texture);
    shapeMask.setOrigin(origin);
    shapeMask.setSize(getSize());
    shapeMask.setRotation(getRotation());
    shapeMask.setPosition(getPosition());
    shapeMask.setFillColor(fillColor);
    bMasked = true;
}

void CVShape::hideMask()
{
    bMasked = false;
}

void CVShape::showMask()
{
    bMasked = true;
}

void CVShape::setSpriteColor(const sf::Color& newColor)
{
    CVElement::setColor(newColor);
}

void CVShape::setMaskAlpha(const uint8_t& newAlpha)
{
    sf::Color tmp = shapeMask.getFillColor();
    tmp.a = newAlpha;
    shapeMask.setFillColor(tmp);
}

bool CVShape::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVElement::update(event, mousePos)) return false;
    if(bMasked)
    {
        shapeMask.setSize(getSize());
        shapeMask.setPosition(getPosition());

        sf::Color tmp = shapeMask.getFillColor();
        if(targetAlpha > tmp.a)
        {
            if(tmp.a + fadeRate < targetAlpha)
            {
                tmp.a += fadeRate;
            }
            else tmp.a = targetAlpha;
        }
        else
        {
            if(tmp.a - fadeRate > targetAlpha)
            {
                tmp.a -= fadeRate;
            }
            else
            {
                tmp.a = targetAlpha;
            }
        }
        shapeMask.setFillColor(tmp);
    }
    else if(!active)
    {
        inactiveMask.setSize(getSize());
        inactiveMask.setPosition(getPosition());
        inactiveMask.setFillColor(inactiveColor());
    }


    return true;
}

bool CVShape::draw(sf::RenderTarget* target)
{
    if(target == nullptr) return false;
    if(!visible) return false;
    return true;
}

}
