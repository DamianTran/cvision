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
// warranties.  By using CVision, you ackowledge and agree
// to this disclaimer.  Use of CVision in Users's programs
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

#include "cvision/box.hpp"
#include "cvision/shape.hpp"
#include "cvision/event.hpp"
#include "cvision/view.hpp"

namespace cvis
{

bool CVBox::draw(sf::RenderTarget* target)
{
    if(!CVShape::draw(target)) return false;

    CV_DRAW_CLIP_BEGIN

    if(!bSpriteOnly)
    {
        for(auto& item : panel)
        {
            target->draw(item);
        }
    }

    if(bMasked) target->draw(shapeMask);

    for(auto& sprite : spriteList)
    {
        target->draw(sprite);
    }

    if(!active) target->draw(inactiveMask);

    CV_DRAW_CLIP_END

    return true;
}

void CVBox::highlight(const bool& state)
{
    CVElement::highlight(state);
    if(highlighted)
    {
        for(auto& shape : panel)
        {
            shape.setFillColor(highlightColor);
        }
    }
    else
    {
        for(auto& shape : panel)
        {
            shape.setFillColor(baseFillColor());
        }
    }
}

void CVBox::setPosition(const sf::Vector2f& position)
{
    move(position - getPosition());
}

void CVBox::move(const sf::Vector2f& offset)
{

    if(bStatic) return;

    CVShape::move(offset);

    for(auto& item : panel)
    {
        item.move(offset);
    }
    for(auto& item : mask)
    {
        item.move(offset);
    }
}

void CVBox::setSize(const sf::Vector2f& newSize)
{
    sf::Vector2f sizeScale = newSize/getSize();

    for(auto& item : panel)
    {
        item.setSize(item.getSize()*sizeScale);
    }
    for(auto& item : mask)
    {
        item.setSize(item.getSize()*sizeScale);
    }
    shapeMask.setSize(shapeMask.getSize() * sizeScale);

    CVShape::setSize(newSize);
}

void CVBox::rotate(const float& angle, const unsigned char& flags, const sf::Vector2f& origin)
{
    sf::Vector2f objCenter = origin, itemCenter;
    if(isnan(origin.x) || isnan(origin.y)) objCenter = getPosition() + this->origin;

    float dist, oldAngle;

//    std::cout << "Object: " << objCenter.x << ' ' << objCenter.y << '\n';

    for(auto& item : panel)
    {
        item.rotate(angle);
        itemCenter = item.getPosition();

//        std::cout << "Item: " << itemCenter.x << ' ' << itemCenter.y << '\n';

        dist = getDistance(objCenter, itemCenter);
        if(dist != 0.0f)
        {
            oldAngle = get_angle(objCenter, itemCenter);
//            item.setPosition(radial_position(objCenter, dist, (oldAngle + angle)/PI));
        }
    }

    shapeMask.rotate(angle);

    for(auto& item : spriteList)
    {
        item.rotate(angle);
        itemCenter = item.getPosition();

        dist = getDistance(objCenter, itemCenter);
        if(dist != 0.0f)
        {
            oldAngle = get_angle(objCenter, itemCenter);
//            item.setPosition(radial_position(objCenter, dist, (oldAngle + angle)/PI));
        }
    }
}

void CVBox::setColor(const sf::Color& newColor)
{
    CVElement::setColor(newColor);
    for(auto& item : panel)
    {
        item.setFillColor(newColor);
        item.setOutlineColor(newColor);
    }
    shapeMask.setFillColor(newColor);
}

void CVBox::setSpriteColor(const sf::Color& newColor)
{
    for(auto& spr : spriteList)
    {
        spr.setColor(newColor);
    }

    colorTheme.resize(3, sf::Color::Transparent);
    colorTheme[2] = newColor;
}

void CVBox::setFillColor(const sf::Color& newColor)
{
    for(auto& item : panel)
    {
        item.setFillColor(newColor);
    }
}

void CVBox::setOutlineColor(const sf::Color& newColor)
{
    for(auto& item : panel)
    {
        item.setOutlineColor(newColor);
    }
}

void CVBox::setTexture(const sf::Texture* texture)
{
    panel.front().setTexture(texture);
}

void CVBox::setTexture(const std::string& texture)
{
    setTexture(appTexture(texture));
}

void CVBox::setRounding(const float& radius,
                        const unsigned int& pointCount,
                        const std::vector<bool>& states)
{
    if(!panel.empty())
    {
        panel.front().setRounding(radius, pointCount, states);
    }
    shapeMask.setRounding(radius, pointCount, states);
    inactiveMask.setRounding(radius, pointCount, states);

}

const float& CVBox::getRoundingRadius() const
{
    return panel.front().getRoundingRadius();
}

const unsigned int& CVBox::getRoundingPointCount() const
{
    return panel.front().getArcPointCount();
}

sf::Color CVBox::getFillColor() const
{
    return panel.front().getFillColor();
}

sf::Color CVBox::getColor() const
{
    return baseFillColor();
}

sf::Color CVBox::getOutlineColor() const
{
    return panel.front().getOutlineColor();
}

void CVBox::setOutlineThickness(const float& newThickness)
{
    for(auto& item : panel)
    {
        item.setOutlineThickness(newThickness);
    }
}

void CVBox::setRotation(const float& angle, const unsigned char& flags, const sf::Vector2f& origin)
{
    sf::Vector2f objCenter = origin, itemCenter;
    if(isnan(origin.x) || isnan(origin.y)) objCenter = getPosition() + this->origin;

    float dist;

    if(flags & CV_LAYER_SHAPES)
    {
        for(auto& item : panel)
        {
            item.setRotation(angle);
            itemCenter = item.getPosition();

            dist = getDistance(objCenter, itemCenter);
            if(dist != 0.0f)
            {
                item.setPosition(radial_position(objCenter, dist, angle/PI));
            }
        }
        shapeMask.setRotation(angle);
        inactiveMask.setRotation(angle);
    }

    if(flags & CV_LAYER_SPRITES)
    {
        for(auto& item : spriteList)
        {
            item.setRotation(angle);
            itemCenter = item.getPosition();

            dist = getDistance(objCenter, itemCenter);
            if(dist != 0.0f)
            {
                item.setPosition(radial_position(objCenter, dist, angle/PI));
            }
        }
    }

}

float CVBox::getRotation(const unsigned char& flags) const
{
    if(flags & CV_LAYER_SHAPES)
    {
        return panel.front().getRotation();
    }
    if((flags & CV_LAYER_SPRITES) && !spriteList.empty())
    {
        return spriteList.front().getRotation();
    }
    return 0.0f;
}

CVBox::CVBox(CVView* View,
             sf::Vector2f position, float width, float height,
             sf::Color fillColor,
             sf::Color borderColor,
             float borderWidth):
    CVShape(View),
    bNoFill(false)
{

    createdOrigin = position;

    colorTheme.emplace_back(fillColor);
    colorTheme.emplace_back(borderColor);

    highlightColor = fillColor;
    brighten(highlightColor, 50);

    bounds = sf::FloatRect(position.x, position.y, width, height);

    colorTheme = {fillColor, borderColor};

    // Main panel
    panel.emplace_back(sf::Vector2f(width, height));
    panel.back().setPosition(position);
    panel.back().setFillColor(fillColor);
    panel.back().setOutlineThickness(borderWidth);
    panel.back().setOutlineColor(borderColor);

    shapeMask.setSize(sf::Vector2f(width, height));
    shapeMask.setPosition(position);
    shapeMask.setFillColor(sf::Color(255,255,255,0));

    inactiveMask.setSize(sf::Vector2f(width, height));
    inactiveMask.setPosition(position);
    inactiveMask.setFillColor(inactiveColor());
}

void CVBox::setClearFill(const bool& state)
{
    bNoFill = state;
    if(bNoFill)
    {
        panel.front().setFillColor(sf::Color::Transparent);
    }
    else
    {
        panel.front().setFillColor(baseFillColor());
    }
}

void CVBox::updateBounds()
{
    CVElement::updateBounds();
    if(!bSpriteOnly)
    {
        sf::FloatRect panelBounds = panel.front().getGlobalBounds();
        if(panelBounds.left < bounds.left) bounds.left = panelBounds.left;
        if(panelBounds.top < bounds.top) bounds.top = panelBounds.top;
        if(panelBounds.left + panelBounds.width > bounds.left + bounds.width)
            bounds.width = panelBounds.left + panelBounds.width - bounds.left;
        if(panelBounds.top + panelBounds.height > bounds.top + bounds.height)
            bounds.height = panelBounds.top + panelBounds.height - bounds.top;
        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
        panel.front().setPosition(sf::Vector2f(bounds.left, bounds.top));
    }
}

void CVBox::setExpand(const bool& state)
{
    bExpand = state;
    if(bExpand) updateBounds();
}

bool CVBox::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVShape::update(event, mousePos)) return false;
    if(bFade && !bNoFill && !bSpriteOnly)
    {
        sf::Color tmp;
        uint8_t adjusted_fr = ceil((float)fadeRate*120.0f/View->getFrameRate());

        for(auto& item : panel)
        {
            tmp = item.getFillColor();
            if(targetAlpha > tmp.a)
            {
                if(tmp.a + adjusted_fr < targetAlpha)
                {
                    tmp.a += adjusted_fr;
                }
                else tmp.a = targetAlpha;
            }
            else
            {
                if(tmp.a - adjusted_fr > targetAlpha)
                {
                    tmp.a -= adjusted_fr;
                }
                else
                {
                    tmp.a = targetAlpha;
                }
            }
            item.setFillColor(tmp);

            tmp = item.getOutlineColor();
            if(targetAlpha > tmp.a)
            {
                if(tmp.a + adjusted_fr < targetAlpha)
                {
                    tmp.a += adjusted_fr;
                }
                else tmp.a = targetAlpha;
            }
            else
            {
                if(tmp.a - adjusted_fr > targetAlpha)
                {
                    tmp.a -= adjusted_fr;
                }
                else
                {
                    tmp.a = targetAlpha;
                }
            }
            item.setOutlineColor(tmp);
        }

    }
    if(highlightable() && active)
    {
        if((event.focusFree() && bounds.contains(event.lastFrameMousePosition)) || highlighted)
        {
            sf::Color tmp;
            for(auto& item : panel)
            {
                tmp = highlightColor;
                tmp.a = baseFillColor().a;
                item.setFillColor(tmp);
            }
        }
        else
        {
            for(auto& item : panel)
            {
                item.setFillColor(baseFillColor());
            }
        }
    }

    return true;
}

}
