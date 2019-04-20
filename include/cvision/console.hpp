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

#ifndef CVIS_CONSOLE
#define CVIS_CONSOLE

#include "cvision/type.hpp"
#include "cvision/algorithm.hpp"

namespace cvis{

class CVISION_API CVConsoleLog: public CVTextLog{
protected:

    std::stringstream iobuffer;

public:

    template<typename T> friend CVConsoleLog& operator<<(CVConsoleLog& log, const T& input){

        using EZC::operator<<;

        log.iobuffer << input;
        std::string bufstr = log.iobuffer.str(),
                    output;

        bool edit = false;

        for(size_t i = 0; i < bufstr.size();){
            if(bufstr[i] == '\n'){
                output.assign(bufstr.begin(), bufstr.begin() + i);
                if(edit){
                    log.editLastEntry(bufstr, false);
                    edit = false;
                }
                else{
                    log.addLogEntry(output, 0.0f);
                }
                bufstr.erase(bufstr.begin(), bufstr.begin() + i + 1);
            }
            else if(bufstr[i] == '\r'){
                log.editLastEntry("", false);
                bufstr.erase(bufstr.begin(), bufstr.begin() + i + 1);
                edit = true;
            }
            else ++i;
        }

        if(edit)
        {
            log.editLastEntry(bufstr, false);
        }
        else
        {
            log.addToLastEntry(bufstr, false);
        }

        log.iobuffer.str("");

        return log;
    }

    CVISION_API CVConsoleLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
              const TextEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float& borderWidth, const uint8_t& animType = CV_OBJ_ANIM_NONE,
              const std::string& logFile = "", CVTypeBox* usrEntryBox = nullptr);
};

}

#endif // CVIS_CONSOLE
