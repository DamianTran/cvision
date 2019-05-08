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

#pragma once

#ifndef CVIS_BOX
#define CVIS_BOX

#include <vector>

#include "cvision/lib.hpp"

#include <SFML/Graphics.hpp>
#include "cvision/shape.hpp"

namespace cvis
{

class CVView;
class CVEvent;

class CVISION_API CVBox: public CVShape
{
protected:

    std::vector<rounded_rectangle> panel;
    std::vector<rounded_rectangle> mask;

    bool bNoFill; // For proper fade handling

public:

    void updateTriggers() { }

    CVISION_API bool draw(sf::RenderTarget* target) override;

    CVISION_API void setPosition(const sf::Vector2f& position) override;
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& offset) override;
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setColor(const sf::Color& newColor) override;
    inline void setColor(const int& r, const int& g, const int& b)
    {
        setColor(sf::Color(r, g, b));
    }

    CVISION_API void setSpriteColor(const sf::Color& newColor);
    inline void setSpriteColor(const int& r, const int& g, const int& b)
    {
        setSpriteColor(sf::Color(r, g, b));
    }

    CVISION_API void setFillColor(const sf::Color& newColor);
    inline void setFillColor(const int& r, const int& g, const int& b)
    {
        setFillColor(sf::Color(r, g, b));
    }

    CVISION_API void setOutlineColor(const sf::Color& newColor);
    inline void setOutlineColor(const int& r, const int& g, const int& b)
    {
        setOutlineColor(sf::Color(r, g, b));
    }

    CVISION_API void setTexture(const sf::Texture* texture,
                                const sf::IntRect& textureRect = sf::IntRect(INT_MAX, INT_MAX, INT_MAX, INT_MAX));
    CVISION_API void setTexture(const std::string& texture_name,
                                const sf::IntRect& textureRect = sf::IntRect(INT_MAX, INT_MAX, INT_MAX, INT_MAX));

    CVISION_API void setRounding(const float& newRadius,
                     const unsigned int& pointCount = 10,
                     const std::vector<bool>& states =
                         std::vector<bool>(4, true));
    CVISION_API const float& getRoundingRadius() const;
    CVISION_API const unsigned int& getRoundingPointCount() const;

    CVISION_API sf::Color getFillColor() const override;
    CVISION_API sf::Color getColor() const;
    CVISION_API sf::Color getOutlineColor() const override;

    CVISION_API void setOutlineThickness(const float& newThickness);

    CVISION_API void setClearFill(const bool& state = true);

    CVISION_API void setExpand(const bool& state) override;
    CVISION_API void updateBounds() override;

    CVISION_API void rotate(const float& angle,
                const unsigned char& flags = CV_LAYER_ALL,
                const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) override;
    CVISION_API void setRotation(const float& angle,
                     const unsigned char& flags = CV_LAYER_ALL,
                     const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) override;
    CVISION_API void setSize(const sf::Vector2f& newSize) override;
    inline void setSize(const float& x, const float& y)
    {
        setSize(sf::Vector2f(x, y));
    }

    CVISION_API float getRotation(const unsigned char& flags = CV_LAYER_ALL) const override;

    inline const rounded_rectangle& getPanelObject(const unsigned int order)
    {
        return panel[order];
    }
    inline const rounded_rectangle& getTopPanel()
    {
        return panel.back();
    }
    inline const rounded_rectangle& getBottomPanel()
    {
        return panel.front();
    }
    inline unsigned int numPanelObjects() const
    {
        return panel.size();
    }

    CVISION_API void highlight(const bool& state = true);

    inline float getOutlineThickness() const override
    {
        return panel.front().getOutlineThickness();
    }

    void setFillTexture(sf::Texture* texture) override{ }
    inline sf::Vector2f getSize() override
    {
        return sf::Vector2f(bounds.width, bounds.height);
    }

    CVISION_API bool fadeComplete() const noexcept override;

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos) override;

    CVISION_API CVBox(CVView* View,
          sf::Vector2f position = sf::Vector2f(0.0f,0.0f),
          float width = 0.0f,
          float height = 0.0f,
          sf::Color fillColor = sf::Color::Transparent,
          sf::Color borderColor = sf::Color::Transparent,
          float borderWidth = 0.0f);
    ~CVBox() = default;

};

}

#endif // CVIS_BOX

