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

#include "cvision/widgets.hpp"
#include "cvision/view.hpp"
#include "cvision/button.hpp"

namespace cvis
{

CVTitleBar::CVTitleBar(CVView* View, const uint8_t& alignment, const float& width, const float& height,
                       const TextEntry& textInfo, const std::string& logo, const sf::Color& fillColor,
                       const sf::Color& outlineColor, const float& outlineWidth):
    CVTextBox(View, sf::Vector2f(0.0f,0.0f), width, height, fillColor, outlineColor, outlineWidth),
    alignment(alignment),
    closeButton(nullptr),
    resizeButton(nullptr),
    minimizeButton(nullptr),
    bMoveWindow(true),
    bCanClose(true),
    bCanResize(true),
    bCanMinimize(true),
    bMouseGlow(false),
    fGlowDistance(View->getHeight()/8)
{

#ifdef __APPLE__
    const float logoPct = 0.6f,
                buttonPct = 0.5f,
                button_padding = height*3/4,
                button_spacing = height*3/4;
#else
    const float logoPct = 0.6f,
                buttonPct = 0.4f,
                button_padding = height,
                button_spacing = 3*height/2;
#endif

#ifdef __APPLE__
    closeButton = new CVButton(View, sf::Vector2f(button_padding, height/2),
                               height*buttonPct, height*buttonPct, "orb_button", 1, 0, true, nullptr);
    closeButton->setSpriteColor(sf::Color(220,50,50));
    resizeButton = new CVButton(View, sf::Vector2f(button_padding + button_spacing*2, height/2),
                                height*buttonPct, height*buttonPct, "orb_button", 2, 0, true, nullptr);
    resizeButton->setSpriteColor(sf::Color(50,220,20));
    minimizeButton = new CVButton(View, sf::Vector2f(button_padding + button_spacing, height/2),
                                  height*buttonPct, height*buttonPct, "orb_button", 2, 0, true, nullptr);
    minimizeButton->setSpriteColor(sf::Color(220,220,100));
    setMask(appTexture("gradient_linear"));
#else
    closeButton = new CVButton(View, sf::Vector2f(width - height * 1.8f, 0.0f),
                               height*1.8, height,
                               TextEntry("", ""), "gen_x", fillColor,
                               outlineColor, outlineWidth);
    closeButton->setSpriteColor(sf::Color(220,220,220,220));
    closeButton->setSpriteScale(0.4f);

    resizeButton = new CVButton(View, sf::Vector2f(closeButton->getPosition().x - height * 1.8f, 0.0f),
                                height*1.8, height,
                                TextEntry("", ""), "resize_button", fillColor,
                                outlineColor, outlineWidth);
    resizeButton->setSpriteColor(sf::Color(220,220,220,220));
    resizeButton->setSpriteScale(0.4f);

    minimizeButton = new CVButton(View, sf::Vector2f(resizeButton->getPosition().x - height * 1.8f, 0.0f),
                                  height*1.8, height,
                                  TextEntry("", ""), "bottom_line", fillColor,
                                  outlineColor, outlineWidth);
    minimizeButton->setSpriteColor(sf::Color(220,220,220,220));
    minimizeButton->setSpriteScale(0.4f);
#endif

#ifdef __APPLE__
    addTextEntry(textInfo, sf::Vector2f(width/2, height*3/8));
    displayText.back().move(-displayText.back().getGlobalBounds().width/2, 0.0f);
#else
    addTextEntry(textInfo, sf::Vector2f(height*3/2, height/2));
#endif

    this->textInfo = textInfo;

    #ifdef __APPLE__
    this->textInfo.alignment = ALIGN_CENTER_MIDLINE;
    #else
    this->textInfo.alignment = ALIGN_LEFT_MIDLINE;
    #endif

    align(alignment);
    alignText();

    #ifndef __APPLE__
    displayText.front().move(sf::Vector2f(height*1.42, 0));
    #endif

    displayText.back().setFillColor(textInfo.textColor);

    if(!logo.empty())
    {
#ifdef __APPLE__
        addSprite(appTexture(logo),
                  sf::Vector2f(displayText.back().getGlobalBounds().left - height*3/4,
                               height/2),
                  sf::Vector2f(height*logoPct, height*logoPct));
#else
        addSprite(appTexture(logo),
                  sf::Vector2f(height*3/4, height/2),
                  sf::Vector2f(height*logoPct, height*logoPct));
#endif
    }
}

void CVTitleBar::align(const uint8_t& alignment)
{
    if(alignment == this->alignment) return;

    this->alignment = alignment;


    switch(alignment)
    {
    case ALIGN_LEFT_MIDLINE:
    {
        setPosition(sf::Vector2f(0.0f, View->getHeight()/2 - bounds.height/2));
        break;
    }
    case ALIGN_LEFT_BOTTOM:
    {
        setPosition(sf::Vector2f(0.0f, View->getHeight() - bounds.height));
        break;
    }
    case ALIGN_CENTER:
    {
        setPosition(sf::Vector2f(View->getWidth()/2 - bounds.width/2, 0.0f));
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        setPosition(sf::Vector2f(View->getWidth()/2 - bounds.width/2,
                                 View->getHeight()/2 - bounds.height/2));
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        setPosition(sf::Vector2f(View->getWidth()/2 - bounds.width/2,
                                 View->getHeight() - bounds.height));
        break;
    }
    case ALIGN_RIGHT:
    {
        setPosition(sf::Vector2f(View->getWidth() - bounds.width, 0.0f));
        break;
    }
    case ALIGN_RIGHT_MIDLINE:
    {
        setPosition(sf::Vector2f(View->getWidth() - bounds.width,
                                 View->getHeight()/2 - bounds.height/2));
        break;
    }
    case ALIGN_RIGHT_BOTTOM:
    {
        setPosition(sf::Vector2f(View->getWidth() - bounds.width,
                                 View->getHeight() - bounds.height));
        break;
    }
    default:  // Left-top
    {
        setPosition(sf::Vector2f(0.0f,0.0f));
        break;
    }
    }

}

void CVTitleBar::setGlowColor(const sf::Color& color)
{
    followGlow.setColor(color);
}

void CVTitleBar::setButtonColor(const sf::Color& color)
{
    if(bCanClose)
    {
        closeButton->setSpriteColor(color);
    }
    if(bCanMinimize)
    {
        minimizeButton->setSpriteColor(color);
    }
    if(bCanResize)
    {
        resizeButton->setSpriteColor(color);
    }
}

void CVTitleBar::setIconColor(const sf::Color& color)
{
    for(auto& sprite : spriteList)
    {
        sprite.setColor(color);
    }
}

void CVTitleBar::setMouseGlow(const bool& state, const std::string& texture)
{
    if(!state)
    {
        bMouseGlow = state;
        return;
    }

    setDrawClipping(true);

    bMouseGlow = true;
    followGlow.setTexture(*appTexture(texture));
    followGlow.setOrigin(sf::Vector2f(appTexture(texture)->getSize())/2);
    followGlow.setScale(sf::Vector2f(getSize().y * 10, getSize().y * 10)/appTexture(texture)->getSize());
    followGlow.setPosition(getPosition() + getSize()/2);
    followGlow.setColor(sf::Color(255,255,255,0));
}

bool CVTitleBar::draw(sf::RenderTarget* target)
{
    if(!CVTextBox::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    if(bCanClose) closeButton->draw(target);
    if(bCanResize) resizeButton->draw(target);
    if(bCanMinimize) minimizeButton->draw(target);

    if(bMouseGlow) target->draw(followGlow);

    CV_DRAW_CLIP_END

    return true;
}

bool CVTitleBar::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVTextBox::update(event, mousePos)) return false;

