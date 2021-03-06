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

#include "cvision/button.hpp"
#include "cvision/event.hpp"
#include "cvision/view.hpp"
#include "cvision/app.hpp"

using namespace hyperC;
using namespace std;

namespace cvis
{

CVButton::CVButton(CVView* View,
                   const sf::Vector2f& position,
                   const float& width,
                   const float& height,
                   const TextEntry& textInfo,
                   const string& icon,
                   const sf::Color& fillColor,
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
    bFadeInHover(false),
    bFadeInToggle(false),
    bToggleFlipX(false),
    bToggleFlipY(false),
    rotateAngle(NAN),
    rotateType(CV_LAYER_ALL),
    responseEvent(CV_EVENT_LMB),
    baseAlpha(fillColor.a),
    activateFunc(nullptr),
    stateColors(
{
    fillColor
})
{

    if(!icon.empty())
    {
        const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
        sf::Vector2f texSize(iconTex->getSize());

        float scale = width < height ? width/iconTex->getSize().x : height/iconTex->getSize().y;

        addSprite(iconTex,
                  sf::Vector2f(width/2, height/2),
                  sf::Vector2f(texSize*scale));

        stateTextures.push_back(iconTex);
    }

    canHighlight = true;

}

CVButton::CVButton(CVView* View,
                   const sf::Vector2f& position,
                   const float& width,
                   const float& height,
                   const string& icon,
                   const unsigned int& numStates,
                   const unsigned int& initialState,
                   const bool& bHighlight,
                   void (*activateFunc)(const unsigned int&, CVView*)):
    CVTextBox(View,
              position - sf::Vector2f(width/2, height/2),
              width,
              height,
              sf::Color::Transparent,
              sf::Color::Transparent,
              0.0f),
    bFadeInHover(false),
    bFadeInToggle(false),
    bToggleFlipX(false),
    bToggleFlipY(false),
    rotateAngle(NAN),
    rotateType(CV_LAYER_ALL),
    responseEvent(CV_EVENT_LMB),
    baseAlpha(255),
    activateFunc(activateFunc),
    stateColors(
{
    sf::Color(200,200,200)
})
{

    stateNum = initialState;
    setNumStates(numStates);

    bSpriteOnly = true;

    setHighlightableStatus(bHighlight);

    const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
    addSprite(iconTex, sf::Vector2f(width/2, height/2), sf::Vector2f(width, height));
    origin.x = width/2;
    origin.y = height/2;

    stateTextures.push_back(iconTex);

    colorTheme[0] = sf::Color(200,200,200);

}

void CVButton::setColor(const sf::Color& newColor, const unsigned int& state)
{

    while(colorTheme.size() < 2)
    {
        colorTheme.emplace_back(sf::Color::Transparent);
    }

    while(stateColors.size() < state)
    {
        stateColors.emplace_back(baseFillColor());
    }

    CVBox::setColor(newColor);
}

void CVButton::setFadeIn(const bool& state, const uint8_t& targetAlpha,
                         const uint8_t& fadeSpeed)
{
    fadeRate = fadeSpeed;
    baseAlpha = targetAlpha;
    if(state)
    {
        for(auto& color : colorTheme)
        {
            color.a = 0;
        }
    }
    else
    {
        for(auto& color : colorTheme)
        {
            color.a = targetAlpha;
        }
    }
    bFadeInHover = state;
}

void CVButton::setFadeOnClick(const bool& state, const uint8_t& targetAlpha,
                              const uint8_t& fadeSpeed)
{
    fadeRate = fadeSpeed;
    baseAlpha = targetAlpha;

    if(state)
    {
        for(auto& color : colorTheme)
        {
            color.a = 0;
        }
    }
    else
    {
        for(auto& color : colorTheme)
        {
            color.a = targetAlpha;
        }
    }

    bFadeInToggle = state;
}

void CVButton::setStateTexture(const unsigned int& state,
                               const string& texture)
{
    if(stateTextures.size() <= state)
    {
        if(!spriteList.empty())
        {
            stateTextures.resize(state + 1, getSprite(0).getTexture());
        }
        else
        {
            stateTextures.resize(state + 1, nullptr);
        }
    }

    if(texture.empty())
    {
        stateTextures[state] = nullptr;
    }
    else
    {
        stateTextures[state] = appTexture(texture);
    }

    if(!spriteList.empty() && (state == stateNum))
    {
        getSprite(0).setTexture(*stateTextures[state]);
    }
}

void CVButton::setSize(const sf::Vector2f& newSize)
{

    sf::Vector2f newScale = newSize/getSize();
    sf::Vector2f dist;

    CVTextBox::setSize(newSize);

    for(auto& sprite : spriteList)
    {
        dist = sprite.getPosition() - sf::Vector2f(bounds.left, bounds.top);
        dist *= newScale;
        sprite.setPosition(round(sf::Vector2f(bounds.left, bounds.top) + dist));
    }

}

bool CVButton::update(CVEvent& event, const sf::Vector2f& mousePos)
{

    if(!CVTextBox::update(event, mousePos))
    {
        return false;
    }

    if(active && !View->cursor_overriden() && event.focusFree() && bounds.contains(mousePos))
    {
        event.setCursor(sf::Cursor::Hand);
    }

    if(take_trigger("Toggle") == "TRUE")
    {
        toggle();
    }

    if(bounds.contains(mousePos) &&
       event.captureMouse())
    {

        setFocus(true);

        if((responseEvent & CV_EVENT_LMB) &&
            bounds.contains(event.LMBreleasePosition))
        {
            if((event.LMBreleaseFrames == 1) &&
                    (event.LMBholdTime < 0.5f) &&
                    (hasFocus() || event.captureFocus()))  // Cycle states
            {
                toggle();
            }
        }

        if((responseEvent & CV_EVENT_RMB) &&
            bounds.contains(event.RMBreleasePosition))
        {
            if((event.RMBreleaseFrames == 1) &&
               (event.RMBholdTime < 0.5f) &&
               (hasFocus() || event.captureFocus()))
            {
                toggle();
            }
        }

    }

    if(bFadeInHover)
    {
        if(bounds.contains(mousePos))
        {
            setFade(baseAlpha, fadeRate);
        }
        else
        {
            setFade(0, fadeRate);
        }
    }

    return true;

}

void CVButton::toggle()
{

    if(!active) return;

    sendTriggers();

    if(stateNum + 1 < numStates)
    {
        ++stateNum;
    }
    else
    {
        stateNum = 0;
    }

    if(this->activateFunc)
    {
        activateFunc(stateNum, View);
    }

    if(!spriteList.empty() &&
       (stateNum < stateTextures.size()))
    {
        getSprite(0).setTexture(*stateTextures[stateNum]);
    }

    if(!isnan(rotateAngle))
    {
        rotate(rotateAngle, rotateType);
    }

    if(bToggleFlipX)
    {
        spriteList.front().setScale(-spriteList.front().getScale().x, spriteList.front().getScale().y);
    }
    if(bToggleFlipY)
    {
        spriteList.front().setScale(spriteList.front().getScale().x, -spriteList.front().getScale().y);
    }
    bTriggered = true;

    if(bFadeInToggle)
    {
        if(stateNum == 0)
        {
            setFade(0, fadeRate);
        }
        else
        {
            setFade(baseAlpha, fadeRate);
        }
    }
}

void CVButton::setState(const unsigned int& state)
{
    if(state >= numStates)
    {
        while(stateNum + 1 != numStates)
        {
            toggle();
        }
    }
    else
    {
        while(stateNum != state)
        {
            toggle();
        }
    }
}

bool CVButton::draw(sf::RenderTarget* target)
{
    if(!target || !visible) return false;

    CV_DRAW_CLIP_BEGIN

    if(bDropShadow)
    {
        target->draw(dropShadow);
    }

    if(!bSpriteOnly)
    {
        for(auto& item : panel)
        {
            target->draw(item);
        }
    }

    if(bMasked) target->draw(shapeMask);

    for(auto& spr : spriteList)
    {
        target->draw(spr);
    }
    for(auto& text : displayText)
    {
        target->draw(text);
    }

    if(is_closable())
    {
        closeButton->draw(target);
    }

    if(!active) target->draw(inactiveMask);

    CV_DRAW_CLIP_END

    return true;
}

}
