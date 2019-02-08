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

#ifndef CVIS_BUTTONIO
#define CVIS_BUTTONIO

#include "cvision/lib.hpp"
#include "cvision/textbox.hpp"

namespace cvis{

class CVEvent;

class CVISION_API CVButton:public CVTextBox{
protected:

    bool bFadeInHover,
        bFadeInToggle,
        bToggleFlipX,
        bToggleFlipY;

    float rotateAngle;          //   Angle to rotate through on trigger
    unsigned char rotateType,   //  Items to rotate on trigger
                    responseEvent;  // Event to trigger


    uint8_t baseAlpha;          // Alpha to revert to during trigger switch if applicable

    void (*activateFunc)(const unsigned int&, CVView*);

    std::vector<const sf::Texture*> stateTextures; // Cycle of textures for button sprite to trigger through
    std::vector<sf::Color> stateColors;             // Colors to cycle through

public:

    inline void setRotateAngle(const float& newAngle){ rotateAngle = newAngle; }
    inline void disableRotation(){ rotateAngle = NAN; }
    inline void setRotateType(const unsigned char& newType){ rotateType = newType; }
    inline void setToggleFlip(const bool& x, const bool& y){
        bToggleFlipX = x;
        bToggleFlipY = y;
    }

    CVISION_API void setState(const unsigned int& state);
    CVISION_API void toggle();
    inline void setResponseEvent(const unsigned char& eventFlags){
        responseEvent = eventFlags;
    }

    CVISION_API void setColor(const sf::Color& newColor, const unsigned int& state = 0);
    CVISION_API void setFadeIn(const bool& state, const uint8_t& targetAlpha = 255, const uint8_t& rate = 0);
    CVISION_API void setFadeOnClick(const bool& state, const uint8_t& targetAlpha = 255, const uint8_t& rate = 0);

    template<typename FUNC> void setActivationFunction(FUNC func){ activateFunc = func; } // Allow for custom activation functions by CVView

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API CVButton(CVView* View, const sf::Vector2f& position, float width, float height,
                const textEntry& textInfo, const std::string& icon = "",
                sf::Color fillColor = sf::Color(168, 168, 168),
                sf::Color borderColor = sf::Color(102,102,102),
                float borderWidth = 2.0f); // Standard box button
    CVISION_API CVButton(CVView* View, const sf::Vector2f& position, const float& width,
             const float& height, const std::string& icon, const unsigned int& numStates = 2,
             const unsigned int& initialState = 0,
             const bool& bHighlight = true,
             void (*activateFunc)(const unsigned int&, CVView*) = nullptr); // Sprite button

};

}

#endif // CVIS_BUTTONIO
