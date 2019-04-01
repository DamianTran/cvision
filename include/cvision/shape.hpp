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

#ifndef CVIS_SHAPE
#define CVIS_SHAPE

#include <SFML/Graphics.hpp>

#include "cvision/element.hpp"
#include "cvision/primitives.hpp"

namespace cvis
{

class CVEvent;
class CVView;

class CVISION_API CVShape: public CVElement
{
protected:

    rounded_rectangle shapeMask;
    rounded_rectangle inactiveMask;

    bool bMasked;

public:

    CVISION_API void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

    CVISION_API bool draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API void setMask(const sf::Texture* texture,
                 const sf::Color& fillColor = sf::Color(255,255,255,100));
    CVISION_API void hideMask();
    CVISION_API void showMask();

    inline void setMaskColor(const sf::Color& newColor)
    {
        shapeMask.setFillColor(newColor);
    }
    inline void setMaskColor(const int& r, const int& g, const int& b)
    {
        setMaskColor(sf::Color(r, g, b));
    }

    CVISION_API void setMaskAlpha(const uint8_t& newAlpha);
    CVISION_API void setSpriteColor(const sf::Color& newColor);
    inline void setSpriteColor(const int& r, const int& g, const int& b)
    {
        setSpriteColor(sf::Color(r, g, b));
    }

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y)
    {
        setSize(sf::Vector2f(x, y));
    }

    virtual void sendData(CVEvent& event) const { }

    CVISION_API CVShape(CVView* View);
    virtual ~CVShape() { }

};

}

#endif // CVIS_SHAPE
