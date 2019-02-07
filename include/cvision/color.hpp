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

#ifndef CVIS_COLOR
#define CVIS_COLOR

#include <vector>

#include "cvision/lib.hpp"
#include <SFML/Graphics.hpp>

namespace cvis
{

class CVISION_API ColorTheme: public std::vector<sf::Color>{
protected:
    std::vector<sf::Color> initStates;

public:

    CVISION_API const sf::Color& getInitial(const unsigned int& index) const;
    CVISION_API const sf::Color& getInitial(const sf::Color& color) const;

    template<class... Args>
    void emplace_back(Args&&... args){
        std::vector<sf::Color>::emplace_back(std::forward<Args>(args)...);
        initStates.emplace_back(std::forward<Args>(args)...);
    }
    CVISION_API void push_back(const sf::Color& color);

    CVISION_API void operator=(const std::vector<sf::Color>& colors);

    CVISION_API void erase(std::vector<sf::Color>::iterator it);
    CVISION_API void insert(std::vector<sf::Color>::iterator it, const sf::Color& value);

    CVISION_API void pop_back();

    ColorTheme(){ }
    CVISION_API ColorTheme(const std::vector<sf::Color>& colors);

};

template<typename T>
sf::Color operator-(const sf::Color& color, const T& other){

    return sf::Color(color.r - other,
                     color.g - other,
                     color.b - other,
                     color.a);

}

template<typename T>
sf::Color operator+(const sf::Color& color, const T& other){

    return sf::Color(color.r + other,
                     color.g + other,
                     color.b + other,
                     color.a);

}

template<typename T>
void operator-=(sf::Color& color, const T& other){
    color.r -= other;
    color.g -= other;
    color.b -= other;
}

template<typename T>
void operator+=(sf::Color& color, const T& other){
    color.r += other;
    color.g += other;
    color.b += other;
}

inline sf::Color setAlpha(const sf::Color& color, const uint8_t& newAlpha){
    return sf::Color(color.r, color.g, color.b, newAlpha);
}

inline void brighten(sf::Shape* shape, uint8_t degree, bool border = true){
    sf::Color color(shape->getFillColor());
    color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
    color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
    color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
        color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
        color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
        shape->setOutlineColor(color);
    }
};

inline void brighten(sf::Color& color, uint8_t degree){
    color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
    color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
    color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
}

inline void darken(sf::Shape* shape, uint8_t degree, bool border = true){
    sf::Color color(shape->getFillColor());
    color.r = (int16_t)color.r >= (int16_t)degree ? color.r - degree : (uint16_t)0;
    color.g = (int16_t)color.g >= (int16_t)degree ? color.g - degree : (uint16_t)0;
    color.b = (int16_t)color.b >= (int16_t)degree ? color.b - degree : (uint16_t)0;
    shape->setFillColor(color);
    if(border){
        color = shape->getOutlineColor();
        color.r = (int16_t)color.r >= (int16_t)degree ? color.r - degree : (uint16_t)0;
        color.g = (int16_t)color.g >= (int16_t)degree ? color.g - degree : (uint16_t)0;
        color.b = (int16_t)color.b >= (int16_t)degree ? color.b - degree : (uint16_t)0;
        shape->setOutlineColor(color);
    }
};

inline void darken(sf::Color& color, uint8_t degree){
    color.r = (int16_t)color.r >= (int16_t)degree ? color.r - degree : (uint16_t)0;
    color.g = (int16_t)color.g >= (int16_t)degree ? color.g - degree : (uint16_t)0;
    color.b = (int16_t)color.b >= (int16_t)degree ? color.b - degree : (uint16_t)0;
}

inline void contrast(sf::Color& color, const sf::Color& refColor, uint8_t degree){

    if(color.r >= refColor.r){
        if((uint16_t)color.r + degree <= 255) color.r += degree;
        else if(color.r >= degree) color.r = refColor.r - degree;
        else color.r = 255;
    }
    else{
        if(color.r >= degree) color.r -= degree;
        else if((uint16_t)color.r + degree <= 255) color.r = refColor.r + degree;
        else color.r = 0;
    }

    if(color.g >= refColor.g){
        if((uint16_t)color.g + degree <= 255) color.g += degree;
        else if(color.g >= degree) color.g = refColor.g - degree;
        else color.g = 255;
    }
    else{
        if(color.g >= degree) color.g -= degree;
        else if((uint16_t)color.g + degree <= 255) color.g = refColor.g + degree;
        else color.g = 0;
    }

    if(color.b >= refColor.b){
        if((uint16_t)color.b + degree <= 255) color.b += degree;
        else if(color.b >= degree) color.b = refColor.b - degree;
        else color.b = 255;
    }
    else{
        if(color.b >= degree) color.b -= degree;
        else if((uint16_t)color.b + degree <= 255) color.b = refColor.b + degree;
        else color.b = 0;
    }
}

inline void red_hue(sf::Shape* shape, uint8_t degree, uint8_t descrimination = 0, bool border = true){
    sf::Color color(shape->getFillColor());
    color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
    if(descrimination > 0){
        color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
        color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
    }
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.r = (uint16_t)color.r + (uint16_t)degree <= (uint16_t)255 ? color.r + degree : (uint16_t)255;
        if(descrimination > 0){
            color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
            color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
        }
        shape->setOutlineColor(color);
    }
};
inline void blue_hue(sf::Shape* shape, uint8_t degree, uint8_t descrimination = 0, bool border = true){
    sf::Color color(shape->getFillColor());
    color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
    if(descrimination > 0){
        color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
        color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
    }
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.b = (uint16_t)color.b + (uint16_t)degree <= (uint16_t)255 ? color.b + degree : (uint16_t)255;
        if(descrimination > 0){
            color.g = (int16_t)color.g >= (int16_t)descrimination ? color.g - descrimination : 0;
            color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
        }
        shape->setOutlineColor(color);
    }
};
inline void green_hue(sf::Shape* shape, uint8_t degree, uint8_t descrimination = 0, bool border = true){
    sf::Color color(shape->getFillColor());
    color.g = (uint16_t)color.g + (uint16_t)degree <= (uint16_t)255 ? color.g + degree : (uint16_t)255;
    if(descrimination > 0){
        color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
        color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
    }
    shape->setFillColor(color);

    if(border){
        color = shape->getOutlineColor();
        color.g = (uint16_t)color.g + (uint16_t)degree <= 255 ? color.g + degree : 255;
        if(descrimination > 0){
            color.r = (int16_t)color.r >= (int16_t)descrimination ? color.r - descrimination : 0;
            color.b = (int16_t)color.b >= (int16_t)descrimination ? color.b - descrimination : 0;
        }
        shape->setOutlineColor(color);
    }
};

inline void colorSwatch(ColorTheme& colors, size_t numColors){
    colors.reserve(colors.size() + numColors);
    size_t L = numColors/2;
    uint8_t colGradient = numColors > uint8_t(8) ? uint8_t(255)/numColors : uint8_t(255)/uint8_t(8);
    for(size_t i = 0; i < L; ++i){
        colors.push_back(colors.front());
        brighten(colors.back(), colGradient);
    }

    L = numColors - L;
    for(size_t i = 0; i < L; ++i){
        colors.push_back(colors.front());
        darken(colors.back(), colGradient);
    }
}

CVISION_API void setImageColor(sf::Image& img, const sf::Color& newColor); // Flush image to one color

CVISION_API sf::Color textToColor(const std::string& input);

}

#endif // CVIS_COLOR
