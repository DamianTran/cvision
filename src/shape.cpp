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

#include "cvision/shape.hpp"
#include "cvision/element.hpp"
#include "cvision/event.hpp"
#include "cvision/algorithm.hpp"

namespace cvis
{

CVShape::CVShape(CVView* View):
    CVElement(View),
    bMasked(false),
    maskAlpha(0)
{

}

void CVShape::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

void CVShape::move(const sf::Vector2f& offset)
{

    CVElement::move(offset);

    if(bStatic) return;

    if(bDropShadow)
    {
        dropShadow.move(offset);
    }

    for(auto& sprite : spriteList)
    {
        sprite.move(offset);
    }

    if(is_closable())
    {
        closeButton->move(offset);
    }

    bounds.left += offset.x;
    bounds.top += offset.y;

    shapeMask.move(offset);
    inactiveMask.move(offset);

}

void CVShape::setSize(const sf::Vector2f& newSize)
{
//    bounds.left -= (newSize.x - getSize().x) * (getOrigin().x / bounds.width);
//    bounds.top -= (newSize.y - getSize().y) * (getOrigin().y / bounds.height);
//    origin.x = newSize.x / bounds.width;
//    origin.y = newSize.y / bounds.height;
    bounds.width = newSize.x;
    bounds.height = newSize.y;
}

void CVShape::setMask(const sf::Texture* texture,
                      const sf::Color& fillColor)
{
    shapeMask.setTexture(texture);
    shapeMask.setOrigin(origin);
    shapeMask.setSize(getSize());
    shapeMask.setRotation(getRotation());
    shapeMask.setPosition(getPosition());

    maskAlpha = fillColor.a;

    if(maskAlpha < getFillColor().a)
    {
        shapeMask.setFillColor(fillColor);
    }
    else
    {
        sf::Color newColor = fillColor;
        newColor.a = getFillColor().a;
        shapeMask.setFillColor(newColor);
    }

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

    maskAlpha = newAlpha;
}

bool CVShape::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVElement::update(event, mousePos)) return false;

    if(bMasked)
    {
        shapeMask.setOrigin(getOrigin());
        shapeMask.setSize(getSize());
        shapeMask.setPosition(getPosition());

        if(bFade && (fadeLayers | CV_LAYER_MASK))
        {

            sf::Color tmp = shapeMask.getFillColor();

            if((targetAlpha > tmp.a) &&
               (tmp.a < maskAlpha))
            {
                if(int(tmp.a) + fadeRate < maskAlpha)
                {
                    tmp.a += fadeRate;
                }
                else tmp.a = maskAlpha;
            }
            else if(targetAlpha < tmp.a)
            {
                if(int(tmp.a) - fadeRate > targetAlpha)
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
    if(!target || !visible) return false;
    return true;
}

}
