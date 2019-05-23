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

#include "cvision/widgets/slider.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

#include <hyper/algorithm.hpp>

using namespace std;
using namespace hyperC;

namespace cvis
{

CVSliderBar::CVSliderBar(CVView * View,
                         const string& tag,
                         const sf::Vector2f& position,
                         const sf::Vector2f& size,
                         const float& min,
                         const float& max,
                         const float& initialValue,
                         const size_t& incrementCount,
                         const sf::Color& barColor,
                         const sf::Color& sliderColor,
                         const sf::Color& backgroundColor,
                         const sf::Color& outlineColor,
                         const float& outlineThickness,
                         CVElement * sliderElement):
                             CVBasicViewPanel(View, tag,
                                              backgroundColor,
                                              size, false,
                                              position),
                             fMinValue(min),
                             fMaxValue(max),
                             fCurrentValue(initialValue),
                             fSliderAnimRate(0.4f),
                             fTickPadding(4.0f),
                             fTickLength(6.0f),
                             fTickThickness(1.0f),
                             bTickMarks(true),
                             bPositionLabels(true),
                             bSliderCaptured(false),
                             bTickBilateral(false),
                             bSliderCount(true),
                             bRound(false),
                             stPositionLabelDensity(2),
                             stIncrementCount(incrementCount),
                             sliderBar(sf::Vector2f(size.x, size.y/5)),
                             sliderElement(sliderElement),
                             sliderCountText(nullptr),
                             titleText(nullptr),
                             tickColor(sf::Color::Black)
{

    if(!sliderElement)
    {
        this->sliderElement = new CVBox(View,
                                      sf::Vector2f(position.x + size.x*9/20,
                                                   position.y),
                                      6.0f * viewScale(),
                                      size.y,
                                      sliderColor);
        this->sliderElement->setHighlightableStatus(false);
        this->sliderElement->setHighlightColor(sliderColor + sf::Color(20, 20, 20));
        this->sliderElement->setOrigin(this->sliderElement->getSize()/2);
        ((CVBox*)this->sliderElement)->setRounding(3.0f * viewScale());
    }

    if(isnan(initialValue))
    {
        fCurrentValue = (fMaxValue - fMinValue)/2;
    }

    addPanelElement(this->sliderElement);

    sliderBar.setFillColor(barColor);
    sliderBar.setOrigin(sf::Vector2f(0.0f,
                                     sliderBar.getSize().y/2));
    sliderBar.setPosition(sf::Vector2f(getPosition().x,
                                        getPosition().y + getSize().y/2 - sliderBar.getSize().y/2));
    sliderBar.setRounding(sliderBar.getSize().y/2);

    this->sliderElement->setPosition(valueToPosition(fCurrentValue));

    updateMarks();
    updateSlider();
}

void CVSliderBar::setMinValue(const float& newValue) noexcept
{
    if(bRound)
    {
        fMinValue = round(newValue);
    }
    else
    {
        fMinValue = newValue;
    }

    updateMarks();
    updateSlider();
}

void CVSliderBar::setMaxValue(const float& newValue) noexcept
{
    if(bRound)
    {
        fMaxValue = round(newValue);
    }
    else
    {
        fMaxValue = newValue;
    }
    updateMarks();
    updateSlider();
}

void CVSliderBar::setIncrementCount(const size_t& newValue) noexcept
{
    stIncrementCount = newValue;
    updateMarks();
    updateSlider();
}

void CVSliderBar::setCurrentValue(const float& newValue) noexcept
{
    if(bRound)
    {
        fCurrentValue = round(newValue);
    }
    else
    {
        fCurrentValue = newValue;
    }
    updateMarks();
    updateSlider();
}

void CVSliderBar::move(const sf::Vector2f& distance)
{

    CVBasicViewPanel::move(distance);

    for(auto& tick : ticks)
    {
        tick.move(distance);
    }

    sliderBar.move(distance);

}

void CVSliderBar::setPosition(const sf::Vector2f& newPosition)
{

    move(newPosition - getPosition());

}

void CVSliderBar::setSize(const sf::Vector2f& newSize)
{

    CVBasicViewPanel::setSize(newSize);

    updateSlider();
    updateMarks();

}

void CVSliderBar::setTickMarks(const bool& state) noexcept
{

    if(bTickMarks != state)
    {

        bTickMarks = state;
        updateMarks();

    }

}

void CVSliderBar::setTickColor(const sf::Color& newColor) noexcept
{

    tickColor = newColor;
    updateMarks();

}

void CVSliderBar::setTextColor(const sf::Color& newColor) noexcept
{
    CVBasicViewPanel::setTextColor(newColor);

    if(sliderCountText)
    {
        sliderCountText->setTextColor(newColor);
    }

    for(auto& text : positionLabels)
    {
        text->setTextColor(newColor);
    }
}

void CVSliderBar::setTitle(const std::string& text,
                           const unsigned int& alignment) noexcept
{

    if(!titleText)
    {
        titleText = new CVText(View,
                               TextEntry("", textInfo.font, 14, alignment, textInfo.textColor),
                               sf::Vector2f(bounds.left,
                                            bounds.top - 12.0f),
                               sf::Vector2f(bounds.width, 18.0f));
        titleText->setTextPadding(0.0f);
        addPanelElement(titleText);
    }

    titleText->setText(0, text);

    float posX;
    float posY;

    switch(alignment)
    {
    case ALIGN_LEFT_BOTTOM:
        {
            posX = bounds.left - 8.0f;
            posY = bounds.top + bounds.height;
            break;
        }
    case ALIGN_CENTER:
        {
            posX = bounds.left + bounds.width/2 - titleText->getSize().x/2;
            if(bSliderCount && (sliderCountText->getPosition().y < sliderBar.getPosition().y))
            {
                posY = sliderCountText->getPosition().y - titleText->getSize().y - 24.0f;
            }
            else
            {
                posY = sliderBar.getPosition().y - sliderElement->getSize().y/2 - titleText->getBounds().height - 6.0f;
            }
            break;
        }
    case ALIGN_CENTER_BOTTOM:
        {
            posX = bounds.left + bounds.width/2 - titleText->getSize().x/2;
            posY = bounds.top + bounds.height;
            break;
        }
    default:
        {
            posX = bounds.left - 8.0f;
            if(bSliderCount && (sliderCountText->getPosition().y < sliderBar.getPosition().y))
            {
                posY = sliderCountText->getPosition().y - titleText->getSize().y - 24.0f;
            }
            else
            {
                posY = sliderBar.getPosition().y - sliderElement->getSize().y/2 - titleText->getBounds().height - 6.0f;
            }
            break;
        }

    }

    titleText->setPosition(posX, posY);

    fitElements(false, true);

}

void CVSliderBar::clearTitle() noexcept
{
    if(titleText)
    {
        removePanelElement(titleText);
        titleText = nullptr;
    }
}

string CVSliderBar::getTitle() const noexcept
{
    if(titleText)
    {
        return titleText->getText();
    }

    return string();
}

void CVSliderBar::usePositionLabels(const bool& state) noexcept
{

    if(bPositionLabels != state)
    {
        bPositionLabels = state;
        updateMarks();
    }

}

void CVSliderBar::setPositionLabelDensity(const size_t& newDensity) noexcept
{

    stPositionLabelDensity = newDensity;
    updateMarks();

}

void CVSliderBar::setSliderMask(const std::string& texture,
                                const sf::Color& maskColor) noexcept
{
    if(dynamic_cast<CVShape*>(sliderElement))
    {
        ((CVShape*)sliderElement)->setMask(appTexture(texture), maskColor);
    }
}

void CVSliderBar::setSliderRounding(const float& newRounding,
                                    const size_t& point_count,
                                    const vector<bool>& corner_states)
{
    if(dynamic_cast<CVBox*>(sliderElement))
    {
        ((CVBox*)sliderElement)->setRounding(newRounding, point_count, corner_states);
    }
}

void CVSliderBar::setSliderSize(const sf::Vector2f& newSize)
{
    sliderElement->setSize(newSize);
    sliderElement->setOrigin(newSize/2);
    updateSlider();
}

void CVSliderBar::setSliderElement(CVElement* newElement)
{

    removePanelElement(sliderElement);
    sliderElement = newElement;

}

float CVSliderBar::positionToValue(const sf::Vector2f& position) const noexcept
{
    double angle = get_angle(sliderBar.getPosition(), position) - (double)sliderBar.getRotation()/180*PI;
    float scalar_distance = getDistance(position, sliderBar.getPosition());

    sf::Vector2f vector_distance = components(scalar_distance, angle);

    return vector_distance.x / sliderBar.getSize().x * (fMaxValue - fMinValue) + fMinValue;
}

sf::Vector2f CVSliderBar::valueToPosition(const float& value) const noexcept
{

    float dist = sliderBar.getSize().x;
    double angle = sliderBar.getRotation()/180.0f * PI;

    sf::Vector2f vDist = components(dist * (value - fMinValue)/(fMaxValue - fMinValue), angle);

    return sf::Vector2f(sliderBar.getPosition().x + vDist.x,
                        sliderBar.getPosition().y + vDist.y);

}

bool CVSliderBar::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    updateSlider();

