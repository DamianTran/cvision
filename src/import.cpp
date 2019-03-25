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

#include "cvision/import.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace cvis
{

bool FontManager::addFont(const string& fileDir, const string& tag)
{
    try
    {
        fonts.at(tag);
        cout << "Warning: request to add redundant font tag \"" << tag << "\" was ignored\n";
        return false;
    }catch(...)
    {
        fonts.emplace(tag, new sf::Font());
        fonts.at(tag)->loadFromFile(fileDir);
    }
}

bool FontManager::addFont(const void* binaries, const size_t& size, const string& tag)
{
    try
    {
        fonts.at(tag);
        cout << "Warning: request to add redundant font tag \"" << tag << "\" was ignored\n";
        return false;
    }catch(...)
    {
        fonts.emplace(tag, new sf::Font());
        fonts.at(tag)->loadFromMemory(binaries, size);
    }
}

FontManager::~FontManager()
{
    for(auto& pair : fonts)
    {
        delete(pair.second);
    }
}

const sf::Texture* ImageManager::taggedTexture(const string& s) const
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
const sf::Image* ImageManager::taggedImage(const string& s) const
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

string ImageManager::getTextureName(const sf::Texture* texture) const
{
    for(size_t i = 0; i < numTextures; ++i)
    {
        if(&textures[i] == texture) return textureTags[i];
    }
    return string();
}

bool ImageManager::addImage(string fileName, string tag)
{
    images.emplace_back();
    if(!images.back().loadFromFile(fileName))
    {
        images.pop_back();
        cout << "Failed to load image: " << tag << '\n';
        return false;
    }
    imageTags.push_back(tag);
    ++numImages;
    return true;
}

bool ImageManager::addTexture(string fileName, string tag)
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
                              const string& tag)
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
