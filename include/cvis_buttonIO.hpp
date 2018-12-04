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

#pragma once

#ifndef CVIS_BUTTONIO
#define CVIS_BUTTONIO

#include "cvision/cvis_base.hpp"

namespace cvis{

class CVButton:public CVTextBox{
protected:

    bool bFadeInHover,
        bFadeInToggle,
        bTriggered,
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

    void setState(const unsigned int& state);
    void toggle();
    inline void setResponseEvent(const unsigned char& eventFlags){
        responseEvent = eventFlags;
    }

    void setColor(const sf::Color& newColor, const unsigned int& state = 0);
    void setFadeIn(const bool& state, const uint8_t& targetAlpha = 255, const uint8_t& rate = 0);
    void setFadeOnClick(const bool& state, const uint8_t& targetAlpha = 255, const uint8_t& rate = 0);

    inline bool getTrigger(){
        if(bTriggered){
            bTriggered = false;
            return true;
        }
        return false;
    }

    template<typename FUNC> void setActivationFunction(FUNC func){ activateFunc = func; } // Allow for custom activation functions by CVView

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    CVButton(CVView* View, const sf::Vector2f& position, float width, float height,
                const textEntry& textInfo, const std::string& icon = "",
                sf::Color fillColor = sf::Color(168, 168, 168),
                sf::Color borderColor = sf::Color(102,102,102),
                float borderWidth = 2.0f); // Standard box button
    CVButton(CVView* View, const sf::Vector2f& position, const float& width,
             const float& height, const std::string& icon, const unsigned int& numStates = 2,
             const unsigned int& initialState = 0,
             const bool& bHighlight = true,
             void (*activateFunc)(const unsigned int&, CVView*) = nullptr); // Sprite button

};

}

#endif // CVIS_BUTTONIO
