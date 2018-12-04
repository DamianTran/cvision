#pragma once

#ifndef CVIS_PRIMITIVE
#define CVIS_PRIMITIVE

#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

namespace cvis{

class rounded_rectangle : public sf::Shape{
private:

    unsigned int pointCount, arcPointCount;
    std::vector<sf::Vector2f> points;
    float radius;

    std::vector<bool> rounded_corner;

    void trace(const sf::Vector2f& size,
               const float& radius,
               const unsigned int& apc);

public:

    virtual size_t getPointCount() const;
    virtual sf::Vector2f getPoint(size_t index) const;

    void setRounding(const float& newRadius,
                     const unsigned int& newPointCount = 10,
                     const std::vector<bool>& states =
                     std::vector<bool>(4, true));

    void setSize(const sf::Vector2f& newSize);
    template<typename T>
    void setSize(const T& x, const T& y){
        setSize(sf::Vector2f(x, y));
    }
    sf::Vector2f getSize() const;

    inline const float& getRoundingRadius() const{
        return radius;
    }
    inline const unsigned int& getArcPointCount() const{
        return arcPointCount;
    }

    bool cornerIsRounded(const unsigned int& corner_idx) const;

    bool topLeftRounded() const;
    bool topRightRounded() const;
    bool bottomRightRounded() const;
    bool bottomLeftRounded() const;

    void setCornerRounding(const unsigned int& corner_idx,
                           const bool& state = true);

    void setTopLeftRounded(const bool& state = true);
    void setTopRightRounded(const bool& state = true);
    void setBottomRightRounded(const bool& state = true);
    void setBottomLeftRounded(const bool& state = true);

    void setAllRoundState(const bool& state);

    rounded_rectangle(const sf::Vector2f& size,
                      const float& rounding_radius = 0.0f,
                      const sf::Color& fill_color = sf::Color::White,
                      const unsigned int& arc_point_count = 10);
    rounded_rectangle();
};

}
#endif // CVIS_PRIMITIVE
