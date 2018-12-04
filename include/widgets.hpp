//////////////////////////////////////////////////////////////
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

#ifndef CVIS_WIDGETS
#define CVIS_WIDGETS

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_buttonIO.hpp"

namespace cvis{

class CVTitleBar: public CVTextBox{

    uint8_t alignment;

    CVButton* closeButton,
            * resizeButton,
            * minimizeButton;

    bool bMoveWindow,
        bCanClose,
        bCanResize,
        bCanMinimize,
        bMouseGlow;

    sf::Sprite followGlow;

public:

    void align(const uint8_t& newAlignment);
    inline const uint8_t& getAlignment() const{ return alignment; }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw(sf::RenderTarget* target);

    void setMouseGlow(const bool& state = true,
                    const std::string& texture = "gradient_radial");

    CVTitleBar(CVView* View, const uint8_t& alignment, const float& height, const float& width,
               const textEntry& textInfo, const std::string& logo = "", const sf::Color& fillColor = sf::Color(128,128,128),
               const sf::Color& outlineColor = sf::Color(60,60,60), const float& outlineWidth = 1.0f);

    ~CVTitleBar();
};

class CVScrollBar: public CVBox{
protected:
    sf::Vector2f anchorBegin,
                anchorEnd;
    float scrollPos,
            scrollMax,
            panelSize,
            scrollAngle,
            scrollSpeed,
            minLength,
            sensitivity;
    uint8_t animType,
            responseDir;
    bool bCanScroll,
        bReleaseOnEnd;
    sf::FloatRect mouseHighlightBounds;

    void calcHighlightBounds();

public:

    bool scroll(const float& delta);
    void setScrollOffset(const float& newOffset);
    void setAnchorPoints(const sf::Vector2f& anchorBegin, const sf::Vector2f& anchorEnd);
    inline void setScrollMax(const float& newMax){
        scrollMax = newMax;
    }
    inline void setPanelSize(const float& newSize){
        panelSize = newSize;
    }
    inline void setSensitivity(const float& value){ sensitivity = value; }

    inline const float& getScrollOffset() const{ return scrollPos; }

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void setScrollable(const bool& status){ bCanScroll = status; }
    void setDirection(const uint8_t& direction){ responseDir = direction; }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVScrollBar(CVView* parentView, const sf::Vector2f& anchorBegin,
                const sf::Vector2f& anchorEnd, const float& panelLength,
                const float& width, const sf::Color& fillColor,
                const sf::Color& borderColor, const float& borderWidth,
                const float& scrollSpeed = 1000.0f,
                const uint8_t& animType = CV_OBJ_ANIM_FADE_IN);

};

class CVProgressBar: public CVTextBox{
protected:

    float progress,
            animSpeed;

    inline rounded_rectangle& progressBar(){ return panel[1]; }
    inline sf::Text& progressText(){ return displayText.front(); }

    std::reference_wrapper<sf::Color> progressColor;

    void alignText();

public:

    inline const sf::Color& getProgressColor() const{ return progressColor.get(); }
    void setProgressColor(const sf::Color& newColor);

    void setProgress(const float& newProgress);
    inline const float& getProgress() const{ return progress; }

    inline void setAnimSpeed(const float& newSpeed){ animSpeed = newSpeed; }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVProgressBar(CVView* View, sf::Vector2f position, float width, float height,
                textEntry textInfo = textEntry(), sf::Color fillColor = sf::Color(128,128,128),
                  sf::Color progressColor = sf::Color(60,60,60),
                  sf::Color borderColor = sf::Color::Black,
                float borderWidth = 1.0f);
};

}

#endif // CVIS_WIDGETS
