#include "cvision/cvis_word.hpp"

namespace cvis{

CVTypePanel::CVTypePanel(view* parentView, const std::string& panelTag,
                         const sf::Color& color, const sf::Vector2f& size,
                         const std::vector<float>& margins = std::vector<float>(4, 25.0f),
                         const textEntry& textInfo):
                    CVViewPanel(parentView, panelTag, color, size, (!isnan(size.x) && !isnan(size.y))),
                    margins(margins){
    addTypeElement(sf::Vector2f(left_margin(),top_margin()));
}

bool CVTypePanel::update(CVEvent& event, const sf::Vector2f& mousePos){
    if(!CVTextBox::update(event, mousePos)) return false;

    return true;
}

bool CVTypePanel::draw(){
    if(!CVTextBox::draw()) return false;

    return true;
}

void CVTypePanel::addTextElement(const sf::Vector2f& position, textEntry textInfo){
    if(textInfo.font.size() < 1){
        textInfo.font = this->textInfo.font;
        if(textInfo.font.size() < 1){
            std::cout << "No fonts available to create text entry in type panel\n";
            return;
        }
    }

    typeElements.emplace_back(View, position, bounds.width - right_margin(), 20.0f*textInfo.fontSize/12,
                              textInfo, sf::Color::Transparent, textInfo.textColor, 0.0f, CV_CURSOR_ANIM_FADE,
                              CV_TEXT_FIT_WRAP);
}

}

