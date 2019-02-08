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

using namespace EZC;

namespace cvis{

CVButton::CVButton(CVView* View, const sf::Vector2f& position, float width, float height,
                const textEntry& textInfo, const std::string& icon, sf::Color fillColor, sf::Color borderColor,
                float borderWidth):
                    CVTextBox(View, position, width, height, textInfo, fillColor, borderColor, borderWidth),
                    bFadeInHover(false),
                    bFadeInToggle(false),
                    bToggleFlipX(false),
                    bToggleFlipY(false),
                    rotateAngle(NAN),
                    rotateType(CV_LAYER_ALL),
                    responseEvent(CV_EVENT_LMB),
                    baseAlpha(fillColor.a),
                    activateFunc(nullptr),
                    stateColors({fillColor})
                {

    if(!icon.empty()){
        const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
        sf::Vector2f texSize(iconTex->getSize());

        float scale = width < height ? width/iconTex->getSize().x : height/iconTex->getSize().y;

        addSprite(iconTex,
              sf::Vector2f(width/2, height/2),
              sf::Vector2f(texSize*scale));

        stateTextures.push_back(iconTex);
    }

    canHighlight = true;
    highlightColor = fillColor;
    brighten(highlightColor, 40);

}

CVButton::CVButton(CVView* View, const sf::Vector2f& position, const float& width,
             const float& height, const std::string& icon, const unsigned int& numStates,
             const unsigned int& initialState, const bool& bHighlight,
             void (*activateFunc)(const unsigned int&, CVView*)):
                 CVTextBox(View, position, width, height, sf::Color::Transparent, sf::Color::Transparent, 0.0f),
                 bFadeInHover(false),
                 bFadeInToggle(false),
                 bToggleFlipX(false),
                 bToggleFlipY(false),
                 rotateAngle(NAN),
                 rotateType(CV_LAYER_ALL),
                 responseEvent(CV_EVENT_LMB),
                 baseAlpha(255),
                 activateFunc(activateFunc),
                 stateColors({sf::Color(200,200,200)}){

    stateNum = initialState;
    setNumStates(numStates);

    bSpriteOnly = true;

    setHighlightableStatus(bHighlight);

    const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
    addSprite(iconTex, sf::Vector2f(0.0f,0.0f), sf::Vector2f(width, height));
    origin.x = width/2;
    origin.y = height/2;

    stateTextures.push_back(iconTex);

    colorTheme[0] = sf::Color(200,200,200);
    highlightColor = sf::Color(255,255,255);

}

void CVButton::setColor(const sf::Color& newColor, const unsigned int& state){
    while(colorTheme.size() < 2) colorTheme.emplace_back(sf::Color::Transparent);
    while(stateColors.size() < state){
        stateColors.emplace_back(baseFillColor());
    }

    colorTheme[0] = newColor;
    highlightColor = newColor;
    brighten(highlightColor, 40);
}

void CVButton::setFadeIn(const bool& state, const uint8_t& targetAlpha,
                         const uint8_t& fadeSpeed){
    fadeRate = fadeSpeed;
    baseAlpha = targetAlpha;
    if(state){
        for(auto& color : colorTheme){
            color.a = 0;
        }
    }
    else{
        for(auto& color : colorTheme){
            color.a = targetAlpha;
        }
    }
    bFadeInHover = state;
}

void CVButton::setFadeOnClick(const bool& state, const uint8_t& targetAlpha,
                              const uint8_t& fadeSpeed){
    fadeRate = fadeSpeed;
    baseAlpha = targetAlpha;
    if(state){
        for(auto& color : colorTheme){
            color.a = 0;
        }
    }
    else{
        for(auto& color : colorTheme){
            color.a = targetAlpha;
        }
    }
    bFadeInToggle = state;
}

bool CVButton::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(!CVTextBox::update(event, mousePos)) return false;

    if((responseEvent & CV_EVENT_LMB) &&
       bounds.contains(event.LMBpressPosition) &&
       bounds.contains(event.LMBreleasePosition)){
       if((event.LMBreleaseFrames == 1) &&
          (event.LMBholdTime < 0.5f) &&
            (hasFocus() || event.captureFocus()) &&
           event.captureMouse()){ // Cycle states
                toggle();
       }
    }
    if((responseEvent & CV_EVENT_RMB) &&
       bounds.contains(event.RMBpressPosition) &&
       bounds.contains(event.RMBreleasePosition)){
       if((event.RMBreleaseFrames == 1) &&
          (event.RMBholdTime < 0.5f) &&
            (hasFocus() || event.captureFocus()) &&
           event.captureMouse()){
                toggle();
       }
    }

    if(bFadeInHover){
        if(bounds.contains(mousePos)){
            setFade(baseAlpha, fadeRate);
        }
        else{
            setFade(0, fadeRate);
        }
    }

    return true;

}

void CVButton::toggle(){

    if(!active) return;

    sendTriggers();

    if(stateNum + 1 < numStates){
        ++stateNum;
        if(this->activateFunc != nullptr) activateFunc(stateNum, View);
    }
    else{
        stateNum = 0;
        if(this->activateFunc != nullptr) activateFunc(stateNum, View);
    }

    if(!isnan(rotateAngle)){
        rotate(rotateAngle, rotateType);
    }

    if(bToggleFlipX){
        spriteList.front().setScale(-spriteList.front().getScale().x, spriteList.front().getScale().y);
    }
    if(bToggleFlipY){
        spriteList.front().setScale(spriteList.front().getScale().x, -spriteList.front().getScale().y);
    }
    bTriggered = true;

    if(bFadeInToggle){
        if(stateNum == 0){
            setFade(0, fadeRate);
        }
        else{
            setFade(baseAlpha, fadeRate);
        }
    }
}

void CVButton::setState(const unsigned int& state){
    while(stateNum != state){
       toggle();
    }
}

bool CVButton::draw(sf::RenderTarget* target){
    if(!target) return false;

    if(!bSpriteOnly){
        for(auto& item : panel){
            target->draw(item);
        }
    }

    if(bMasked) target->draw(shapeMask);

    for(auto& spr : spriteList){
        target->draw(spr);
    }
    for(auto& text : displayText){
        target->draw(text);
    }

    if(is_closable())
    {
        closeButton->draw(target);
    }

    if(!active) target->draw(inactiveMask);

    return true;
}

}
