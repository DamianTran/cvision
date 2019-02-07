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

#include "cvision/import.hpp"

namespace cvis
{

bool FontManager::addFont(std::string fileDir, std::string tag)
{
    fonts.push_back(sf::Font());
    if(!fonts.back().loadFromFile(fileDir))
    {
        fonts.pop_back();
        return false;
    }
    fontTags.push_back(tag);
    return true;
}

bool FontManager::addFont(const void* binaries, const size_t& size, const std::string& tag)
{
    fonts.emplace_back();
    if(!fonts.back().loadFromMemory(binaries, size))
    {
        fonts.pop_back();
        return false;
    }
    fontTags.push_back(tag);
    return true;
}

const sf::Texture* ImageManager::taggedTexture(const std::string& s) const
{
    if(s.size() < 1) return nullptr;
    unsigned int index = 0;
    for(auto& tag : textureTags)
    {
        if(tag == s) return &textures[index];
        ++index;
    }
    return nullptr;
}
const sf::Image* ImageManager::taggedImage(const std::string& s) const
{
    if(s.size() < 1) return nullptr;
    unsigned int index = 0;
    for(auto& tag : imageTags)
    {
        if(tag == s) return &images[index];
        ++index;
    }
    return nullptr;
}

std::string ImageManager::getTextureName(const sf::Texture* texture) const
{
    for(size_t i = 0; i < numTextures; ++i)
    {
        if(&textures[i] == texture) return textureTags[i];
    }
    return std::string();
}

bool ImageManager::addImage(std::string fileName, std::string tag)
{
    images.emplace_back();
    if(!images.back().loadFromFile(fileName))
    {
        images.pop_back();
        return false;
    }
    imageTags.push_back(tag);
    ++numImages;
    return true;
}

bool ImageManager::addTexture(std::string fileName, std::string tag)
{
    textures.emplace_back();
    if(!textures.back().loadFromFile(fileName))
    {
        textures.pop_back();
        return false;
    }
    textures.back().setSmooth(true);
    textures.back().generateMipmap();
    textureTags.push_back(tag);
    ++numTextures;
    return true;
}

bool ImageManager::addTexture(const void* binaries, const size_t& size,
                              const std::string& tag)
{
    textures.emplace_back();
    if(!textures.back().loadFromMemory(binaries, size))
    {
        textures.pop_back();
        return false;
    }
    textures.back().setSmooth(true);
    textures.back().generateMipmap();
    textureTags.push_back(tag);
    ++numTextures;
    return true;
}

}
