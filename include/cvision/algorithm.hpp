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

#ifndef CVIS_ALGORITHM
#define CVIS_ALGORITHM

#include "cvision/lib.hpp"

#if defined WIN32 || defined _WIN32
#include <windows.h>
#endif

#include "hyper/algorithm.hpp"
#include <SFML/Graphics.hpp>

// Dimension space

#define DIMENSION_X                     0u
#define DIMENSION_Y                     1u
#define DIMENSION_Z                     2u

#define CV_DIRECTION_NONE               0b0
#define CV_DIRECTION_X                  0b1
#define CV_DIRECTION_Y                  0b10
#define CV_DIRECTION_Z                  0b100

#define DIMX                            0u
#define DIMY                            1u
#define DIMZ                            2u

// Operator overloads

namespace cvis
{

class CVElement;

template<typename T1, typename T2> constexpr bool operator==(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    return ((T1)RHS.x == LHS.x) && ((T1)RHS.y == LHS.y);
}

template<typename T1, typename T2> constexpr bool operator!=(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    return ((T1)RHS.x != LHS.x) || ((T1)RHS.y != LHS.y);
}

template<typename T1, typename T2> constexpr sf::Vector2<T1> operator+(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    return sf::Vector2<T1>(LHS.x + RHS.x, LHS.y + RHS.y);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator-(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    return sf::Vector2<T1>(LHS.x - RHS.x, LHS.y - RHS.y);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator*(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    return sf::Vector2<T1>(LHS.x * RHS.x, LHS.y * RHS.y);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator/(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    return sf::Vector2<T1>(LHS.x / RHS.x, LHS.y / RHS.y);
}

template<typename T1, typename T2> constexpr sf::Vector2<T1> operator+(const sf::Vector2<T1>& LHS, const T2& RHS)
{
    return sf::Vector2<T1>(LHS.x + RHS, LHS.y + RHS);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator-(const sf::Vector2<T1>& LHS, const T2& RHS)
{
    return sf::Vector2<T1>(LHS.x - RHS, LHS.y - RHS);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator*(const sf::Vector2<T1>& LHS, const T2& RHS)
{
    return sf::Vector2<T1>(LHS.x * RHS, LHS.y * RHS);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator/(const sf::Vector2<T1>& LHS, const T2& RHS)
{
    return sf::Vector2<T1>(LHS.x / RHS, LHS.y / RHS);
}

template<typename T1, typename T2> inline void operator+=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    LHS.x += T1(RHS.x);
    LHS.y += T1(RHS.y);
}
template<typename T1, typename T2> inline void operator-=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    LHS.x -= T1(RHS.x);
    LHS.y -= T1(RHS.y);
}
template<typename T1, typename T2> inline void operator*=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    LHS.x *= T1(RHS.x);
    LHS.y *= T1(RHS.y);
}
template<typename T1, typename T2> inline void operator/=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS)
{
    LHS.x /= T1(RHS.x);
    LHS.y /= T1(RHS.y);
}

template<typename T1, typename T2> inline void operator+=(sf::Vector2<T1>& LHS, const T2& RHS)
{
    LHS.x += T1(RHS);
    LHS.y += T1(RHS);
}
template<typename T1, typename T2> inline void operator-=(sf::Vector2<T1>& LHS, const T2& RHS)
{
    LHS.x -= T1(RHS);
    LHS.y -= T1(RHS);
}
template<typename T1, typename T2> inline void operator*=(sf::Vector2<T1>& LHS, const T2& RHS)
{
    LHS.x *= T1(RHS);
    LHS.y *= T1(RHS);
}
template<typename T1, typename T2> inline void operator/=(sf::Vector2<T1>& LHS, const T2& RHS)
{
    LHS.x /= T1(RHS);
    LHS.y /= T1(RHS);
}

// Functions

template<typename T> std::ostream& operator<<(std::ostream& output, const sf::Vector2<T>& V){
    output << '[' << V.x << ',' << V.y << ']';
    return output;
}

template<typename T> std::ostream& operator<<(std::ostream& output, const sf::Rect<T>& box){
    output << "Rect (Left: " << box.left << ";Top: " << box.top << ";Width: " << box.width << ";Height: " << box.height << ')';
    return output;
}

// Vector2 arithmetic and logical operators

template<typename T>
bool operator<(const sf::Vector2<T>& first, const sf::Vector2<T>& second){
    return(first.x*first.y < second.x*second.y);
}

template<typename T>
bool operator>(const sf::Vector2<T>& first, const sf::Vector2<T>& second){
    return(first.x*first.y > second.x*second.y);
}

template<typename T>
bool operator<=(const sf::Vector2<T>& first, const sf::Vector2<T>& second){
    return !(first > second);
}

template<typename T>
bool operator>=(const sf::Vector2<T>& first, const sf::Vector2<T>& second){
    return !(first < second);
}

template<typename T> bool contains(const sf::Rect<T>& inner, const sf::Rect<T>& outer){
    return(
        (inner.left > outer.left) &&
        (inner.top > outer.top) &&
        (inner.left + inner.width < outer.left + outer.width) &&
        (inner.top + inner.height < outer.top + outer.height));
}

CVISION_API void alignText(std::vector<sf::Text>& textItems, std::vector<sf::Vector2f> coords);
CVISION_API void alignShapes(std::vector<sf::Shape*>& textItems, std::vector<sf::Vector2f> coords);

template<typename T1, typename T2> bool addUnique(T1& item, std::vector<T2>& vectorItem, const unsigned int& position = UINT_MAX){
    for(auto& V : vectorItem){
        if(V == item) return false;
    }
    if(position == UINT_MAX){
        vectorItem.push_back(item);
    }
    else{
        vectorItem.insert(vectorItem.begin() + position, item);
    }
    return true;
}

template<typename T1, typename T2> void eraseItem(const T1& item, std::vector<T2>& vectorItem){
    for(typename std::vector<T2>::iterator it = vectorItem.begin();
        it != vectorItem.end();){
            if(*it == item) vectorItem.erase(it);
            else ++it;
        }
}

CVISION_API void sigFigs(std::string& numeric);
CVISION_API std::string sigFigs(const float& number);


inline sf::Vector2f getOffset(sf::Vector2f source, sf::Vector2f target){
    return sf::Vector2f(target.x - source.x, target.y - source.y);
}

inline float getElapsedTime(std::chrono::high_resolution_clock::time_point& t0,
                     std::chrono::high_resolution_clock::time_point t1){
    return std::chrono::duration_cast<std::chrono::duration<float>>(t1-t0).count();
}

inline float getDistance(const sf::Vector2f& source, const sf::Vector2f& target){
    return sqrt(pow(target.x - source.x, 2) + pow(target.y - source.y, 2));
}

sf::Vector2f getTextHorizontalAlignment(sf::Text& text, sf::Rect<float>& bounds,
                         uint8_t alignment, float borderPadding = 12.0f);

inline float getTextCenterOffsetY(const sf::Text& text){
    if(!text.getFont()) return 0.0f;
    return text.getFont()->getGlyph(97, text.getCharacterSize(), false).bounds.height/2 +
            text.getFont()->getLineSpacing(text.getCharacterSize())/4;
}
inline float getLineHeight(const sf::Text& text)
{
    if(!text.getFont()) return 0.0f;
    return text.getFont()->getGlyph(65, text.getCharacterSize(), false).bounds.height +
            text.getFont()->getLineSpacing(text.getCharacterSize());
}
inline unsigned int getLineCount(const sf::Text& text)
{
    std::string str = text.getString();

    if(str.empty())
    {
        return 0;
    }

    unsigned int lineCount = 1;

    for(auto& c : str)
    {
        if(c == '\n')
        {
            ++lineCount;
        }
    }

    return lineCount;
}
inline float getTextRelativeCenterY(const sf::Text& text){
    return text.getGlobalBounds().height/2 + getTextCenterOffsetY(text);
}

template<typename T> sf::Rect<T> operator+(const sf::Rect<T>& focus, const sf::Rect<T>& other){
    return sf::Rect<T>(focus.left,
                       focus.top,
                       focus.width + other.width,
                       focus.height + other.height);
}
template<typename T> void operator +=(sf::Rect<T>& focus, const sf::Rect<T>& other){
    focus.height += other.height;
    focus.width += other.width;
}
template<typename T1, typename T2> void operator/=(sf::Rect<T1>& focus, const T2 divisor){
    focus.height /= divisor;
    focus.width /= divisor;
}

template<typename T> sf::Rect<T> averageBounds(std::vector<sf::Rect<T>>& rectVector){
    sf::Rect<T> output;
    size_t L = 0;
    for(typename std::vector<sf::Rect<T>>::iterator it = rectVector.begin();
        it != rectVector.end(); ++it){
            output += *it;
            ++L;
        }
    if(L > 0) output /= L;
    return output;
}

template<typename T> sf::Vector2<T> getBoundCenter(const sf::Rect<T>& bounds)
{

    return sf::Vector2<T>(bounds.left + bounds.width/2,
                          bounds.top + bounds.height/2);

}

CVISION_API sf::Rect<float> averageTextSize(std::vector<sf::Text>& textItems);

template<typename T> inline sf::Vector2f element_range_y(const std::vector<T>& elements){
    sf::Vector2f output(NAN, NAN);
    sf::FloatRect bounds;
    for(auto& item : elements){
        bounds = item.getGlobalBounds();
        if(isnan(output.x) || (bounds.top < output.x)) output.x = bounds.top;
        if(isnan(output.y) || (bounds.top + bounds.height > output.y)) output.y = bounds.top + bounds.height;
    }
    return output;
}

template<typename T> inline sf::Vector2f element_range_x(const std::vector<T>& elements){
    sf::Vector2f output(NAN, NAN);
    sf::FloatRect bounds;
    for(auto& item : elements){
        bounds = item.getGlobalBounds();
        if(isnan(output.x) || (bounds.left < output.x)) output.x = bounds.left;
        if(isnan(output.y) || (bounds.left + bounds.width > output.y)) output.y = bounds.left + bounds.width;
    }
    return output;
}

template<typename T> inline sf::FloatRect get_element_bounds(const std::vector<T>& elements){
    sf::FloatRect bounds, output(NAN, NAN, 0.0f, 0.0f);
    for(auto& item : elements){
        bounds = item.getGlobalBounds();
        if(isnan(output.top) || (bounds.top < output.top)) output.top = bounds.top;
        if(isnan(output.height) || (bounds.top + bounds.height > output.top + output.height))
            output.height = bounds.top + bounds.height - output.top;

        if(isnan(output.left) || (bounds.left < output.left)) output.left = bounds.left;
        if(isnan(output.width) || (bounds.left + bounds.width > output.left + output.width))
            output.width = bounds.left + bounds.width - output.left;
    }
    return output;
}

template<typename T> inline sf::FloatRect get_element_bounds(const std::vector<T*>& elements){
    sf::FloatRect bounds, output(NAN, NAN, 0.0f, 0.0f);
    for(auto& item : elements){
        bounds = item->getGlobalBounds();
        if(isnan(output.top) || (bounds.top < output.top)) output.top = bounds.top;
        if(isnan(output.height) || (bounds.top + bounds.height > output.top + output.height))
            output.height = bounds.top + bounds.height - output.top;

        if(isnan(output.left) || (bounds.left < output.left)) output.left = bounds.left;
        if(isnan(output.width) || (bounds.left + bounds.width > output.left + output.width))
            output.width = bounds.left + bounds.width - output.left;
    }
    return output;
}

inline float totalTextHeight(const std::vector<sf::Text>& textItems){
    float output = 0.0f;
    for(auto text : textItems){
        output += text.getGlobalBounds().height;
    }
    return output;
}

inline float maxTextWidth(const std::vector<sf::Text>& textItems){
    float output = 0.0f;
    for(auto& text : textItems){
        if(text.getGlobalBounds().width > output) output = text.getGlobalBounds().width;
    }
    return output;
}

inline float maxTextHeight(const std::vector<sf::Text>& textItems){
    float output = 0.0f;
    for(auto& text : textItems){
        if(text.getGlobalBounds().height > output) output = text.getGlobalBounds().height;
    }
    return output;
}

CVISION_API void wrapText(sf::Text& textItem, const sf::FloatRect& bounds, const float& padding);

// Keyboard

inline bool ctrlPressed(){
    #ifdef __APPLE__
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
    #else
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
    #endif
}
inline bool shiftPressed(){
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
}
inline bool altPressed(){
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt));
}

// Clipboard

CVISION_API bool copyToClipboard(const std::string& cpyString);
CVISION_API std::string getClipboardText();

inline const unsigned int taggedIndex(std::string tag, std::vector<std::string>& tags){
    unsigned int index = 0;
    for(std::vector<std::string>::const_iterator it = tags.begin();
        it != tags.end(); ++it, ++index){
            if(*it == tag) return index;
        }
    return UINT_MAX;
}

template<typename T> T* taggedItem(std::string tag, std::vector<T>& items, std::vector<std::string>& tags){
    unsigned int L = 0;
    for(std::vector<std::string>::const_iterator strIT = tags.begin();
        strIT != tags.end(); ++strIT){
            if(*strIT == tag) return &items[L];
            else ++L;
        }
    return nullptr;
}

CVISION_API void sleep(float time);
CVISION_API float getElapsedTime(std::chrono::high_resolution_clock::time_point& t0,
                     std::chrono::high_resolution_clock::time_point t1);


// 2D space

template<typename T1, typename T2> void expandBounds(sf::Rect<T1>& bounds, const sf::Rect<T2>& otherBounds){
    if(otherBounds.left < bounds.left){
        bounds.width = bounds.left + bounds.width - otherBounds.left;
        bounds.left = otherBounds.left;
    }
    if(otherBounds.top < bounds.top){
        bounds.height = bounds.top + bounds.height - otherBounds.top;
        bounds.top = otherBounds.top;
    }
    if(otherBounds.left + otherBounds.width > bounds.left + bounds.width)
        bounds.width = otherBounds.left + otherBounds.width - bounds.left;
    if(otherBounds.top + otherBounds.height > bounds.top + bounds.height)
        bounds.height = otherBounds.top + otherBounds.height - bounds.top;
}

template<typename T1, typename T2> void expandBounds(sf::Rect<T1>& bounds, const T2& distance){
    bounds.left -= distance;
    bounds.width += 2*distance;
    bounds.top -= distance;
    bounds.height += 2*distance;
}

template<typename bound_t, typename shape_t> void applyBounds(const sf::Rect<bound_t>& bounds, shape_t& shape){
    shape.setPosition(sf::Vector2f(bounds.left, bounds.top));
    shape.setSize(sf::Vector2f(bounds.width, bounds.height));
}

CVISION_API void physicsSpread(std::vector<sf::Sprite>& sprites,
                               const float& strength,
                               const unsigned char& direction = CV_DIRECTION_X);
CVISION_API void physicsSpread(std::vector<sf::Shape*>& shapes,
                               const float& strength,
                               const unsigned char& direction = CV_DIRECTION_X);
CVISION_API void physicsSpread(std::vector<sf::Sprite*>& shapes,
                               const float& strength,
                               const unsigned char& direction = CV_DIRECTION_X);
CVISION_API void physicsSpread(std::vector<CVElement*>& elements,
                               const float& strength,
                               const unsigned char& direction = CV_DIRECTION_X | CV_DIRECTION_Y);

CVISION_API double get_angle(const sf::Vector2f& origin, const sf::Vector2f& destination); // Returns radians, in the cartesian plane (CW rotation)

inline sf::Vector2f radial_position(const sf::Vector2f& origin, const float& radius, const double& angle){ // Accepts radians
    return origin + sf::Vector2f(radius*cos(angle), radius*sin(angle));
}

inline void rotate_about_origin(sf::Vector2f& position, const sf::Vector2f& origin, const double& angle){
    position = radial_position(origin, getDistance(origin, position), get_angle(origin, position)*180/PI + angle);
}

template<typename T> sf::Vector2<T> components(const T& scalar, const float& angle){
    return sf::Vector2<T>(scalar*cos(angle), scalar*sin(angle));
}
template<typename T> T scalar(const sf::Vector2<T>& V){
    return sqrt(pow(V.x, 2) + pow(V.y, 2));
}

inline float getVelocity(const sf::Vector2f& lastFramePosition,
                         const sf::Vector2f& position,
                         const float& frameTime){
    return getDistance(lastFramePosition, position)/frameTime;
}

/* Get the arc of a circle that can be seen inside a box
    Output: sf::Vector2f(CWBeginRadians, CWEndRadians)*/
CVISION_API sf::Vector2f internal_arc(const sf::Vector2f& origin,
                          const sf::FloatRect& bounds,
                          const float& distance,
                          const float& resolution = PI/12); // Default of 24 samplings

template<typename T> void rotate(sf::Rect<T>& rect, const sf::Vector2f& origin, const float& angle){ // Gets new bounds of virtual rotation

}

// Graphics

CVISION_API void modulate(sf::Image& image, const sf::Color& newColor);
CVISION_API void expand_canvas(sf::Image& image,
                               const sf::Vector2i& distance,
                               const sf::Color& background = sf::Color::Transparent);
CVISION_API void gaussianBlur(sf::Image& image, const int& radius = 6);

}

#endif // CVIS_ALGORITHM
