#pragma once

#ifndef CVIS_WORD
#define CVIS_WORD

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_typeIO.hpp"
#include "cvision/widgets.hpp"

namespace cvis{

class CVTypePanel: public CVViewPanel{
protected:
    std::vector<CVTypeBox> typeElements;
    std::vector<float> margins;
    std::vector<sf::RectangleShape> marginLines,
                                    highlight;

    float scrollBarPadding;

    ColorTheme highlightColors;

    AIALG::StringVector waitingHighlightText;
    cvis::CVScrollBar scrollBarY;

    bool bShowMarginLines,
        bCanEdit;

    inline sf::RectangleShape& left_margin_line(){ return marginLines[0]; }
    inline sf::RectangleShape& top_margin_line(){ return marginLines[1]; }
    inline sf::RectangleShape& right_margin_line(){ return marginLines[2]; }
    inline sf::RectangleShape& bottom_margin_line(){ return marginLines[3]; }

public:



    inline const float& left_margin(){ return margins[0]; }
    inline const float& top_margin(){ return margins[1]; }
    inline const float& right_margin(){ return margins[2]; }
    inline const float& bottom_margin(){ return margins[3]; }

    void set_left_margin(const float& newMargin);
    void set_right_margin(const float& newMargin);
    void set_top_margin(const float& newMargin);
    void set_bottom_margin(const float& newMargin);

    void show_margins();
    void hide_margins();

    inline void setMarginLines(const bool& state){ bShowMarginLines = state; }
    inline void setScrollBarPadding(const float& newPadding){ scrollBarPadding = newPadding; }

    bool setHighlight(const AIALG::StringVector& query);
    void setHighlightableStatus(const bool& status);

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void addTextElement(const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f), textEntry textInfo = textEntry());
    void getText(AIALG::StringVector& output);
    void setText(const AIALG::StringVector& newText);

    void setTextSize(const unsigned int& newSize);
    void setTextColor(const sf::Color& newColor);

    void disable_edit();
    void enable_edit();
    inline const bool& can_edit() const{ return bCanEdit; }

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    CVTypePanel(CVView* parentView,
                const textEntry& textInfo,
                const std::string& panelTag = "",
                const sf::Color& color = sf::Color::White,
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                const std::vector<float>& margins = std::vector<float>(4, 25.0f),
                const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
};

}



#endif // CVIS_WORD