    if(bCanClose)
    {
        closeButton->update(event, mousePos);
        if(closeButton->getTrigger()) event.close();
    }

    if(bCanMinimize)
    {
        minimizeButton->update(event, mousePos);
        if(minimizeButton->getTrigger())
        {
            View->minimize();
        }
    }

    if(bCanResize)
    {
        resizeButton->update(event, mousePos);
        if(resizeButton->getTrigger())
        {
            if(View->isMaximized())
            {
                View->restore();
            }
            else
            {
                View->maximize();
            }
        }
    }

    if(event.LMBhold &&
            bounds.contains(event.LMBpressPosition) &&
            event.captureMouse())
    {

        View->viewPort->setPosition(View->viewPort->getPosition() + (sf::Vector2f(sf::Mouse::getPosition()) - event.lastFrameGlobalMousePosition));

    }

    if(bMouseGlow && (mousePos.y - followGlow.getPosition().y < fGlowDistance))
    {
        followGlow.setPosition(mousePos.x, followGlow.getPosition().y);
        sf::Color tmp = followGlow.getColor();

        float glowFactor = 1.0f - abs(mousePos.y - followGlow.getPosition().y)/fGlowDistance;

        if(glowFactor < 0.0f)
        {
            glowFactor = 0.0f;
        }

        tmp.a = 60 * glowFactor;
        followGlow.setColor(tmp);
        followGlow.setScale(sf::Vector2f(getSize().y * 10, getSize().y * 10)/followGlow.getTexture()->getSize() * glowFactor);
    }

