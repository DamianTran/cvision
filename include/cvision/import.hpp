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

#ifndef CVIS_IMPORT
#define CVIS_IMPORT

#include <vector>
#include <string>

#include "cvision/lib.hpp"

#include <SFML/Graphics.hpp>

namespace cvis
{

class CVISION_API FontManager
{
public:
    std::vector<sf::Font> fonts;
    std::vector<std::string> fontTags;

    inline const sf::Font* operator[](const unsigned int& i) const
    {
        if(i >= fonts.size()) throw std::out_of_range("Font Manager: Index out of range of available fonts");
        return &fonts[i];
    }
    inline const sf::Font* operator[](const std::string& s) const
    {
        if(s.empty()) return nullptr;
        unsigned int index = 0;
        for(auto& tag : fontTags)
        {
            if(tag == s) return &fonts[index];
            ++index;
        }
        throw std::invalid_argument("Font Manager: Font tag does not correspond to a font in Font Manager");
    }

    CVISION_API bool addFont(std::string fileDir, std::string tag);
    CVISION_API bool addFont(const void* binaries, const size_t& size,
                 const std::string& tag);

    FontManager() { }

};

typedef std::vector<sf::Texture> TextureList;
typedef std::vector<sf::Image> ImageList;

class CVISION_API ImageManager
{
protected:
    TextureList textures;
    std::vector<std::string> textureTags;
    unsigned int numTextures;

    ImageList images;
    std::vector<std::string> imageTags;
    unsigned int numImages;

public:
    inline const sf::Texture* indexTexture(const unsigned int i) const
    {
        return &textures[i];
    }
    inline const sf::Image* indexImage(const unsigned int i) const
    {
        return &images[i];
    }

    CVISION_API const sf::Texture* taggedTexture(const std::string& s) const;
    CVISION_API const sf::Image* taggedImage(const std::string& s) const;

    CVISION_API std::string getTextureName(const sf::Texture* texture) const;

    CVISION_API bool addImage(std::string fileName, std::string tag);
    CVISION_API bool addTexture(std::string fileName, std::string tag);

    CVISION_API bool addTexture(const void* binaries, const size_t& size,
                    const std::string& tag);

    ImageManager():
        numTextures(0),
        numImages(0) { }
    ~ImageManager() { }

};

}

#endif // CVIS_IMPORT
