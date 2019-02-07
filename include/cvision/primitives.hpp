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

#pragma once

#ifndef CVIS_PRIMITIVE
#define CVIS_PRIMITIVE

#include <SFML/Graphics.hpp>

#include "cvision/lib.hpp"

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

namespace cvis{

class CVISION_API rounded_rectangle : public sf::Shape{
private:

    unsigned int pointCount, arcPointCount;
    std::vector<sf::Vector2f> points;
    float radius;

    std::vector<bool> rounded_corner;

    CVISION_API void trace(const sf::Vector2f& size,
               const float& radius,
               const unsigned int& apc);

public:

    virtual size_t getPointCount() const;
    virtual sf::Vector2f getPoint(size_t index) const;

    CVISION_API void setRounding(const float& newRadius,
                     const unsigned int& newPointCount = 10,
                     const std::vector<bool>& states =
                     std::vector<bool>(4, true));

    CVISION_API void setSize(const sf::Vector2f& newSize);
    template<typename T>
    void setSize(const T& x, const T& y){
        setSize(sf::Vector2f(x, y));
    }
    CVISION_API sf::Vector2f getSize() const;

    inline const float& getRoundingRadius() const{
        return radius;
    }
    inline const unsigned int& getArcPointCount() const{
        return arcPointCount;
    }

    CVISION_API bool cornerIsRounded(const unsigned int& corner_idx) const;

    CVISION_API bool topLeftRounded() const;
    CVISION_API bool topRightRounded() const;
    CVISION_API bool bottomRightRounded() const;
    CVISION_API bool bottomLeftRounded() const;

    CVISION_API void setCornerRounding(const unsigned int& corner_idx,
                           const bool& state = true);

    CVISION_API void setTopLeftRounded(const bool& state = true);
    CVISION_API void setTopRightRounded(const bool& state = true);
    CVISION_API void setBottomRightRounded(const bool& state = true);
    CVISION_API void setBottomLeftRounded(const bool& state = true);

    CVISION_API void setAllRoundState(const bool& state);

    CVISION_API rounded_rectangle(const sf::Vector2f& size,
                      const float& rounding_radius = 0.0f,
                      const sf::Color& fill_color = sf::Color::White,
                      const unsigned int& arc_point_count = 10);
    CVISION_API rounded_rectangle();
};

}
#endif // CVIS_PRIMITIVE