    return true;
}

CVTitleBar::~CVTitleBar()
{
    if(bCanClose) delete(closeButton);
    if(bCanMinimize) delete(minimizeButton);
    if(bCanResize) delete(resizeButton);
}

CVScrollBar::CVScrollBar(CVView* parentView, const sf::Vector2f& anchorBegin,
                         const sf::Vector2f& anchorEnd, const float& panelLength,
                         const float& width, const sf::Color& fillColor,
                         const sf::Color& borderColor, const float& borderWidth,
                         const float& scrollSpeed, const uint8_t& animType):
    CVBox(parentView, anchorBegin, width, getDistance(anchorBegin, anchorEnd), fillColor,
          borderColor, borderWidth),
    anchorBegin(anchorBegin),
    anchorEnd(anchorEnd),
    scrollPos(0.0f),
    scrollMax(0.0f),
    panelSize(panelLength),
    scrollAngle(get_angle(anchorBegin, anchorEnd)),
    scrollSpeed(scrollSpeed),
    minLength(12.0f),
    sensitivity(20.0f),
    animType(animType),
    responseDir(CV_WHEEL_DIR_VERTICAL),
    bCanScroll(true),
    bReleaseOnEnd(false),
    bReverseY(false),
    bReverseX(false)
{
    panel.front().setOrigin(width/2, 0.0f);
    panel.front().setRotation(scrollAngle*180.0f/PI - 90.0f);

    setRounding(width/2);

    calcHighlightBounds();
}

CVScrollBar::~CVScrollBar()
{

    for(auto& item : panel)
    {
        View->stopAnim(&item);
    }

}

void CVScrollBar::calcHighlightBounds()
{
    if(anchorBegin.x < anchorEnd.x)
    {
        mouseHighlightBounds.left = anchorBegin.x - 4*bounds.width;
        mouseHighlightBounds.width = anchorEnd.x - anchorBegin.x + 8*bounds.width;
    }
    else
    {
        mouseHighlightBounds.left = anchorEnd.x - 4*bounds.width;
        mouseHighlightBounds.width = anchorBegin.x - anchorEnd.x + 8*bounds.width;
    }

    if(anchorBegin.y < anchorEnd.y)
    {
        mouseHighlightBounds.top = anchorBegin.y - 4*bounds.width;
        mouseHighlightBounds.height = anchorEnd.y - anchorBegin.y + 8*bounds.width;
    }
    else
    {
        mouseHighlightBounds.top = anchorEnd.y - 4*bounds.width;
        mouseHighlightBounds.height = anchorBegin.y - anchorEnd.y + 8*bounds.width;
    }
}

bool CVScrollBar::scroll(const float& delta)
{
    if(((delta > 0.0f) && (scrollPos == scrollMax - panelSize)) ||
            ((delta < 0.0f) && (scrollPos == 0.0f))) return false;

    if(delta + scrollPos + panelSize > scrollMax)
    {
        scrollPos = scrollMax - panelSize;
    }
    else if(delta + scrollPos < 0.0f)
    {
        scrollPos = 0.0f;
    }
    else
    {
        scrollPos += std::round(delta);
    }

    return true;
}

void CVScrollBar::setAnchorPoints(const sf::Vector2f& anchorBegin,
                                  const sf::Vector2f& anchorEnd)
{
    this->anchorBegin = anchorBegin;
    this->anchorEnd = anchorEnd;
    scrollAngle = get_angle(anchorBegin, anchorEnd);
    panel.front().setRotation(scrollAngle*180.0f/PI - 90.0f);
    calcHighlightBounds();
}

