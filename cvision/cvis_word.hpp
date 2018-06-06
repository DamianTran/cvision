#pragma once

#ifndef CVIS_WORD
#define CVIS_WORD

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_typeIO.hpp"

namespace cvis{

class CVTypePanel: public CVViewPanel{
protected:
    std::vector<CVTypeBox> typeElements;
    std::vector<float> margins;

public:

    inline float& left_margin(){ return margins[0]; }
    inline float& top_margin(){ return margins[1]; }
    inline float& right_margin(){ return margins[2]; }
    inline float& bottom_margin(){ return margins[3]; }

    bool draw();
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void addTextElement(const sf::Vector2f& position, const textEntry& textInfo = textEntry());

    CVTypePanel(view* parentView, const std::string& panelTag = "",
                const sf::Color& color = sf::Color::White,
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                bool fitToWindow = true);
};

}



#endif // CVIS_WORD
