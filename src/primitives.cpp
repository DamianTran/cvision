//
// CVision: a multi-platform graphics interface libary for C++
//
//////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2018 Damian Tran
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
// Redistribution of CVision is permitted under accordance with
// the GNU general public licence (GPL) version 3.0 and the
// terms and conditions specified in CVlicence.txt

#include "cvision/primitives.hpp"
#include "cvision/cvis_algorithm.hpp"

namespace cvis{

rounded_rectangle::rounded_rectangle(const sf::Vector2f& size,
                                     const float& rounding_radius,
                                     const sf::Color& fill_color,
                                     const unsigned int& arc_point_count):
                                         rounded_corner(4,
                                                        ((rounding_radius != 0.0f) &&
                                                         (arc_point_count != 0))){

    trace(size, rounding_radius, arc_point_count);
    setFillColor(fill_color);
}

rounded_rectangle::rounded_rectangle():
    pointCount(4),
    arcPointCount(10),
    points(4, sf::Vector2f(0,0)),
    radius(0.0f),
    rounded_corner(4, false){

}

void rounded_rectangle::trace(const sf::Vector2f& size,
               const float& radius,
               const unsigned int& arc_point_count){

    points.clear();

    this->radius = radius;

    if(!radius || (arc_point_count < 2)){
        arcPointCount = 1;
        pointCount = 4;
        points.emplace_back(0.0f,0.0f);
        points.emplace_back(size.x, 0.0f);
        points.emplace_back(size.x, size.y);
        points.emplace_back(0.0f, size.y);
    }
    else{

        arcPointCount = arc_point_count;
        pointCount = 0;
        for(auto state : rounded_corner){
            if(state) pointCount += arc_point_count;
            else ++pointCount;
        }

        float adj_radius = radius;
        if((size.x < size.y) && (adj_radius > size.x/2)){
            adj_radius = size.x/2;
        }
        else if(adj_radius > size.y/2){
            adj_radius = size.y/2;
        }

        double i = 0.0, apc = arc_point_count;

        if(topLeftRounded()){
            for(; i < apc; ++i){
                points.emplace_back(radial_position(sf::Vector2f(adj_radius, adj_radius),
                                                    adj_radius, PI + PI/2*i/(arc_point_count-1)));
            }
        }
        else{
            points.emplace_back(sf::Vector2f(0.0f,0.0f));
        }

        if(topRightRounded()){
            for(i = 0.0; i < apc; ++i){
                points.emplace_back(radial_position(sf::Vector2f(size.x - adj_radius, adj_radius),
                                                    adj_radius, PI/2*i/(arc_point_count-1) - PI/2));
            }
        }
        else{
            points.emplace_back(sf::Vector2f(size.x,0.0f));
        }

        if(bottomRightRounded()){
            for(i = 0.0; i < apc; ++i){
                points.emplace_back(radial_position(sf::Vector2f(size.x - adj_radius, size.y - adj_radius),
                                                    adj_radius, PI/2*i/(arc_point_count-1)));
            }
        }
        else{
            points.emplace_back(sf::Vector2f(size.x,size.y));
        }

        if(bottomLeftRounded()){
            for(i = 0.0; i < apc; ++i){
                points.emplace_back(radial_position(sf::Vector2f(adj_radius, size.y - adj_radius),
                                                    adj_radius, PI/2*i/(arc_point_count-1) - PI*3/2));
            }
        }
        else{
            points.emplace_back(sf::Vector2f(0.0f,size.y));
        }
    }

    update();

}

size_t rounded_rectangle::getPointCount() const{
    return pointCount;
}

bool rounded_rectangle::cornerIsRounded(const unsigned int& corner_idx) const{
    if(corner_idx > 3) throw std::out_of_range("Index larger than possible corners");
    return rounded_corner[corner_idx];
}

bool rounded_rectangle::topLeftRounded() const{
    return rounded_corner[0];
}
bool rounded_rectangle::topRightRounded() const{
    return rounded_corner[1];
}
bool rounded_rectangle::bottomRightRounded() const{
    return rounded_corner[2];
}
bool rounded_rectangle::bottomLeftRounded() const{
    return rounded_corner[3];
}

void rounded_rectangle::setCornerRounding(const unsigned int& corner_idx,
                                          const bool& state){
    if(corner_idx > 3) throw std::out_of_range("Index larger than possible corners");
    rounded_corner[corner_idx] = state;
}

void rounded_rectangle::setTopLeftRounded(const bool& state){
    rounded_corner[0] = state;
    trace(getSize(), radius, arcPointCount);
}
void rounded_rectangle::setTopRightRounded(const bool& state){
    rounded_corner[1] = state;
    trace(getSize(), radius, arcPointCount);
}
void rounded_rectangle::setBottomRightRounded(const bool& state){
    rounded_corner[2] = state;
    trace(getSize(), radius, arcPointCount);
}
void rounded_rectangle::setBottomLeftRounded(const bool& state){
    rounded_corner[3] = state;
    trace(getSize(), radius, arcPointCount);
}

void rounded_rectangle::setAllRoundState(const bool& state){
    for(size_t i = 0; i < rounded_corner.size(); ++i){
        rounded_corner[i] = state;
    }
    trace(getSize(), radius, arcPointCount);
}

void rounded_rectangle::setRounding(const float& newRadius, const unsigned int& newPC,
                                    const std::vector<bool>& states){
    rounded_corner = states;
    trace(getSize(), newRadius, newPC);
}

sf::Vector2f rounded_rectangle::getPoint(size_t index) const{
    return points[index];
}

sf::Vector2f rounded_rectangle::getSize() const{
    return sf::Vector2f(getLocalBounds().width, getLocalBounds().height);
}

void rounded_rectangle::setSize(const sf::Vector2f& newSize){
    trace(newSize, radius, arcPointCount);
}

}
