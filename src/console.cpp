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

#include "cvision/console.hpp"

using namespace AIALG;

namespace cvis{

CVConsoleLog::CVConsoleLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float& borderWidth, const uint8_t& animType,
              const std::string& logFile, CVTypeBox* usrEntryBox):
                  CVTextLog(View, position, width, height, textInfo, fillColor, borderColor, borderWidth,
                            animType, logFile, usrEntryBox){

    setTextPanelPadding(3.0f);
    initialMessageCount = 150;
    messageLoadInc = 50;

    disableWidthTextWrap();

    setIncomingMsgPanelFillColor(sf::Color::Transparent);
    setMsgPanelFillColor(sf::Color::Transparent);
    setUserTextAlign(ALIGN_LEFT);
    setHighlightPanelFillColor(sf::Color::Transparent);
}

}
