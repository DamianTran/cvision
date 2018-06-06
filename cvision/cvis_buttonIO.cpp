#include "cvision/cvis_buttonIO.hpp"

using namespace AIALG;

namespace cvis{

CVButton::CVButton(view* View, const sf::Vector2f& position, float width, float height,
                const textEntry& textInfo, const std::string& icon, sf::Color fillColor, sf::Color borderColor,
                float borderWidth):
                    CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
                    stateNum(0),
                    numStates(2),
                    bSpriteOnly(false)
                {

    if(icon.size() > 0){
        const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
        sf::Vector2f texSize(iconTex->getSize());

        addSprite(iconTex,
              sf::Vector2f(position.x + width/2 - texSize.x/2, position.y + height/2 - texSize.y/2),
              sf::Vector2f(position.x - borderWidth, position.y - borderWidth));
    }

    colorTheme = { fillColor, borderColor, textInfo.textColor };

    canHighlight = true;
    canClick = true;

    displayText.push_back(sf::Text(textInfo.text,
                *mainApp()->fonts[textInfo.font],
                textInfo.fontSize));
    sf::Rect<float> textBounds = displayText.back().getLocalBounds();

    displayText.back().setPosition(position.x + width/2 - textBounds.width/2,
                                   position.y + height/2 - textBounds.height);
    displayText.back().setFillColor(textInfo.textColor);

}

CVButton::CVButton(view* View, const sf::Vector2f& position, const float& width,
             const float& height, const std::string& icon, const unsigned int& numStates,
             const unsigned int& initialState, const bool& bHighlight,
             void (*activateFunc)(const unsigned int&, view*)):
                 CVTextBox(View, position, width, height, sf::Color::Transparent, sf::Color::Transparent, 0.0f),
                 stateNum(initialState),
                 numStates(numStates),
                 bSpriteOnly(true),
                 activateFunc(activateFunc){

    setHighlightableStatus(bHighlight);

    const sf::Texture* iconTex = View->mainApp->bitmaps.taggedTexture(icon);
    addSprite(iconTex, position, sf::Vector2f(width, height));
    bounds = spriteList.front().getGlobalBounds();

    colorTheme[0] = sf::Color(200,200,200);
    colorTheme[1] = sf::Color(255,255,255);

}

bool CVButton::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(!CVTextBox::update(event, mousePos)) return false;

    if(highlightable()){
        if(bounds.contains(mousePos)){
            if(bSpriteOnly) spriteList.front().setColor(highlightColor());
            else panel.front().setFillColor(highlightColor());
        }
        else{
            if(bSpriteOnly) spriteList.front().setColor(baseColor());
            else panel.front().setFillColor(baseColor());
        }
    }

    if(bounds.contains(mousePos) && event.LMBhold && (event.LMBholdFrames < 2) && event.captureMouse()){ // Cycle states
        if(stateNum < numStates - 1){
            ++stateNum;
            if(this->activateFunc != nullptr) activateFunc(stateNum, View);
        }
        else{
            stateNum = 0;
            if(this->activateFunc != nullptr) activateFunc(stateNum, View);
        }
    }

    return true;

}

}
