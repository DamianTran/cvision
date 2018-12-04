#include "cvision/cvis_buttonIO.hpp"

using namespace AIALG;

namespace cvis{

CVButton::CVButton(CVView* View, const sf::Vector2f& position, float width, float height,
                const textEntry& textInfo, const std::string& icon, sf::Color fillColor, sf::Color borderColor,
                float borderWidth):
                    CVTextBox(View, position, width, height, textInfo, fillColor, borderColor, borderWidth),
                    bFadeInHover(false),
                    bFadeInToggle(false),
                    bTriggered(false),
                    bToggleFlipX(false),
                    bToggleFlipY(false),
                    rotateAngle(NAN),
                    rotateType(CV_LAYER_ALL),
                    responseEvent(CV_EVENT_LMB),
                    baseAlpha(fillColor.a),
                    activateFunc(nullptr),
                    stateColors({fillColor})
                {

    if(icon.size() > 0){
        const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
        sf::Vector2f texSize(iconTex->getSize());

        addSprite(iconTex,
              sf::Vector2f(width/2 - texSize.x/2, height/2 - texSize.y/2),
              sf::Vector2f(position.x - borderWidth, position.y - borderWidth));

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
                 bTriggered(false),
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
    if(stateNum < numStates - 1){
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
    if(target == nullptr) return false;
    if(!bSpriteOnly){
        for(auto& item : panel){
            target->draw(item);
        }
    }
    target->draw(shapeMask);
    for(auto& spr : spriteList){
        target->draw(spr);
    }
    for(auto& text : displayText){
        target->draw(text);
    }
    return true;
}

}
