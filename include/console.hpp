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

#pragma once

#ifndef CVIS_CONSOLE
#define CVIS_CONSOLE

#include "cvision/cvis_base.hpp"
#include "cvision/cvis_typeIO.hpp"
#include "algorithm.hpp"

namespace cvis{

class CVConsoleLog: public CVTextLog{
protected:

    std::stringstream iobuffer;

public:

    template<typename T> friend CVConsoleLog& operator<<(CVConsoleLog& log, const T& input){
        using namespace AIALG;
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

        if(edit) log.editLastEntry(bufstr, false);
        else log.addToLastEntry(bufstr, false);

        log.iobuffer.str("");
        return log;
    }

    CVConsoleLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float& borderWidth, const uint8_t& animType = CV_OBJ_ANIM_NONE,
              const std::string& logFile = "", CVTypeBox* usrEntryBox = nullptr);
};

}

#endif // CVIS_CONSOLE