    if(sliderElement->getBounds().contains(mousePos))
    {
        event.setCursor(sf::Cursor::Hand);
    }

    if(event.LMBhold)
    {
        if(!bSliderCaptured &&
           sliderElement->getBounds().contains(event.LMBpressPosition))
        {
            bSliderCaptured = true;
            event.setCursor(sf::Cursor::SizeHorizontal);
        }
        else if(bSliderCaptured)
        {
            setCurrentValue(closest(positionToValue(mousePos), tickPositions));
            event.setCursor(sf::Cursor::SizeHorizontal);
        }
    }
    else if(bSliderCaptured && (event.LMBreleaseFrames == 1))
    {
        bSliderCaptured = false;
        sendTriggers();
    }

    return true;
}

bool CVSliderBar::draw(sf::RenderTarget * target)
{

    if(!CVTextBox::draw(target))
    {
        return false;
    }

    target->draw(sliderBar);

    if(bTickMarks)
    {
        for(auto& tick : ticks)
        {
            target->draw(tick);
        }
    }

    for(auto& element : viewPanelElements)
    {
        if(!bPositionLabels && anyEqual(element, positionLabels))
        {
            continue;
        }

        element->draw(target);
    }

    return true;

}

void CVSliderBar::updateSlider()
{

    sf::Vector2f destination = valueToPosition(fCurrentValue);

    float dist = getDistance(sliderElement->getPosition(), destination) * fSliderAnimRate;
    double angle = get_angle(sliderElement->getPosition(), destination);

    sliderElement->move(components(dist * fSliderAnimRate, angle));

    if(bSliderCount)
    {

        if(!sliderCountText)
        {

            sliderCountText = new CVText(View,
                                         TextEntry("", textInfo.font, 12, ALIGN_CENTER, textInfo.textColor),
                                         sliderElement->getPosition(),
                                         sf::Vector2f(sliderElement->getSize().x + 36.0f,
                                                      sliderElement->getSize().y + 36.0f));
            sliderCountText->setOrigin(sliderCountText->getSize()/2);
            sliderCountText->setTextPadding(0.0f);
            addPanelElement(sliderCountText);

        }

        stringstream ss;
        ss << fCurrentValue;
        sliderCountText->setText(0, ss.str());
        sliderCountText->setPosition(sliderElement->getPosition());

    }

}

