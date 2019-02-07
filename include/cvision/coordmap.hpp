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

#pragma once

#ifndef CVIS_COORD_MAP
#define CVIS_COORD_MAP

#include <string>
#include <vector>
#include <utility>

#include <SFML/System/Vector2.hpp>

namespace cvis{

class CVISION_API coord_map : public std::vector<std::pair<sf::Vector2i, std::string>>
{
public:

    inline std::pair<sf::Vector2i, std::string>& operator[](const size_t& index)
    {
        return std::vector<std::pair<sf::Vector2i, std::string>>::operator[](index);
    }
    inline const std::pair<sf::Vector2i, std::string>& operator[](const size_t& index) const
    {
        return std::vector<std::pair<sf::Vector2i, std::string>>::operator[](index);
    }

    inline std::string& operator[](const sf::Vector2i& index)
    {

        for(auto& pair : *this)
        {
            if((pair.first.x == index.x) &&
               (pair.first.y == index.y))
            {
                return pair.second;
            }
        }

        emplace_back(index, "");
        return back().second;

    }

    inline const std::string& operator[](const sf::Vector2i& index) const
    {

        for(auto& pair : *this)
        {
            if((pair.first.x == index.x) &&
               (pair.first.y == index.y))
            {
                return pair.second;
            }
        }

        throw std::out_of_range("Index out of range of coordinate map");

    }

    inline const std::string& at(const sf::Vector2i& index) const
    {
        return (*this)[index];
    }

    inline void erase(const sf::Vector2i& index)
    {
        for(size_t i = 0; i < size();)
        {
            if((*this)[i].first == index)
            {
                std::vector<std::pair<sf::Vector2i, std::string>>::erase(begin() + i);
            }
            else ++i;
        }
    }

    coord_map(){ }

};

}

#endif // CVIS_COORD_MAP
