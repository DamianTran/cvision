#pragma once

#ifndef CVIS_BUTTONIO
#define CVIS_BUTTONIO

#include "cvision/cvis_base.hpp"

namespace cvis{

class CVButton:public CVTextBox{
protected:

    unsigned int stateNum,
                numStates;

    bool bSpriteOnly;

    void (*activateFunc)(const unsigned int&, view*);

public:

    inline const unsigned int& getCurrentState() const{ return stateNum; }
    inline void setNumStates(const unsigned int& num){ numStates = num; }

    inline sf::Color& baseColor(){ return colorTheme[0]; }
    inline sf::Color& highlightColor(){ return colorTheme[1]; }

    template<typename FUNC> void setActivationFunction(FUNC func){ activateFunc = func; } // Allow for custom activation functions by view

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVButton(view* View, const sf::Vector2f& position, float width, float height,
                const textEntry& textInfo, const std::string& icon = "",
                sf::Color fillColor = sf::Color(168, 168, 168),
                sf::Color borderColor = sf::Color(102,102,102),
                float borderWidth = 2.0f); // Standard box button
    CVButton(view* View, const sf::Vector2f& position, const float& width,
             const float& height, const std::string& icon, const unsigned int& numStates = 2,
             const unsigned int& initialState = 0,
             const bool& bHighlight = true,
             void (*activateFunc)(const unsigned int&, view*) = nullptr); // Sprite button

};

}

#endif // CVIS_BUTTONIO