void CVScrollBar::move(const sf::Vector2f& distance)
{
    CVBox::move(distance);
    anchorBegin += distance;
    anchorEnd += distance;
    mouseHighlightBounds.left += distance.x;
    mouseHighlightBounds.top += distance.y;
}

void CVScrollBar::setPosition(const sf::Vector2f& newPosition)
{
    move(newPosition - getPosition() + origin);
}

bool CVScrollBar::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBox::update(event, mousePos)) return false;

    if(scrollMax > panelSize)
    {
        float barSize = panelSize*panelSize/scrollMax;
        if(barSize < minLength) barSize = minLength;

        panel.front().setSize(sf::Vector2f(getSize().x, barSize));
        View->anim_to(&panel.front(),
                      radial_position(anchorBegin, scrollPos/scrollMax*(getDistance(anchorBegin,anchorEnd)), scrollAngle),
                      scrollSpeed*pow((getDistance(anchorBegin, anchorEnd) - panel.front().getSize().y), 2)/scrollMax,
                      CV_OBJ_ANIM_SLIDE, true);
    }
    else
    {
        panel.front().setSize(sf::Vector2f(getSize().x, 0.0f));
        panel.front().setPosition(anchorBegin);
    }

    if((bCanScroll &&
            (((responseDir & CV_WHEEL_DIR_VERTICAL) && (event.mouseWheelDelta.y != 0.0f)) ||
             ((responseDir & CV_WHEEL_DIR_HORIZONTAL) && (event.mouseWheelDelta.x != 0.0f)))) ||
            mouseHighlightBounds.contains(mousePos))
    {
        if(animType == CV_OBJ_ANIM_FADE_IN)
        {
            if(panel.front().getFillColor().a < 255_BIT) View->anim_passive(&panel.front(), 1000.0f, CV_OBJ_ANIM_FADE_IN);
        }
    }
    else
    {
        if(animType == CV_OBJ_ANIM_FADE_IN)
        {
            if(panel.front().getFillColor().a > 0_BIT) View->anim_passive(&panel.front(), 30.0f, CV_OBJ_ANIM_FADE_OUT);
        }
    }

    if(event.viewHasFocus && bCanScroll)
    {
        if((responseDir & CV_WHEEL_DIR_VERTICAL) && (event.mouseWheelDelta.y != 0.0f))
        {
            if(bReverseY)
            {
                if(!bReleaseOnEnd | scroll(event.mouseWheelDelta.y*sensitivity))
                {
                    event.mouseWheelDelta.y = 0.0f; // Capture mouse wheel event
                }
            }
            else
            {
                if(!bReleaseOnEnd | scroll(-event.mouseWheelDelta.y*sensitivity))
                {
                    event.mouseWheelDelta.y = 0.0f;
                }
            }
        }
        if((responseDir & CV_WHEEL_DIR_HORIZONTAL) && (event.mouseWheelDelta.x != 0.0f))
        {
            if(bReverseX)
            {
                if(!bReleaseOnEnd | scroll(event.mouseWheelDelta.x*sensitivity))
                {
                    event.mouseWheelDelta.x = 0.0f;
                }
            }
            else
            {
                if(!bReleaseOnEnd | scroll(-event.mouseWheelDelta.x*sensitivity))
                {
                    event.mouseWheelDelta.x = 0.0f;
                }
            }
        }
        if(event.LMBhold && mouseHighlightBounds.contains(event.LMBpressPosition))
        {

            float dist = getDistance(anchorBegin, mousePos) - panel.front().getSize().y/2;

            if(dist < 0.0f)
            {
                setScrollOffset(0.0f);
            }
            else
            {

                float anchor_dist = getDistance(anchorBegin, anchorEnd);
                double mouse_angle = get_angle(anchorBegin, mousePos);
                double anchor_angle = get_angle(anchorBegin, anchorEnd);

                setScrollOffset(dist * cos(mouse_angle - anchor_angle) / anchor_dist * scrollMax);

            }

        }
    }

    return true;
}

