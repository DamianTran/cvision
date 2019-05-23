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

#ifndef CVIS_SLIDER
#define CVIS_SLIDER

#include "cvision/panel.hpp"
#include "cvision/primitives.hpp"

namespace cvis
{

class CVISION_API CVSliderBar : public CVBasicViewPanel
{
public:

    CVISION_API CVSliderBar(cvis::CVView * View,
                            const std::string& tag,
                            const sf::Vector2f& position,
                            const sf::Vector2f& size,
                            const float& min,
                            const float& max,
                            const float& initialValue = NAN,
                            const size_t& increments = 10,
                            const sf::Color& barColor = sf::Color(128, 128, 128),
                            const sf::Color& sliderColor = sf::Color(160, 160, 160),
                            const sf::Color& backgroundColor = sf::Color::Transparent,
                            const sf::Color& outlineColor = sf::Color::Transparent,
                            const float& outlineThickness = 0.0f,
                            CVElement* sliderElement = nullptr);

    inline const float& getMinValue()           const noexcept{ return fMinValue; }
    inline const float& getMaxValue()           const noexcept{ return fMaxValue; }
    inline const size_t& getIncrementCount()    const noexcept{ return stIncrementCount; }
    inline const float& getCurrentValue()       const noexcept{ return fCurrentValue; }

    CVISION_API void setMinValue(const float& newValue) noexcept;
    CVISION_API void setMaxValue(const float& newValue) noexcept;
    CVISION_API void setIncrementCount(const size_t& newValue) noexcept;
    CVISION_API void setCurrentValue(const float& newValue) noexcept;

    CVISION_API void move(const sf::Vector2f& distance) override;
    CVISION_API void setPosition(const sf::Vector2f& newPosition) override;
    CVISION_API void setSize(const sf::Vector2f& newSize) override;

    CVISION_API void setTickMarks(const bool& state = true) noexcept;
    CVISION_API void setTickColor(const sf::Color& newColor) noexcept;
    CVISION_API void setTextColor(const sf::Color& newColor) noexcept;

    CVISION_API void setTitle(const std::string& text,
                              const unsigned int& alignment = ALIGN_LEFT) noexcept;
    CVISION_API void clearTitle() noexcept;
    CVISION_API std::string getTitle() const noexcept;

    CVISION_API void usePositionLabels(const bool& state = true) noexcept;
    CVISION_API void setPositionLabelDensity(const size_t& newDensity) noexcept;

    CVISION_API void setSliderMask(const std::string& mask_name,
                                   const sf::Color& color) noexcept;

    CVISION_API void addTitle(const std::string& text,
                              const unsigned int& alignment);

    CVISION_API void setSliderRounding(const float& newRounding,
                                       const size_t& point_count = 12,
                                       const std::vector<bool>& corner_states = std::vector<bool>(4, true));
    CVISION_API void setSliderSize(const sf::Vector2f& newSize);

    CVISION_API void setSliderElement(CVElement * newElement);

    inline bool setValueRounding(const bool& state) noexcept{ bRound = state; }
    inline const bool& isRounded() const noexcept{ return bRound; }

    CVISION_API float positionToValue(const sf::Vector2f& position) const noexcept;
    CVISION_API sf::Vector2f valueToPosition(const float& value) const noexcept;

    CV_UPDATE_OVERRIDE;
    CV_DRAW_OVERRIDE;

protected:

    float               fMinValue;
    float               fMaxValue;
    float               fCurrentValue;
    float               fSliderAnimRate;
    float               fTickPadding;
    float               fTickLength;
    float               fTickThickness;

    bool                bTickMarks;
    bool                bPositionLabels;
    bool                bSliderCaptured;
    bool                bTickBilateral;
    bool                bSliderCount;
    bool                bRound;

    size_t              stPositionLabelDensity;
    size_t              stIncrementCount;

    rounded_rectangle   sliderBar;

    CVElement *         sliderElement;

    CVText *            sliderCountText;
    CVText *            titleText;

    std::vector<CVText*>            positionLabels;
    std::vector<rounded_rectangle>  ticks;
    std::vector<float>              tickPositions;

    sf::Color tickColor;

    void updateSlider();
    void updateMarks();

};

}

#endif // CVIS_SLIDER