void CVSliderBar::updateMarks()
{

    tickPositions = vseq(fMinValue, fMaxValue, stIncrementCount);

    if(bTickBilateral)
    {
        ticks.resize(stIncrementCount * 2);

        for(size_t i = 0; i < stIncrementCount; ++i)
        {
            ticks[i].setSize(sf::Vector2f(fTickThickness, fTickLength));
            ticks[i].setFillColor(tickColor);
            ticks[i].setOrigin(sf::Vector2f(fTickThickness/2, 0.0f));
            ticks[i].setRotation(sliderBar.getRotation());
            ticks[i].setPosition(valueToPosition(tickPositions[i]) +
                            components(fTickLength, (sliderBar.getRotation() + 90.0f)/180 * PI));
        }

        for(size_t i = stIncrementCount; i < stIncrementCount*2; ++i)
        {
            ticks[i].setSize(sf::Vector2f(fTickThickness, fTickLength));
            ticks[i].setFillColor(tickColor);
            ticks[i].setOrigin(sf::Vector2f(fTickThickness/2, 0.0f));
            ticks[i].setRotation(sliderBar.getRotation() + 180.0f);
            ticks[i].setPosition(valueToPosition(tickPositions[i]) +
                            components(fTickLength, (sliderBar.getRotation() - 90.0f)/180 * PI));
        }
    }
    else
    {
        ticks.resize(stIncrementCount);
        for(size_t i = 0; i < stIncrementCount; ++i)
        {
            ticks[i].setSize(sf::Vector2f(fTickThickness, fTickLength));
            ticks[i].setFillColor(tickColor);
            ticks[i].setOrigin(sf::Vector2f(fTickThickness/2, 0.0f));
            ticks[i].setRotation(sliderBar.getRotation());
            ticks[i].setPosition(valueToPosition(tickPositions[i]) +
                            components(fTickPadding, (sliderBar.getRotation() + 90.0f)/180 * PI));
        }

    }

    if(bPositionLabels)
    {

        while(positionLabels.size() < stPositionLabelDensity)
        {

            CVText * newPositionLabel = new CVText(View,
                                                   TextEntry("", textInfo.font, 12, ALIGN_CENTER_MIDLINE, textInfo.textColor),
                                                   sliderBar.getPosition(),
                                                   sf::Vector2f(24.0f, 24.0f));
            positionLabels.emplace_back(newPositionLabel);
            addPanelElement(newPositionLabel);

        }

        while(positionLabels.size() > stPositionLabelDensity)
        {

            removePanelElement(positionLabels.back());
            positionLabels.erase(positionLabels.end() - 1);

        }

        std::vector<size_t> position_idx = vseq(size_t(0), tickPositions.size() - 1, stPositionLabelDensity);

        for(size_t i = 0; i < stPositionLabelDensity; ++i)
        {
            stringstream ss;
            ss << tickPositions[position_idx[i]];
            positionLabels[i]->setText(0, ss.str());
            positionLabels[i]->setPosition(valueToPosition(tickPositions[position_idx[i]]) +
                                           components(fTickLength + fTickPadding, (sliderBar.getRotation() + 90.0f)/180 * PI) -
                                           sf::Vector2f(positionLabels[i]->getSize().x/2, 0.0f));

        }

    }

}

}
