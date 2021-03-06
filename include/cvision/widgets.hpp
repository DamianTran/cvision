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

#ifndef CVIS_WIDGETS
#define CVIS_WIDGETS

#ifndef CVIS_ANIM_MACROS
#define CVIS_ANIM_MACROS

#define CV_OBJ_ANIM_NONE                0
#define CV_OBJ_ANIM_SLIDE               1
#define CV_OBJ_ANIM_ACCEL               2
#define CV_OBJ_ANIM_DECEL               3

#define CV_OBJ_ANIM_FADE_IN             4
#define CV_OBJ_ANIM_FADE_OUT            5
#define CV_OBJ_ANIM_ROTATE              6
#endif

#include "cvision/lib.hpp"
#include "cvision/textbox.hpp"

namespace cvis{

class CVButton;

/** @brief Title-bar widget.
  *
  * Simple top-bar with configurable close, minimize, and restore buttons.
  * Optionally, include a follow glow effect on mouse proximity.
  */

class CVTitleBar: public CVTextBox{
public:

    CVISION_API CVTitleBar(CVView* View, const uint8_t& alignment, const float& height, const float& width,
                           const TextEntry& textInfo, const std::string& logo = "", const sf::Color& fillColor = sf::Color(128,128,128),
                           const sf::Color& outlineColor = sf::Color(60,60,60), const float& outlineWidth = 1.0f);

    CVISION_API ~CVTitleBar();

    CVISION_API void align(const uint8_t& newAlignment);
    inline const uint8_t& getAlignment()                    const{ return alignment; }

    inline void setGlowDistance(const float& newDistance)        { fGlowDistance = newDistance; }

    CVISION_API void setButtonColor(const sf::Color& color);
    CVISION_API void setIconColor(const sf::Color& color);
    CVISION_API void setGlowColor(const sf::Color& color);

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setMouseGlow(const bool& state = true,
                    const std::string& texture = "gradient_radial");

protected:

    uint8_t     alignment;

    CVButton *  closeButton;
    CVButton *  resizeButton;
    CVButton *  minimizeButton;

    bool        bMoveWindow;
    bool        bCanClose;
    bool        bCanResize;
    bool        bCanMinimize;
    bool        bMouseGlow;

    sf::Sprite  followGlow;

    float       fGlowDistance;

};

/** @brief An attachable scrollbar widget.
  *
  * Keeps track of total scroll progress for a defined panel
  * length.  Control the scrollbar from another panel's update
  * loop in order to track total scroll distance and visualize
  * the progress. */

class CVISION_API CVScrollBar : public CVBox{
public:

    CVISION_API CVScrollBar(CVView* parentView,
                            const sf::Vector2f& anchorBegin,
                            const sf::Vector2f& anchorEnd,
                            const float& panelLength,
                            const float& width,
                            const sf::Color& fillColor,
                            const sf::Color& borderColor,
                            const float& borderWidth,
                            const float& scrollSpeed = 1000.0f,
                            const uint8_t& animType = CV_OBJ_ANIM_FADE_IN);

    CVISION_API ~CVScrollBar();

    CVISION_API bool scroll(const float& delta);
    CVISION_API void setScrollOffset(const float& newOffset);
    CVISION_API void setAnchorPoints(const sf::Vector2f& anchorBegin, const sf::Vector2f& anchorEnd);
    inline void setScrollMax(const float& newMax){
        scrollMax = newMax;
    }
    inline void setPanelSize(const float& newSize){
        panelSize = newSize;
    }

    inline const float& getScrollMax() const{
        return scrollMax;
    }
    inline const float& getPanelSize() const{
        return panelSize;
    }

    inline void setSensitivity(const float& value){ sensitivity = value; }

    inline const float& getScrollOffset() const{ return scrollPos; }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    inline void setScrollable(const bool& status){ bCanScroll = status; }
    inline void setDirection(const uint8_t& direction){ responseDir = direction; }

    inline void reverseY() noexcept{ bReverseY = !bReverseY; }
    inline void setYReverse(const bool& status = true) noexcept{ bReverseY = status; }
    inline void reverseX() noexcept{ bReverseX = !bReverseX; }
    inline void setXReverse(const bool& status = true) noexcept{ bReverseX = status; }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

protected:

    sf::Vector2f    anchorBegin;
    sf::Vector2f    anchorEnd;

    float           scrollPos;
    float           scrollMax;
    float           panelSize;
    float           scrollAngle;
    float           scrollSpeed;
    float           minLength;
    float           sensitivity;

    uint8_t         animType;
    uint8_t         responseDir;

    bool            bCanScroll;
    bool            bReleaseOnEnd;
    bool            bReverseY;
    bool            bReverseX;

    sf::FloatRect   mouseHighlightBounds;

    CVISION_API void calcHighlightBounds();

};

/** @brief A progress bar widget.
  *
  * Use this widget to keep track of progress in a loading
  * bar format.  Useful for loading screens, health bars,
  * and other linear dichromatic-type indicators.
  */

class CVISION_API CVProgressBar: public CVTextBox{
public:

    inline const sf::Color& getProgressColor() const{ return progressColor.get(); }
    CVISION_API void setProgressColor(const sf::Color& newColor);

    CVISION_API void setProgress(const float& newProgress);
    inline const float& getProgress() const{ return progress; }

    inline void setAnimSpeed(const float& newSpeed){ animSpeed = newSpeed; }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API CVProgressBar(CVView* View,
                              const sf::Vector2f& position,
                              const float& width,
                              const float& height,
                              const TextEntry& textInfo = TextEntry(),
                              const sf::Color& fillColor = sf::Color(128,128,128),
                              const sf::Color& progressColor = sf::Color(60,60,60),
                              const sf::Color& borderColor = sf::Color::Black,
                              const float& borderWidth = 1.0f);

protected:

    float progress;
    float animSpeed;

    inline rounded_rectangle& progressBar(){ return panel[1]; }
    inline sf::Text& progressText(){ return displayText.front(); }

    std::reference_wrapper<sf::Color> progressColor;

    CVISION_API void alignText();
};

}

#endif // CVIS_WIDGETS