void CVScrollBar::setScrollOffset(const float& newOffset)
{
    if(newOffset + panelSize > scrollMax) scrollPos = scrollMax - panelSize;
    else if(newOffset < 0.0f) scrollPos = 0.0f;
    else scrollPos = newOffset;

    if(scrollPos < 0.0f) scrollPos = 0.0f;

    View->anim_to(&panel.front(),
                  radial_position(anchorBegin, scrollPos/scrollMax*(getDistance(anchorBegin,anchorEnd)), scrollAngle),
                  scrollSpeed*pow((getDistance(anchorBegin, anchorEnd) - panel.front().getSize().y), 2)/scrollMax,
                  CV_OBJ_ANIM_SLIDE, true);
}

CVProgressBar::CVProgressBar(CVView* View,
                             const sf::Vector2f& position,
                             const float& width,
                             const float& height,
                             const TextEntry& textInfo,
                             const sf::Color& fillColor,
                             const sf::Color& progressColor,
                             const sf::Color& borderColor,
                             const float& borderWidth):
    CVTextBox(View,
              position,
              width,
              height,
              textInfo,
              fillColor,
              borderColor,
              borderWidth),
    progress(0.0f),
    animSpeed(5.0f),
    progressColor(colorTheme.front())
{
    panel.emplace_back(sf::Vector2f(0.0f, height));

    colorTheme.emplace_back(progressColor);
    this->progressColor = std::ref(colorTheme.back());
    progressBar().setFillColor(getProgressColor());
    progressBar().setOrigin(origin);
    progressBar().setPosition(getPosition());

    if(textFont != nullptr)
    {
        alignText();
    }
}

void CVProgressBar::alignText()
{
    sf::FloatRect textBounds = progressText().getGlobalBounds();
    sf::Vector2f textPos(bounds.left, bounds.top + bounds.height/2);

    switch(textInfo.alignment)
    {
    case ALIGN_CENTER:
    {
        textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
        textPos.y = bounds.top - textBounds.height;
        break;
    }
    case ALIGN_CENTER_MIDLINE:
    {
        textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
        textPos.y = bounds.top + bounds.height - textBounds.height/2 - getTextCenterOffsetY(progressText())/2;
        break;
    }
    case ALIGN_CENTER_BOTTOM:
    {
        textPos.x = bounds.left + bounds.width/2 - textBounds.width/2;
        textPos.y = bounds.top + bounds.height;
        break;
    }
    case ALIGN_RIGHT:
    {
        textPos.x = bounds.left + bounds.width - textBounds.width;
        textPos.y = bounds.top - textBounds.height;
        break;
    }
    case ALIGN_RIGHT_MIDLINE:
    {
        textPos.x = bounds.left + bounds.width - textBounds.width;
        textPos.y = bounds.top + bounds.height - textBounds.height/2 - getTextCenterOffsetY(progressText())/2;
        break;
    }
    case ALIGN_RIGHT_BOTTOM:
    {
        textPos.x = bounds.left + bounds.width - textBounds.width;
        textPos.y = bounds.top + bounds.height;
        break;
    }
    case ALIGN_LEFT_MIDLINE:
    {
        textPos.x = bounds.left;
        textPos.y = bounds.top + bounds.height - textBounds.height/2 - getTextCenterOffsetY(progressText());
        break;
    }
    case ALIGN_LEFT_BOTTOM:
    {
        textPos.x = bounds.left;
        textPos.y = bounds.top + bounds.height;
        break;
    }
    default:  // Left
    {
        textPos.x = bounds.left;
        textPos.y = bounds.top - textBounds.height*2;
        break;
    }
    }

    progressText().setPosition(textPos);
}

void CVProgressBar::setProgressColor(const sf::Color& newColor)
{
    progressColor.get() = newColor;
    progressBar().setFillColor(newColor);
}

void CVProgressBar::setProgress(const float& newProgress)
{
    if(newProgress < 0.0f) progress = 0.0f;
    else if(newProgress > 1.0f) progress = 1.0f;
    else progress = newProgress;
}

bool CVProgressBar::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVTextBox::update(event, mousePos)) return false;

    alignText();
    progressBar().setSize(sf::Vector2f(progressBar().getSize().x + (progress*bounds.width - progressBar().getSize().x)*animSpeed/100.0f,
                                       progressBar().getSize().y));

    return true;
}

}
