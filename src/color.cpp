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

#include "cvision/color.hpp"

#include <hyper/toolkit/string.hpp>

using namespace hyperC;

namespace cvis
{

void contrast(sf::Color& color, const sf::Color& refColor, uint8_t degree)
{

    if(color.r >= refColor.r)
    {
        if((uint16_t)color.r + degree <= 255) color.r += degree;
        else if(color.r >= degree) color.r = refColor.r - degree;
        else color.r = 255;
    }
    else
    {
        if(color.r >= degree) color.r -= degree;
        else if((uint16_t)color.r + degree <= 255) color.r = refColor.r + degree;
        else color.r = 0;
    }

    if(color.g >= refColor.g)
    {
        if((uint16_t)color.g + degree <= 255) color.g += degree;
        else if(color.g >= degree) color.g = refColor.g - degree;
        else color.g = 255;
    }
    else
    {
        if(color.g >= degree) color.g -= degree;
        else if((uint16_t)color.g + degree <= 255) color.g = refColor.g + degree;
        else color.g = 0;
    }

    if(color.b >= refColor.b)
    {
        if((uint16_t)color.b + degree <= 255) color.b += degree;
        else if(color.b >= degree) color.b = refColor.b - degree;
        else color.b = 255;
    }
    else
    {
        if(color.b >= degree) color.b -= degree;
        else if((uint16_t)color.b + degree <= 255) color.b = refColor.b + degree;
        else color.b = 0;
    }
}

void contrast_brightness(sf::Color& color,
                         const sf::Color& other_color,
                         const uint8_t& degree)
{
    unsigned int avg_brightness_a = (unsigned int)color.a + (unsigned int)color.g + (unsigned int)color.b;
    unsigned int avg_brightness_b = (unsigned int)other_color.a + (unsigned int)other_color.g + (unsigned int)other_color.b;

    if(avg_brightness_b > avg_brightness_b)
    {
        color -= sf::Color(degree, degree, degree);
    }
    else
    {
        color += sf::Color(degree, degree, degree);
    }
}

void setImageColor(sf::Image& img, const sf::Color& newColor)
{
    size_t x = 0;
    sf::Vector2u imgSize = img.getSize();
    for(size_t y = 0; y < imgSize.y; ++y)
    {
        for(x = 0; x < imgSize.x; ++x)
        {
            img.setPixel(x, y, newColor);
        }
    }
}

sf::Color textToColor(const std::string& input)
{
    sf::Color output(0,0,0,255);
    StringVector parseBuff;
    splitString(input, parseBuff, hyperC::DELIM_STANDARD);
    std::vector<unsigned int> values;

    if(parseBuff.size() > 1)
    {
        for(auto& str : parseBuff)
        {
            try
            {
                int newVal = std::stoi(str);
                if(newVal < 0) newVal = 0;
                else if(newVal > 255) newVal = 255;
                values.push_back(newVal);
            }
            catch(...) { }
        }
        if(values.size() > 0) output.r = values.front();
        if(values.size() > 1) output.b = values[1];
        if(values.size() > 2) output.g = values[2];
        if(values.size() > 3) output.a = values[3];
    }

    if(values.size() < 1)
    {
        if(cmpString(input, "red")) output = sf::Color::Red;
        else if(cmpString(input, "sky")) output = sf::Color(135,206,235);
        else if(cmpString(input, "navy")) output = sf::Color(0,0,128);
        else if(cmpString(input, "blue")) output = sf::Color(0,0,255);
        else if(cmpString(input, "lime")) output = sf::Color(0,255,0);
        else if(cmpString(input, "olive")) output = sf::Color(128,128,0);
        else if(cmpString(input, "pine")) output = sf::Color(0,100,0);
        else if(cmpString(input, "green")) output = sf::Color(0,128,0);
        else if(cmpString(input, "magenta")) output = sf::Color::Magenta;
        else if(cmpString(input, "purple")) output = sf::Color(128,0,128);
        else if(cmpString(input, "indigo")) output = sf::Color(75,0,130);
        else if(cmpString(input, "violet")) output = sf::Color(238,130,238);
        else if(cmpString(input, "yellow")) output = sf::Color::Yellow;
        else if(cmpString(input, "peach")) output = sf::Color(255,218,185);
        else if(cmpString(input, "white")) output = sf::Color::White;
        else if(cmpString(input, "turquoise")) output = sf::Color(64,224,208);
        else if(cmpStringToList(input, { "aqua", "cyan" })) output = sf::Color(0,183,255);
        else if(cmpStringToList(input, {"ocean", "sea"})) output = sf::Color(0, 119, 190);
        else if(cmpString(input, "orange")) output = sf::Color(255,165,0);
        else if(cmpString(input, "gold")) output = sf::Color(255,215,0);
        else if(cmpString(input, "tomato")) output = sf::Color(255,99,71);
        else if(cmpString(input, "coral")) output = sf::Color(255,127,80);
        else if(cmpString(input, "pink")) output = sf::Color(255,192,203);
        else if(cmpString(input, "maroon")) output = sf::Color(128,0,0);
        else if(cmpString(input, "silver")) output = sf::Color(192,192,192);
        else if(cmpString(input, "slate")) output = sf::Color(112,128,144);
        else if(cmpString(input, "grey")) output = sf::Color(128,128,128);

        if(cmpString(input, "dark")) darken(output, 60);
        else if(cmpStringToList(input, { "strong", "bold", "dim" })) darken(output, 30);
        else if(cmpString(input, "pale")) brighten(output, 30);
        else if(cmpStringToList(input, {"light", "bright"})) brighten(output, 60);

        if(cmpString(input, "translucent")) output.a = 128;
        else if(cmpString(input, "transparent")) output.a = 255;
    }

    return output;

}

ColorTheme::ColorTheme(const std::vector<sf::Color>& colors):
    std::vector<sf::Color>(colors),
    initStates(colors) { }

const sf::Color& ColorTheme::getInitial(const unsigned int& index) const
{
    return initStates[index];
}

const sf::Color& ColorTheme::getInitial(const sf::Color& color) const
{
    for(size_t i = 0; i < size(); ++i)
    {
        if((*this)[i] == color) return initStates[i];
    }
    throw std::invalid_argument("No color matching reference in color theme");
}

void ColorTheme::push_back(const sf::Color& color)
{
    std::vector<sf::Color>::push_back(color);
    initStates.push_back(color);
}

void ColorTheme::operator=(const std::vector<sf::Color>& colors)
{
    std::vector<sf::Color>::operator=(colors);
    initStates = colors;
}

void ColorTheme::erase(std::vector<sf::Color>::iterator it)
{
    unsigned int index = 0;
    for(std::vector<sf::Color>::iterator bit = begin();
            bit != end(); ++bit, ++index)
    {
        if(bit == it)
        {
            initStates.erase(initStates.begin() + index);
            break;
        }
    }
    std::vector<sf::Color>::erase(it);
}
void ColorTheme::insert(std::vector<sf::Color>::iterator it, const sf::Color& value)
{
    unsigned int index = 0;
    for(std::vector<sf::Color>::iterator bit = begin();
            bit != end(); ++bit, ++index)
    {
        if(bit == it)
        {
            initStates.insert(initStates.begin() + index, value);
            break;
        }
    }
    std::vector<sf::Color>::insert(it, value);
}

}
