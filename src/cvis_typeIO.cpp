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

#include "cvision/cvis_typeIO.hpp"

using namespace AIALG;

namespace cvis{

CVTypeBox::CVTypeBox(CVView* View, const sf::Vector2f& position, const float width, const float height,
      const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
      const float borderWidth, const uint8_t& animType, const uint8_t& textFitType,
      const uint8_t& expandType):
          CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
          bkgString(textInfo.text),
          timeLastBlink(0.0f),
          blinkFreq(1.0f),
          expandRate(0.2f),
          bBlink(false),
          bTypeStringChanged(false),
          bAcceptSuggestions(true),
          bCanEdit(true),
          expandType(expandType),
          initBounds(bounds),
          animType(animType),
          textFitType(textFitType),
          cursorPos(0),
          typeFrameStart(0),
          typeFrameEnd(0),
          logTextIndex(0),
          selectionStart(UINT_MAX),
          selectionLineStart(0),
          selectionLineEnd(0),
          enterLine(false),
          textLog(nullptr)
          {

    this->textInfo = textInfo;
    addTextEntry(textInfo);
    setTextTemplate(textInfo);

    sf::FloatRect textBounds;

    textBounds = displayText.back().getGlobalBounds();

    xPadding = (bounds.height - textBounds.height)/2;
    if(xPadding < 0.0f) xPadding = 0.0f;

    displayText.back().setPosition(bounds.left + xPadding, bounds.top + bounds.height/2 - textBounds.height);

    if(textInfo.text.find("\n") > textInfo.text.size()){
        if(textBounds.height > bounds.height) bounds.height = textBounds.height;
    }

    float cursorOrigin = 0.0f;

    switch(textInfo.alignment){
        case ALIGN_RIGHT:{
            displayText.back().move(bounds.width - xPadding - textBounds.width - borderWidth, 0.0f);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_CENTER:{
            displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2, displayText.back().getPosition().y);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2,
                                           displayText.back().getPosition().y - getTextCenterOffsetY(displayText.back())/2);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_CENTER_BOTTOM:{
            displayText.back().setPosition(bounds.left + bounds.width/2 - textBounds.width/2,
                                           displayText.back().getPosition().y);
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            break;
        }
        case ALIGN_VERTICAL_INVERTED:{

            panel.front().setOrigin(bounds.width/2, bounds.height/2);
            panel.front().setRotation(-90);
            panel.front().setPosition(position);
            bounds = panel.front().getGlobalBounds();
            setOrigin(sf::Vector2f(bounds.width, bounds.height/2));

            displayText.back().setOrigin(textBounds.width/2, textBounds.height/2);
            displayText.back().setPosition(panel.front().getPosition());
            cursorOrigin = textBounds.height-getTextCenterOffsetY(displayText.back());
            displayText.back().setRotation(-90);

            break;
        }
        case ALIGN_LEFT_MIDLINE:{
            displayText.back().setPosition(displayText.back().getPosition().x,
                                           displayText.back().getPosition().y - textBounds.height/2);
            cursorOrigin = textBounds.height/2-getTextCenterOffsetY(displayText.back());
            break;
        }
        default:{ // Left-aligned
            break;
        }
    }

    textBounds = displayText.back().getGlobalBounds();
    cursor.setSize(sf::Vector2f(2.0f,
                displayText.back().getFont()->getGlyph(65, textInfo.fontSize, false, 0).bounds.height*1.2f +
                                xPadding/2));

    if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
        cursor.setOrigin(0.0f, cursorOrigin);
        cursor.setRotation(-90);
    }
    else if(textInfo.alignment == ALIGN_VERTICAL){
        cursor.setOrigin(0.0f, cursorOrigin);
        cursor.setRotation(90);
    }
    else{
        cursor.setOrigin(0.0f, cursorOrigin);
    }

    cursor.setPosition(sf::Vector2f(displayText.back().getPosition().x,
                                    displayText.back().getPosition().y));

    if((borderColor != fillColor) && (borderColor != sf::Color::Transparent)) cursor.setFillColor(borderColor);
    else cursor.setFillColor(textInfo.textColor);

}

void CVTypeBox::setTypeString(std::string newString){
    if(textFitType == CV_TEXT_FIT_WRAP){
        displayText.back().setString(newString);
        sf::FloatRect textBounds = displayText.back().getGlobalBounds();
        if(textBounds.width > bounds.width){
            for(size_t i = 0; i < newString.size(); ++i){ // Add new line characters to fit text horizontally
                if(displayText.back().findCharacterPos(i).x - bounds.left > bounds.width){
                    for(int j = i; j > 0; --j){
                        if(newString[j-1] == ' '){
                            newString[j-1] = '\n';
                            break;
                        }
                        else if(isCharType(newString[j-1], ",\\/:._-")){
                            newString.insert(newString.begin() + j, '\n');
                            break;
                        }
                        else if(displayText.back().findCharacterPos(i).x -
                                displayText.back().findCharacterPos(j).x > bounds.width){
                            newString.insert(newString.begin() + i - 1, '\n');
                            break;
                        }
                    }

                    displayText.back().setString(newString);
                    textBounds = displayText.back().getGlobalBounds();
                    if(textBounds.width < bounds.width) break;
                }
            }
        }

        if((textInfo.alignment == ALIGN_LEFT) ||
           (textInfo.alignment == ALIGN_LEFT_MIDLINE) ||
           (textInfo.alignment == ALIGN_LEFT_BOTTOM))
            displayText.back().move(sf::Vector2f(bounds.left + textPadding -
                                                 displayText.back().getPosition().x,
                                    0.0f));
    }
    else if(textFitType == CV_TEXT_FIT_LATERAL){
        for(size_t i = 0; i < newString.size();){
            if(!isTypeChar(newString[i])) newString.erase(newString.begin() + i);
            else if(newString[i] == '\n'){
                bool nextChar = true;
                while((i < newString.size() - 1) && (newString[i+1] == '\n')){
                    newString.erase(newString[i+1]);
                    nextChar = false;
                }
                newString[i] = ' ';
                if(nextChar) ++i;
            }
            else ++i;
        }
    }
    else{
        for(size_t i = 0; i < newString.size();){
            if(!isTypeChar(newString[i])) newString.erase(newString.begin() + i);
            else ++i;
        }
        displayText.back().setString(newString);
    }

    this->typeString = newString;
    selectionStart = UINT_MAX;
    typeFrameStart = 0;
    typeFrameEnd = newString.size();
    bTypeStringChanged = true;
    if(newString.size() > 0) cursorPos = newString.size()-1;
}

void CVTypeBox::make_suggestion(const std::string& text){
    suggested = text;
}

void CVTypeBox::clear_suggestion(){
    suggested.clear();
}

const bool& CVTypeBox::can_suggest() const{
    return bAcceptSuggestions;
}

std::string CVTypeBox::string_at_cursor() const{
    std::string output;
    if(typeString.empty() || (cursorPos > typeString.size())) return output;

    int i = cursorPos - 1;
    while((i >= 0) && !isCharType(typeString[i], DELIM_BASIC)){
        output.insert(output.begin(), typeString[i]);
        --i;
    }
    i = cursorPos;
    while((i < typeString.size()) && !isCharType(typeString[i], DELIM_BASIC)){
        output += typeString[i];
        ++i;
    }
    return output;
}

bool CVTypeBox::cursor_at_word_end() const{
    if(typeString.empty() ||
       (cursorPos > typeString.size())
       || (cursorPos == 0)) return false;
    if(!isCharType(typeString[cursorPos-1], DELIM_BASIC)){
        if((cursorPos == typeString.size()) ||
           isCharType(typeString[cursorPos], DELIM_BASIC)) return true;
    }
    return false;
}

bool CVTypeBox::cursor_at_word_begin() const{
    if(typeString.empty() ||
       (cursorPos > typeString.size())) return false;
    if(!isCharType(typeString[cursorPos], DELIM_BASIC)){
        if((cursorPos == 0) || isCharType(typeString[cursorPos - 1], DELIM_BASIC)) return true;
    }
    return false;
}

unsigned int CVTypeBox::cursor_string_position() const{
    int i = cursorPos;
    while((i > 0) && !isCharType(typeString[i-1], DELIM_BASIC)){
        --i;
    }
    return cursorPos - i;
}

bool CVTypeBox::update(CVEvent& event, const sf::Vector2f& mousePos){

    bool slideCursor = true,
        cursorChanged = false,
        mouseCaptured = false;
    unsigned int rangeBegin = 0, rangeEnd = displayText.back().getString().getSize();

    if(event.LMBhold && bounds.contains(event.LMBpressPosition)){ // Handle click and drag
        if((bounds.contains(mousePos) || displayText.back().getGlobalBounds().contains(mousePos)) && event.captureFocus() && event.captureMouse()){
            if(!hasFocus()){
                timeLastBlink = 0.0f;
                bBlink = false;
            }
            mouseCaptured = true;
            setFocus(true);
            size_t i = 0, S = displayText.back().getString().getSize();
            float charDist = 0.0f;
            bool cursorFound = false,
                selectionFound = false;

            if((event.LMBholdTime < 0.2f) || (mousePos != event.lastFrameMousePosition)){
                if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                    if(textFitType != CV_TEXT_FIT_LATERAL){
                        for(size_t j = 0; i < S; i += 100){
                            if(!selectionFound){
                                if(!cursorFound){
                                    if(event.LMBpressPosition.x < displayText.back().findCharacterPos(i).x){
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        selectionFound = true;
                                    }
                                }
                                else{
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(event.LMBpressPosition.x < displayText.back().findCharacterPos(j).x){
                                        rangeEnd = j;
                                        selectionFound = true;
                                    }
                                }
                            }
                            if(!cursorFound){
                                if(!selectionFound){
                                    if(mousePos.x < displayText.back().findCharacterPos(i).x){
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        cursorFound = true;
                                    }
                                }
                                else{
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(mousePos.x < displayText.back().findCharacterPos(j).x){
                                        rangeEnd = j;
                                        cursorFound = true;
                                    }
                                }
                            }

                            if(selectionFound && cursorFound) break;
                        }

                        if(!selectionFound || !cursorFound) rangeBegin = rangeEnd > 200 ? rangeEnd - 200 : 0;

                        cursorFound = false;
                        selectionFound = false;
                    }

                    for(i = rangeBegin; i < rangeEnd; ++i){
                        if(i == typeString.size() - 1){
                            charDist = displayText.back().getPosition().y -
                                    displayText.back().findCharacterPos(i).y;
                        }
                        else charDist = displayText.back().findCharacterPos(i).y - displayText.back().findCharacterPos(i+1).y;

                        if((event.LMBholdTime < 0.2f) && !selectionFound){
                            if(event.LMBpressPosition.y > displayText.back().findCharacterPos(i).y - charDist/2){
                                selectionStart = i + typeFrameStart;
                                selectionFound = true;
                            }
                            else if(i == typeString.size() - 1){
                                selectionStart = i + typeFrameStart + 1;
                            }
                        }
                        if(!cursorFound){
                            if(mousePos.y > displayText.back().findCharacterPos(i).y - charDist/2){
                                cursorPos = i + typeFrameStart;
                                cursorFound = true;
                            }
                            else if(i == typeString.size() - 1){
                                cursorPos = i + typeFrameStart + 1;
                            }
                        }

                        if(cursorFound && (selectionStart != UINT_MAX)) break;
                    }
                }
                else{
                    if(textFitType != CV_TEXT_FIT_LATERAL){
                        for(size_t j = 0; i < S; i += 100){
                            if(!selectionFound){
                                if(!cursorFound){
                                    if(event.LMBpressPosition.y < displayText.back().findCharacterPos(i).y){
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        selectionFound = true;
                                    }
                                }
                                else{
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(event.LMBpressPosition.y < displayText.back().findCharacterPos(j).y){
                                        rangeEnd = j;
                                        selectionFound = true;
                                    }
                                }
                            }
                            if(!cursorFound){
                                if(!selectionFound){
                                    if(mousePos.y < displayText.back().findCharacterPos(i).y){
                                        rangeBegin = i > 200 ? i - 200 : 0;
                                        cursorFound = true;
                                    }
                                }
                                else{
                                    j = i + 100 < S ? i + 100 : S-1;
                                    if(mousePos.y < displayText.back().findCharacterPos(j).y){
                                        rangeEnd = j;
                                        cursorFound = true;
                                    }
                                }
                            }

                            if(selectionFound && cursorFound) break;
                        }

                        if(!selectionFound || !cursorFound) rangeBegin = rangeEnd > 200 ? rangeEnd - 200 : 0;

                        cursorFound = false;
                        selectionFound = false;
                    }

                    for(i = rangeBegin; i < rangeEnd; ++i){
                        if(i == typeString.size() - 1){
                            charDist = displayText.back().getGlobalBounds().width -
                            displayText.back().findCharacterPos(i).x
                            + displayText.back().getPosition().x;
                        }
                        else charDist = displayText.back().findCharacterPos(i+1).x - displayText.back().findCharacterPos(i).x;

                        if((event.LMBholdTime < 0.2f) && !selectionFound){
                            if((event.LMBpressPosition.x < displayText.back().findCharacterPos(i).x + charDist/2)
                               && ((textFitType != CV_TEXT_FIT_WRAP) ||
                                   (event.LMBpressPosition.y < displayText.back().findCharacterPos(i).y +
                                    2*getTextCenterOffsetY(displayText.back())))){
                                selectionStart = i + typeFrameStart;
                                selectionFound = true;
                            }
                            else if(i == typeString.size() - 1){
                                selectionStart = i + typeFrameStart + 1;
                            }
                        }

                        if(!cursorFound){
                            if((mousePos.x < displayText.back().findCharacterPos(i).x + charDist/2)
                               && ((textFitType != CV_TEXT_FIT_WRAP) ||
                                   (mousePos.y < displayText.back().findCharacterPos(i).y +
                                    2*getTextCenterOffsetY(displayText.back())))){
                                cursorPos = i + typeFrameStart;
                                cursorFound = true;
                            }
                            else if(i == typeString.size() - 1){
                                cursorPos = i + typeFrameStart + 1;
                            }
                        }

                        if(cursorFound && (selectionStart != UINT_MAX)) break;
                    }
                }

            slideCursor = false;
            cursorChanged = true;
            }
        }
        else if(bounds.contains(event.LMBpressPosition)){ // Dragging off the boundaries
            if(textFitType == CV_TEXT_FIT_LATERAL){
                if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                    if(mousePos.y < bounds.top){
                        if(cursorPos < typeFrameEnd) ++cursorPos; // Shift right
                        else if(typeFrameEnd < typeString.size()){
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                    else{
                        if(cursorPos > typeFrameStart) --cursorPos; // Shift left
                        else if(typeFrameStart > 0){
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                }
                else if(textInfo.alignment == ALIGN_VERTICAL){
                    if(mousePos.y < bounds.top){
                        if(cursorPos > typeFrameStart) --cursorPos;
                        else if(typeFrameStart > 0){
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                    else{
                        if(cursorPos < typeFrameEnd) ++cursorPos;
                        else if(typeFrameEnd < typeString.size()){
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                }
                else{
                    if(mousePos.x < bounds.left){
                        if(cursorPos > typeFrameStart) --cursorPos;
                        else if(typeFrameStart > 0){
                            --cursorPos;
                            --typeFrameStart;
                            --typeFrameEnd;
                        }
                    }
                    else{
                        if(cursorPos < typeFrameEnd) ++cursorPos;
                        else if(typeFrameEnd < typeString.size()){
                            ++cursorPos;
                            ++typeFrameEnd;
                            ++typeFrameStart;
                        }
                    }
                }
            }
        }
        else{
            setFocus(false);
            selectionStart = UINT_MAX;
        }
    }
    else if(event.LMBhold || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        setFocus(false);
        selectionStart = UINT_MAX;
    }

    if(hasFocus()){
        if(bAcceptSuggestions &&
           (selectionStart == UINT_MAX) &&
           bCanEdit && cursor_at_word_end() && (suggested.size() > 0)){ // Handle auto-fill
            if(cursorPos > typeString.size()) cursorPos = typeString.size();
            unsigned int indexBegin = findString(suggested, string_at_cursor(),
                                                 CMP_STR_CASE_INSENSITIVE | CMP_STR_SIZE_INSENSITIVE),
                        L = string_at_cursor().size();
            if((indexBegin != UINT_MAX) &&
               (indexBegin + L < suggested.size())){
                typeString.insert(typeString.begin() + cursorPos,
                                  suggested.begin() + indexBegin + L,
                                  suggested.end());
                selectionStart = cursorPos + suggested.size() - indexBegin - L;
                typeFrameEnd += suggested.size() - indexBegin - L;
                if(typeFrameStart > 0) typeFrameStart += suggested.size() - indexBegin - L;
                bAcceptSuggestions = false;
            }

            cursorChanged = true;
            bTypeStringChanged = true;
        }

        for(auto& key : event.keyLog){
            if((key == CV_KEY_RETURN) || ((key > 31) && (key < 127))){ // ASCII non-white space
                if(ctrlPressed()){
                    if(key == 'c'){
                        std::string tmp;
                        if(selectionStart != UINT_MAX){
                            if(selectionStart < cursorPos) tmp.assign(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                            else if(selectionStart > cursorPos) tmp.assign(typeString.begin() + cursorPos, typeString.begin() + selectionStart);

                            if(selectionStart != cursorPos) copyToClipboard(tmp);
                        }
                    }
                    else if(bCanEdit && key == 'v'){
                        std::string tmp = getClipboardText();

                        if(tmp.size() > 0){
                            if(textFitType != CV_TEXT_FIT_WRAP){
                                for(size_t i = 0; i < tmp.size();){
                                    if(tmp[i] == '\n'){
                                        if(((i > 0) && (tmp[i-1] == ' ')) ||
                                           ((i < tmp.size()-1) && (tmp[i+1] == ' '))) tmp.erase(tmp.begin() + i);
                                        else{
                                            tmp[i] = ' ';
                                            ++i;
                                        }
                                    }
                                    else if(!isTypeChar(tmp[i])) tmp.erase(tmp.begin() + i);
                                    else ++i;
                                }
                            }
                            else{
                                for(size_t i = 0; i < tmp.size();){
                                    if(!isTypeChar(tmp[i])) tmp.erase(tmp.begin() + i);
                                    else ++i;
                                }
                            }

                            if(selectionStart != UINT_MAX){
                                if(selectionStart > typeString.size()) selectionStart = typeString.size();
                                if(cursorPos > typeString.size()) cursorPos = typeString.size();

                                if(selectionStart < cursorPos){
                                    typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                                    cursorPos = selectionStart;
                                }
                                else if(selectionStart > cursorPos){
                                    typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                                }
                                selectionStart = UINT_MAX;
                            }

                            typeString.insert(cursorPos, tmp);
                            cursorPos += tmp.size();
                            typeFrameEnd += tmp.size();

                            if(cursorPos > typeString.size()) cursorPos = typeString.size();
                            if(typeFrameEnd > typeString.size()) typeFrameEnd = typeString.size();
                        }
                    }
                    else if(key == 'a'){
                        selectionStart = 0;
                        cursorPos = typeString.size();
                    }
                }
                else{
                    if(key == CV_KEY_RETURN){ // Return key
                        if(bCanEdit && suggested.size() > 0){
                            typeString.erase(typeString.begin() + cursorPos, typeString.end());
                            clear_suggestion();
                        }
                        if(enterLine){
                            if(((textFitType == CV_TEXT_FIT_WRAP) ||
                                (textFitType == CV_TEXT_FIT_NONE)) && shiftPressed()){
                                    goto insertKey;
                            }
                            else{
                                enterString();
                                logTextIndex = 0;
                                if(textLog != nullptr) sendEnteredString();
                                goto skipInsertKey;
                            }
                        }
                        else if(textFitType == CV_TEXT_FIT_LATERAL) goto skipInsertKey;
                    }

                    insertKey:;

                    if(bCanEdit){

                    bAcceptSuggestions = true;

                        if(cursorPos > typeString.size()) cursorPos = typeString.size();
                        if((selectionStart != UINT_MAX) && (selectionStart > typeString.size())) selectionStart = typeString.size();

                        if(selectionStart != UINT_MAX){
                            if(selectionStart < cursorPos){
                                typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                                typeFrameEnd -= (cursorPos - selectionStart);
                                cursorPos = selectionStart;
                            }
                            else if(selectionStart > cursorPos){
                                typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                                typeFrameEnd -= (selectionStart - cursorPos);
                            }
                            selectionStart = UINT_MAX;
                        }

                        typeString.insert(typeString.begin() + cursorPos, key);
                        ++cursorPos;
                        ++typeFrameEnd;

                    }

                    skipInsertKey:;
                }
            }
            else if(bCanEdit && (key == CV_KEY_BACKSPACE) && (typeString.size() > 0)){ // Backspace
                if(selectionStart != UINT_MAX){
                    if(selectionStart < cursorPos){
                        if(cursorPos > typeString.size()) cursorPos = typeString.size();
                        typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                        cursorPos = selectionStart;
                    }
                    else if(selectionStart > cursorPos){
                        if(selectionStart > typeString.size()) selectionStart = typeString.size();
                        typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                    }
                    selectionStart = UINT_MAX;
                }
                else if(cursorPos > 0){
                    --cursorPos;
                    --typeFrameEnd;
                    typeString.erase(typeString.begin() + cursorPos);
                }
                clear_suggestion();
                bAcceptSuggestions = false;
            }
            else if(bCanEdit && (key == CV_KEY_DELETE) && (typeString.size() > 0)){ // Delete
                if(selectionStart != UINT_MAX){
                    if(selectionStart < cursorPos){
                        if(cursorPos > typeString.size()) cursorPos = typeString.size();
                        typeString.erase(typeString.begin() + selectionStart, typeString.begin() + cursorPos);
                        cursorPos = selectionStart;
                    }
                    else if(selectionStart > cursorPos){
                        if(selectionStart > typeString.size()) selectionStart = typeString.size();
                        typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                    }
                    selectionStart = UINT_MAX;
                }
                else if(cursorPos < typeString.size()){
                    if(typeFrameStart == 0) --typeFrameEnd;
                    else{
                        --typeFrameStart;
                        --typeFrameEnd;
                    }
                    typeString.erase(typeString.begin() + cursorPos);
                }
                clear_suggestion();
                bAcceptSuggestions = false;
            }
            else if(key == CV_KEY_LEFT){

                if((selectionStart != UINT_MAX) && (suggested.size() > 0)){
                    if(selectionStart > typeString.size()) selectionStart = typeString.size();
                    typeString.erase(typeString.begin() + cursorPos, typeString.begin() + selectionStart);
                }

                if(!shiftPressed()) selectionStart = UINT_MAX;
                else if(selectionStart == UINT_MAX) selectionStart = cursorPos;

                if(cursorPos > typeFrameStart) --cursorPos;
                else if((textFitType == CV_TEXT_FIT_LATERAL) && (typeFrameStart > 0)){
                    --cursorPos;
                    --typeFrameStart;
                    --typeFrameEnd;
                }

                bAcceptSuggestions = false;
            }
            else if(key == CV_KEY_RIGHT){

                if(suggested.size() > 0){
                    cursorPos = selectionStart;
                }

                if(!shiftPressed()) selectionStart = UINT_MAX;
                else if(selectionStart == UINT_MAX) selectionStart = cursorPos;

                if(cursorPos < typeFrameEnd) ++cursorPos;
                else if((textFitType == CV_TEXT_FIT_LATERAL) && (typeFrameEnd < typeString.size())){
                    ++cursorPos;
                    ++typeFrameEnd;
                    ++typeFrameStart;
                }
                bAcceptSuggestions = false;
            }

            if(has_text_target() && (textFitType == CV_TEXT_FIT_LATERAL)){
                if(key == CV_KEY_UP){
                    ++logTextIndex;
                    getTextLogString(logTextIndex-1);
                }
                else if(key == CV_KEY_DOWN){
                    if(logTextIndex > 0){
                        --logTextIndex;
                        getTextLogString(logTextIndex-1);
                    }
                }
            }
            else if(textFitType == CV_TEXT_FIT_WRAP){
                if((key == CV_KEY_UP) && (cursorPos > 0)){
                    size_t i = cursorPos - 1;
                    std::string strBuff = displayText.back().getString();
                    while((i > 0) && (strBuff[i] != '\n')) --i;
                    while((i > 0) &&
                       (displayText.back().findCharacterPos(i).x >
                                  displayText.back().findCharacterPos(cursorPos).x)) --i;
                    cursorPos = i;
                }
                else if(key == CV_KEY_DOWN){
                    size_t i = cursorPos;
                    std::string strBuff = displayText.back().getString();
                    unsigned int L = strBuff.size();
                    while((i < L) && (strBuff[i] != '\n')) ++i;
                    ++i;
                    if(strBuff[i] != '\n'){
                        while((i < L) && (strBuff[i] != '\n') &&
                              (displayText.back().findCharacterPos(i).x <
                                          displayText.back().findCharacterPos(cursorPos).x)) ++i;
                    }
                    cursorPos = i;
                }
            }
        }

        if(event.keyLog.size() > 0){
            cursorChanged = true;
            bTypeStringChanged = true;
            event.keyLog.clear(); // Capture all key strokes
            suggested.clear();
        }

        if(cursorChanged){
            bBlink = false;
            timeLastBlink = 0.0f;
        }

        // Animate

        switch(animType){
            case CV_CURSOR_ANIM_FADE:{
                if(timeLastBlink >= blinkFreq){
                    bBlink = !bBlink;
                    timeLastBlink = 0.0f;
                }
                else{
                    sf::Color newFill = cursor.getFillColor();
                    if(bBlink){
                        newFill.a = 255_BIT*(timeLastBlink/blinkFreq);
                    }
                    else{
                        newFill.a = 255_BIT*(1.0f-timeLastBlink/blinkFreq);
                    }
                    cursor.setFillColor(newFill);
                }
                break;
            }
            default:{ // Blink
                if(timeLastBlink >= blinkFreq){
                    bBlink = !bBlink;
                    timeLastBlink = 0.0f;
                }
                else{
                    sf::Color newFill = cursor.getFillColor();
                    if(bBlink){
                        newFill.a = 0_BIT;
                    }
                    else{
                        newFill.a = 255_BIT;
                    }
                    cursor.setFillColor(newFill);
                }
                break;
            }
        }

        timeLastBlink += event.lastFrameTime;
    }

    if(bTypeStringChanged){

        displayText.back().setString(typeString);

        if(textFitType == CV_TEXT_FIT_LATERAL){ // Fit laterally only
            std::string typeFrameStr;

            if(displayText.back().getGlobalBounds().width > bounds.width - 2*xPadding){
                if(typeFrameStart < typeFrameEnd){
                    typeFrameStr.assign(typeString.begin() + typeFrameStart,
                                    typeString.begin() + typeFrameEnd); // Assign frames and check again
                }
                displayText.back().setString(typeFrameStr);

                if(displayText.back().getGlobalBounds().width > bounds.width - 2*xPadding){
                    if(cursorPos >= typeFrameStr.size() + typeFrameStart){
                        ++typeFrameStart;
                    }
                    else{
                        --typeFrameEnd;
                    }
                }
            }
            else{
                typeFrameStart = 0;
                typeFrameEnd = typeString.size();
            }

            if(typeFrameStart < typeFrameEnd){
                typeFrameStr.assign(typeString.begin() + typeFrameStart,
                                typeString.begin() + typeFrameEnd);
            }
            displayText.back().setString(typeFrameStr);
        }
        else if(textFitType == CV_TEXT_FIT_WRAP){
            sf::FloatRect textBounds = displayText.back().getGlobalBounds();
            if(textBounds.width > bounds.width){
                std::string displayTextString = displayText.back().getString();

                for(size_t i = 0; i < displayTextString.size(); ++i){ // Add new line characters to fit text horizontally
                    if(displayText.back().findCharacterPos(i).x - bounds.left > bounds.width){
                        for(int j = i; j > 0; --j){
                            if(displayTextString[j-1] == ' '){
                                displayTextString[j-1] = '\n';
                                break;
                            }
                            else if(isCharType(displayTextString[j-1], ",\\/:._-")){
                                displayTextString.insert(displayTextString.begin() + j, '\n');
                                break;
                            }
                            else if(displayText.back().findCharacterPos(i).x -
                                    displayText.back().findCharacterPos(j).x > bounds.width){
                                displayTextString.insert(displayTextString.begin() + i - 1, '\n');
                                break;
                            }
                        }

                        displayText.back().setString(displayTextString);
                        textBounds = displayText.back().getGlobalBounds();
                        if(textBounds.width < bounds.width) break;
                    }
                }
            }
        }

        displayText.back().setFillColor(colorTheme.back());
    }
    else if((typeString.size() < 1) && !hasFocus() && (bkgString.size() > 0)){
        displayText.back().setString(this->bkgString);
        sf::Color bkgColor = colorTheme.back();
        bkgColor.a = bkgColor.a > 100 ? bkgColor.a - 100 : 0;
        displayText.back().setFillColor(bkgColor);
    }
    else if((typeString.size() < 1) && hasFocus()){
        displayText.back().setString("");
    }

    switch(textInfo.alignment){ // Shift position to match typing
        case ALIGN_VERTICAL_INVERTED:{
            displayText.back().setOrigin(displayText.back().getLocalBounds().width/2,
                                         displayText.back().getLocalBounds().height/2);
            displayText.back().setPosition(panel.front().getPosition());
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        case ALIGN_CENTER_BOTTOM:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        case ALIGN_CENTER:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        default:{ // Unknown text alignment
            break;
        }
    }

    // Handle text expansion
    if(expandType){
        sf::FloatRect textBounds = displayText.back().getGlobalBounds();
        float expandDist = 0.0f;
        if(expandType & CV_TEXT_EXPAND_BOTTOM){
            expandDist = (textBounds.top + textBounds.height - bounds.top - bounds.height)*expandRate;
            if(expandDist > 0.0f){
                if(bounds.height + expandDist < textBounds.top + textBounds.height)
                    bounds.height += expandDist;
                else bounds.height = textBounds.top + textBounds.height - bounds.top;
            }
            else if(expandDist < 0.0f){
                if(bounds.top + bounds.height + expandDist > initBounds.top + initBounds.height){
                    bounds.height += expandDist;
                }
                else bounds.height = initBounds.height;
            }
        }
        if(expandType & CV_TEXT_EXPAND_TOP){
//            if(textBounds.top < bounds.top){
//                move(sf::Vector2f(0.0f,-expandRate));
//                bounds.height += expandRate;
//            }
//            else if((textBounds.top < bounds.top - expandRate) &&
//                    (bounds.top < initBounds.top)){
//                move(sf::Vector2f(0.0f,expandRate));
//                bounds.height -= expandRate;
//            }
        }
        if(expandType & CV_TEXT_EXPAND_RIGHT){
//            if(textBounds.left + textBounds.width > bounds.left + bounds.width){
//                bounds.width += expandRate;
//            }
//            else if((textBounds.left + textBounds.width < bounds.left + bounds.width - expandRate) &&
//                    (bounds.left + bounds.width > initBounds.left + initBounds.width)){
//                bounds.width -= expandRate;
//            }
        }
        if(expandType & CV_TEXT_EXPAND_LEFT){
//            if(textBounds.left < bounds.left){
//                move(sf::Vector2f(-expandRate, 0.0f));
//                bounds.width += expandRate;
//            }
//            else if((textBounds.left < bounds.left - expandRate) &&
//                    (bounds.left < initBounds.left)){
//                move(sf::Vector2f(expandRate, 0.0f));
//                bounds.width -= expandRate;
//            }
        }
        panel.front().setSize(sf::Vector2f(bounds.width, bounds.height));
    }

    // Move the cursor
    if(hasFocus()){
        sf::Vector2f movePos(0.0f,0.0f);

        if((textInfo.alignment == ALIGN_VERTICAL_INVERTED) || (textInfo.alignment == ALIGN_VERTICAL)){
            movePos.y = displayText.back().findCharacterPos(cursorPos - typeFrameStart).y + 1.0f;
            movePos.x = displayText.back().findCharacterPos(cursorPos - typeFrameStart).x;
            View->anim_to(&cursor, movePos, pow(getDistance(cursor.getPosition(), movePos), 2), CV_OBJ_ANIM_SLIDE);
        }
        else{
            movePos.x = displayText.back().findCharacterPos(cursorPos - typeFrameStart).x - 1.0f;
            movePos.y = displayText.back().findCharacterPos(cursorPos - typeFrameStart).y;
            View->anim_to(&cursor, movePos, pow(getDistance(cursor.getPosition(), movePos), 2), CV_OBJ_ANIM_SLIDE);
        }
    }

    if(hasFocus() && (selectionStart != UINT_MAX) && (selectionStart != cursorPos)){

        sf::Color textHighlightColor;
        if(cursorChanged || bTypeStringChanged){
            lineFrames.clear();
            lineFrames.emplace_back(2);
            selectionLineStart = 0;
            selectionLineEnd = 0;

            textHighlightColor = cursor.getFillColor();
            textHighlightColor.a = 60;
            std::string stringBuff = displayText.back().getString();

            if(selectionStart < cursorPos){
                for(size_t i = rangeBegin, line = 0; i <= cursorPos; ++i){
                    if(selectionStart == i){
                        selectionLineStart = line;
                        lineFrames.back()[0] = i;
                    }
                    else if(cursorPos == i){
                        selectionLineEnd = line;
                        lineFrames.back()[1] = i;
                    }

                    if(stringBuff[i] == '\n'){
                        ++line;
                        lineFrames.back()[1] = i;
                        lineFrames.emplace_back(2);
                        lineFrames.back()[0] = i+1;
                    }
                }
                lineFrames.back()[1] = cursorPos;
            }
            else{
                for(size_t i = rangeBegin, line = 0; i <= selectionStart; ++i){
                    if(cursorPos == i){
                        selectionLineStart = line;
                        lineFrames.back()[0] = i;
                    }
                    if(selectionStart == i){
                        selectionLineEnd = line;
                        lineFrames.back()[1] = i;
                    }

                    if(stringBuff[i] == '\n'){
                        ++line;
                        lineFrames.back()[1] = i;
                        lineFrames.emplace_back(2);
                        lineFrames.back()[0] = i+1;
                    }
                }
                lineFrames.back()[1] = selectionStart;
            }

            if(panel.size() > 1){
                panel.erase(panel.begin() + 1, panel.end());
            }

        }
        float selectionWidth = 0.0f, lineSelectBegin = 0.0f, lineSelectEnd = 0.0f;

        for(size_t i = selectionLineStart, j = 1; i <= selectionLineEnd; ++i, ++j){
            if(cursorChanged || bTypeStringChanged){
                panel.emplace_back();
                panel.back().setFillColor(textHighlightColor);
                panel.back().setOrigin(cursor.getOrigin());
            }

            if(textInfo.alignment == ALIGN_VERTICAL){
                lineSelectBegin = displayText.back().findCharacterPos(lineFrames[i][0]).y;
                lineSelectEnd = displayText.back().findCharacterPos(lineFrames[i][1]).y;
            }
            else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                lineSelectBegin = displayText.back().findCharacterPos(lineFrames[i][1]).y;
                lineSelectEnd = displayText.back().findCharacterPos(lineFrames[i][0]).y;
            }
            else{
                lineSelectBegin = displayText.back().findCharacterPos(lineFrames[i][0]).x;
                lineSelectEnd = displayText.back().findCharacterPos(lineFrames[i][1]).x;
            }
            panel[j].setPosition(displayText.back().findCharacterPos(lineFrames[i][0]));

            if(i == selectionLineEnd){
                if(cursorPos > selectionStart){
                    if(textInfo.alignment == ALIGN_VERTICAL){
                        lineSelectEnd = cursor.getPosition().y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                        lineSelectEnd = cursor.getPosition().y;
                    }
                    else{
                        lineSelectEnd = cursor.getPosition().x;
                    }
                }
                else{
                    if(textInfo.alignment == ALIGN_VERTICAL){
                        lineSelectEnd = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                        lineSelectEnd = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else{
                        lineSelectEnd = displayText.back().findCharacterPos(selectionStart).x;
                    }
                }
            }
            if(i == selectionLineStart){
                if(cursorPos < selectionStart){
                    if(textInfo.alignment == ALIGN_VERTICAL){
                        lineSelectBegin = cursor.getPosition().y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                        lineSelectBegin = cursor.getPosition().y;
                    }
                    else{
                        lineSelectBegin = cursor.getPosition().x;
                    }
                    panel[j].setPosition(cursor.getPosition());
                }
                else{
                    if(textInfo.alignment == ALIGN_VERTICAL){
                        lineSelectBegin = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                        lineSelectBegin = displayText.back().findCharacterPos(selectionStart).y;
                    }
                    else{
                        lineSelectBegin = displayText.back().findCharacterPos(selectionStart).x;
                    }
                    panel[j].setPosition(displayText.back().findCharacterPos(selectionStart));
                }
            }

            selectionWidth = lineSelectEnd - lineSelectBegin;

            if(textInfo.alignment == ALIGN_VERTICAL){
                panel[j].setSize(sf::Vector2f(cursor.getGlobalBounds().width, selectionWidth));
            }
            else if(textInfo.alignment == ALIGN_VERTICAL_INVERTED){
                panel[j].setSize(sf::Vector2f(cursor.getGlobalBounds().width, selectionWidth));
            }
            else{
                panel[j].setSize(sf::Vector2f(selectionWidth, cursor.getGlobalBounds().height));
            }
        }
    }
    else if((panel.size() > 1) && (!hasFocus() || cursorChanged || bTypeStringChanged)){
        panel.erase(panel.begin() + 1, panel.end());
    }

    bTypeStringChanged = false;
    return true;
}

void CVTypeBox::setAlignment(const uint8_t& alignment){
    textInfo.alignment = alignment;
    switch(alignment){ // Shift position to match typing
        case ALIGN_VERTICAL_INVERTED:{
            displayText.back().setOrigin(displayText.back().getLocalBounds().width/2,
                                         displayText.back().getLocalBounds().height/2);
            displayText.back().setPosition(panel.front().getPosition());
            break;
        }
        case ALIGN_CENTER_MIDLINE:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        case ALIGN_CENTER_BOTTOM:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        case ALIGN_CENTER:{
            float centerDist = bounds.left + bounds.width/2 - displayText.back().getPosition().x -
                            displayText.back().getGlobalBounds().width/2;
            displayText.back().move(centerDist, 0.0f);
            break;
        }
        default:{ // Unknown text alignment
            break;
        }
    }
}

bool CVTypeBox::draw(sf::RenderTarget* target){
    if(!CVTextBox::draw(target)) return false;
    if(hasFocus()){
        target->draw(cursor);
    }
    return true;
}

CVTextLog::CVTextLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
                     const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
                     const float& borderWidth, const uint8_t& animType, const std::string& logFile,
                     CVTypeBox* userEntrySource):
                         CVTextBox(View, position, width, height, fillColor, borderColor, borderWidth),
                         usrEntryBox(userEntrySource),
                         logFile(logFile),
                         selectedLogIndex(0),
                         maxMessages(1000),
                         initialMessageCount(50),
                         messageLoadInc(25),
                         timeWaiting(0.0f),
                         scrollOffsetY(0.0f),
                         msgPopupSpeed(600.0f),
                         msgScrollSpeed(3600.0f),
                         maxPanelWidthPct(0.7f),
                         msgPanelPaddingPx(12.0f*View->getViewScale()),
                         scrollBar(sf::Vector2f(width*0.02f, 0.0f), width*0.01f),
                         templateTheme(textInfo),
                         bClear(false),
                         bMsgReady(true),
                         bCanScroll(true),
                         animType(animType),
                         userTextAlign(ALIGN_RIGHT),
                         otherTextAlign(ALIGN_LEFT){

    colorTheme.emplace_back(borderColor); // Message panel fill
    brighten(colorTheme.back(), 20_BIT);
    colorTheme.emplace_back(borderColor); // Message panel border
    colorTheme.emplace_back(getMsgPanelFillColor()); // Incoming message panel fill
    brighten(colorTheme.back(), 60_BIT);
    colorTheme.emplace_back(getMsgPanelFillColor()); // Highlight panel color
    darken(colorTheme.back(), 50_BIT);
    colorTheme.emplace_back(textInfo.textColor);

    sf::Color scrollBarColor = getMsgPanelFillColor();
    scrollBarColor.a = 0_BIT;   // Invisible until scroll
    scrollBar.setFillColor(scrollBarColor);
    scrollBar.setPosition(sf::Vector2f(bounds.left + bounds.width*0.97f,
                                       bounds.top + bounds.height*0.025f));

    setDrawClipping(true);
}

void CVTextLog::move(const sf::Vector2f& distance){
    CVTextBox::move(distance);
    for(auto& item : msgPanels){
        item.move(distance);
    }
    scrollBar.move(distance);
}

void CVTextLog::setPosition(const sf::Vector2f& position){
    move(position - getPosition());
}

void CVTextLog::setSize(const sf::Vector2f& newSize){
    float offsetY = newSize.y - getSize().y,
            offsetX = newSize.x - getSize().x;
    CVBox::setSize(newSize);
    for(size_t i = 0; i < msgPanels.size(); ++i){
        if(textLogUserEntered[i]){
            msgPanels[i].move(offsetX, offsetY);
            displayText[i].move(offsetX, offsetY);
        }
        else{
            msgPanels[i].move(0.0f, offsetY);
            displayText[i].move(0.0f, offsetY);
        }
    }
}

void CVTextLog::shiftMsgs(const float& dist){
    // Negative is up (SFML coords)
    for(size_t i = 0; i < msgPanels.size(); ++i){
        View->anim_move(&msgPanels[i],
                        sf::Vector2f(0.0f, dist),
                        msgPopupSpeed, animType);
        View->anim_move(&displayText[i],
                        sf::Vector2f(0.0f, dist),
                        msgPopupSpeed, animType);
    }
}

bool CVTextLog::update(CVEvent& event, const sf::Vector2f& mousePos){

    if(bClear){
        scrollOffsetY = 0.0f;
        textLog.clear();
        times.clear();
        msgPanels.clear();
        displayText.clear();
        msgPanels.reserve(maxMessages);
        displayText.reserve(maxMessages);
        textLogUserEntered.clear();
        save();
        bClear = false;
    }

    if(bMsgReady && (waitingText.size() > 0)){
        printLock.lock();
        if(timeWaiting < updateAddDelay.front()){
            timeWaiting += event.lastFrameTime;
        }
        else{
            updateAddDelay.erase(updateAddDelay.begin());
            timeWaiting = 0.0f;

            times.addTimePoint();

            textLog.push_back(waitingText.front());
            textLogUserEntered.push_back(textLogUserWaiting.front());

            addTextEntry(textEntry(waitingText.front(),
                                   templateTheme.font,
                                   templateTheme.fontSize,
                                   templateTheme.alignment,
                                   templateTheme.textColor),
                          sf::Vector2f(bounds.left, bounds.top));

            msgPanels.emplace_back(sf::Vector2f(0.0f,0.0f), 10.0f*View->getViewScale());
            msgPanels.back().setOutlineColor(getMsgPanelBorderColor());

            wrapLastMsg();

            sf::FloatRect textBounds = displayText.back().getGlobalBounds();

            float moveBump = 0.0f, shiftDist = msgPanels.back().getSize().y + msgPanelPaddingPx/2 + getTextCenterOffsetY(displayText.back())/2;
            if(textLogUserEntered.size() > 1){
                if((textLogUserEntered.back() & !textLogUserEntered[textLogUserEntered.size()-2]) ||
                   (!textLogUserEntered.back() & textLogUserEntered[textLogUserEntered.size()-2])){
                    moveBump = msgPanelPaddingPx;
                }
            }

            for(size_t i = 0; i < msgPanels.size() - 1; ++i){
                View->anim_move(&msgPanels[i],
                                sf::Vector2f(0.0f, -(shiftDist + moveBump)),
                                msgPopupSpeed, animType);
                View->anim_move(&displayText[i],
                                sf::Vector2f(0.0f, -(shiftDist + moveBump)),
                                msgPopupSpeed, animType);
            }

            if(textLogUserWaiting.front()){
                switch(userTextAlign){
                    case ALIGN_LEFT:{
                        displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                            bounds.top + bounds.height + scrollOffsetY));
                        break;
                    }
                    default:{ // Right
                        displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - textBounds.width,
                                                            bounds.top + bounds.height + scrollOffsetY));
                        break;
                    }
                }
                msgPanels.back().setFillColor(getMsgPanelFillColor());
            }
            else{
                switch(otherTextAlign){
                    case ALIGN_RIGHT:{
                        displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - textBounds.width,
                                                            bounds.top + bounds.height + scrollOffsetY));
                        break;
                    }
                    default:{ // Left
                        displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                            bounds.top + bounds.height + scrollOffsetY));
                        break;
                    }
                }
                msgPanels.back().setFillColor(getIncomingMsgPanelFillColor());
                displayText.back().setFillColor(getOtherTextColor());
            }

            msgPanels.back().setPosition(displayText.back().getPosition().x - msgPanelPaddingPx,
                                                displayText.back().getPosition().y - msgPanelPaddingPx/2 +
                                         getTextCenterOffsetY(displayText.back())/2);

            View->anim_move(&displayText.back(),
                              sf::Vector2f(0.0f,
                                           -(shiftDist + 12.0f)),
                              msgPopupSpeed, animType);
            View->anim_move(&msgPanels.back(),
                          sf::Vector2f(0.0f,
                                       -(shiftDist + 12.0f)),
                          msgPopupSpeed, animType);

            waitingText.erase(waitingText.begin());
            textLogUserWaiting.erase(textLogUserWaiting.begin());

            save();
        }
        printLock.unlock();
    }
    else{
        timeWaiting = 0.0f;
    }

    // Process scrolling

    if((bounds.contains(mousePos) || (event.LMBhold && bounds.contains(event.LMBpressPosition)))
        && event.captureFocus()) setFocus(true);
    else if(event.LMBhold) setFocus(false);

    float scrollRange = 0.0f, scrollDist = 0.0f, scrollSpeed = msgScrollSpeed;
    if(bCanScroll){
        if(textLog.size() > 0){
            scrollRange = 2*msgPanelPaddingPx + msgPanels.back().getPosition().y + msgPanels.back().getSize().y - msgPanels.front().getPosition().y
                        - bounds.height;

            if(scrollRange > 0.0f){
                scrollBar.setSize(sf::Vector2f(scrollBar.getSize().x,
                                  0.95f*bounds.height*bounds.height/(scrollRange+bounds.height)));
                View->anim_to(&scrollBar, sf::Vector2f(bounds.left + bounds.width*0.97f,
                                      bounds.top + bounds.height*0.025f +
                                      ((bounds.height * 0.95f) - scrollBar.getSize().y)*
                                      (1.0f-(scrollOffsetY/(scrollRange)))), msgScrollSpeed*(bounds.height*0.95f-scrollBar.getSize().y)/scrollRange,
                              CV_OBJ_ANIM_SLIDE, true);
            }
        }
    }

    // Process clicking

    if(event.LMBhold && bounds.contains(mousePos)){
        if(hasFocus()){
            if((usrEntryBox != nullptr) &&
               event.captureMouse()){
               if(event.LMBholdTime < 0.2f){
                    unsigned int index = 0, textIndex;
                    std::string checkString;
                    float xCtr;
                    sf::FloatRect panelBounds;

                    for(auto& item : msgPanels){
                        panelBounds = item.getGlobalBounds();
                        xCtr = item.getPosition().x + panelBounds.width/2;
                        if(panelBounds.contains(mousePos)){
                            if(xCtr > bounds.left + bounds.width/2 - 2.0f){
                                item.setFillColor(getHighlightPanelFillColor());
                                usrEntryBox->setTypeString(displayText[index].getString());
                                selectedLogIndex = index;
                            }
                            else if(xCtr < bounds.left + bounds.width/2 + 2.0f){
                                item.setFillColor(getHighlightPanelFillColor());
                                selectedLogIndex = index;
                            }
                        }
                        else{
                            if(xCtr < bounds.left + bounds.width/2 - 2.0f){
                                item.setFillColor(getIncomingMsgPanelFillColor());
                            }
                            else if(xCtr > bounds.left + bounds.width/2 + 2.0f){
                                item.setFillColor(getMsgPanelFillColor());
                            }
                        }
                        ++index;
                    }
               }
               else if((event.LMBpressPosition.x > bounds.left + bounds.width*0.9f) &&
                        (event.LMBpressPosition.x < bounds.left + bounds.width*1.05f)){
                    if((scrollRange > 0.0f) && (event.LMBholdTime > 0.2f)){
                        scrollSpeed *= 3;
                        scrollDist = (scrollBar.getPosition().y + scrollBar.getSize().y/2 - mousePos.y)*event.lastFrameTime*20;
                    }
                }
            }
        }
    }

    bool fadeOut = true;

    if(bCanScroll && bounds.contains(mousePos) && event.viewHasFocus){

        if(event.mouseWheelDelta.y != 0.0f){
            scrollDist = 20.0f*event.mouseWheelDelta.y;
        }
        if(mousePos.x > bounds.left + bounds.width*0.9f){
            if(scrollBar.getFillColor().a < 255_BIT) View->anim_passive(&scrollBar, 1000.0f, CV_OBJ_ANIM_FADE_IN);
            fadeOut = false;
        }
        if(scrollDist != 0.0f){

            if((scrollDist < 0.0f) && (scrollOffsetY > 0.0f)){
                if(scrollOffsetY + scrollDist < 0.0f) scrollDist = -scrollOffsetY;
                scrollOffsetY += scrollDist;
            }
            else if((scrollDist > 0.0f) && (scrollOffsetY < scrollRange)){
                if(scrollDist + scrollOffsetY > scrollRange) scrollDist = scrollRange - scrollOffsetY;
                scrollOffsetY += scrollDist;
            }
            else{
                scrollDist = 0.0f;
            }

            if(scrollBar.getFillColor().a < 255_BIT) View->anim_passive(&scrollBar, 1000.0f, CV_OBJ_ANIM_FADE_IN);

            if(scrollDist != 0.0f){
                for(size_t i = 0; i < msgPanels.size(); ++i){
                    View->anim_move(&msgPanels[i], sf::Vector2f(sf::Vector2f(0.0f, scrollDist)), msgScrollSpeed, animType);
                    View->anim_move(&displayText[i], sf::Vector2f(sf::Vector2f(0.0f, scrollDist)), msgScrollSpeed, animType);
                }
            }
            fadeOut = false;
        }
    }

    if(fadeOut && (scrollBar.getFillColor().a > 0)){
        View->anim_passive(&scrollBar, 30.0f, CV_OBJ_ANIM_FADE_OUT);
    }

    // Fade elements at the edge of the top scroll boundary

    sf::Color tmp;
    float gradient = 0.0f;
    for(auto& item : msgPanels){
        tmp = item.getFillColor();
        if(item.getGlobalBounds().height > bounds.height){
            gradient = (item.getPosition().y + item.getGlobalBounds().height - bounds.top)/bounds.height*4;
        }
        else{
            gradient = (item.getPosition().y + item.getGlobalBounds().height/2 - bounds.top)/item.getGlobalBounds().height;
        }

        if(gradient < 0.0f) gradient = 0.0f;
        if(gradient > 1.0f) gradient = 1.0f;
        tmp.a = getMsgPanelFillColor().a * gradient;
        item.setFillColor(tmp);
    }
    for(auto& item : displayText){
        tmp = item.getFillColor();
        if(item.getGlobalBounds().height > bounds.height){
            gradient = pow((item.getPosition().y + item.getGlobalBounds().height - bounds.top)/bounds.height*5, 1.0f/3);
        }
        else{
            gradient = sqrt((item.getPosition().y + item.getGlobalBounds().height/2 - bounds.top)/item.getGlobalBounds().height);
        }

        if(gradient < 0.0f) gradient = 0.0f;
        if(gradient > 1.0f) gradient = 1.0f;
        tmp.a = templateTheme.textColor.a * gradient;
        item.setFillColor(tmp);
    }

    if(hasFocus()){
        for(auto& key : event.keyLog){
            switch(key){
                case char(10):{
                    if((usrEntryBox != nullptr) && usrEntryBox->canEnterLine()){
                        usrEntryBox->enterString();
                        usrEntryBox->sendEnteredString();
                    }
                    break;
                }
                case 'c':{
                    if(ctrlPressed()){
                        copyToClipboard(displayText[selectedLogIndex].getString());
                    }
                    break;
                }
                default: break;
            }
        }
    }

    return true;

}

void CVTextLog::addLogEntry(const std::string& text, bool userEntry, float updateDelay){
    printLock.lock();
    waitingText.push_back(text);
    textLogUserWaiting.push_back(userEntry);
    updateAddDelay.push_back(updateDelay);
    if(userEntry) lastUserEntered = text;
    else lastOtherEntered = text;
    printLock.unlock();
}

void CVTextLog::wrapLastMsg(){
    sf::FloatRect textBounds = displayText.back().getGlobalBounds();

    if(!isnan(maxPanelWidthPct)){
        if(textBounds.width > bounds.width*maxPanelWidthPct){
            std::string displayTextString = displayText.back().getString();

            for(size_t i = 0; i < displayTextString.size(); ++i){ // Add new line characters to fit text horizontally
                if(displayText.back().findCharacterPos(i).x - bounds.left > bounds.width*maxPanelWidthPct){
                    for(int j = i; j > 0; --j){
                        if(displayTextString[j-1] == ' '){
                            displayTextString[j-1] = '\n';
                            break;
                        }
                        else if(isCharType(displayTextString[j-1], ",\\/:._-")){
                            displayTextString.insert(displayTextString.begin() + j, '\n');
                            break;
                        }
                        else if(displayText.back().findCharacterPos(i).x -
                                displayText.back().findCharacterPos(j).x > bounds.width*maxPanelWidthPct){
                            displayTextString.insert(displayTextString.begin() + i - 1, '\n');
                            ++i;
                            break;
                        }
                    }

                    displayText.back().setString(displayTextString);
                    textBounds = displayText.back().getGlobalBounds();
                    if(textBounds.width < bounds.width*maxPanelWidthPct) break;
                }
            }
        }
    }

    msgPanels.back().setSize(sf::Vector2f(textBounds.width + 2*msgPanelPaddingPx,
                                        textBounds.height + 2*msgPanelPaddingPx));
}

void CVTextLog::addToLastEntry(const std::string& text, bool userEntry){
    printLock.lock();
    if((waitingText.size() > 0) && (textLogUserWaiting.back() == userEntry)){
        waitingText.back().append(text);
    }
    else if((textLogUserEntered.size() > 0) && (textLogUserEntered.back() == userEntry)){
        textLog.back().append(text);
        displayText.back().setString(textLog.back());
        float shiftDist = displayText.back().getGlobalBounds().height;
        wrapLastMsg();
        shiftDist -= displayText.back().getGlobalBounds().height;
        if(shiftDist != 0.0f){
            shiftMsgs(shiftDist);
        }
    }
    else{
        addLogEntry(text, userEntry, 0.0f);
    }
    printLock.unlock();
}

void CVTextLog::editLastEntry(const std::string& text, bool userEntry){
    printLock.lock();
    if((waitingText.size() > 0) && (textLogUserWaiting.back() == userEntry)){
        waitingText.back() = text;
    }
    else if((textLogUserEntered.size() > 0) && (textLogUserEntered.back() == userEntry)){
        textLog.back() = text;
        displayText.back().setString(text);
        float shiftDist = displayText.back().getGlobalBounds().height;
        wrapLastMsg();
        shiftDist -= displayText.back().getGlobalBounds().height;
        if(shiftDist != 0.0f){
            shiftMsgs(shiftDist);
        }
    }
    else{
        addLogEntry(text, userEntry, 0.0f);
    }
    printLock.unlock();
}

bool CVTextLog::save(){

    if(logFile.size() < 1) return false;

    if(!access(logFile.c_str(), F_OK)){
        if(access(logFile.c_str(), W_OK)) return false;
    }

    FILE* outFILE = fopen(logFile.c_str(), "wb");

    size_t L = textLog.size(), sSIZE;
    fwrite(&L, 1, sizeof(size_t), outFILE);
    bool userEntered;

    for(size_t i = 0; i < L; ++i){
        sSIZE = textLog[i].size();
        fwrite(&sSIZE, 1, sizeof(size_t), outFILE);
        fwrite(textLog[i].c_str(), sizeof(char), textLog[i].size(), outFILE);
        fwrite(times[i].data(), sizeof(int), 6, outFILE);

        userEntered = textLogUserEntered[i];
        fwrite(&userEntered, 1, sizeof(bool), outFILE);
    }

    fclose(outFILE);

    return true;
}

bool CVTextLog::load(){

    if(logFile.empty()) return false;

    if(access(logFile.c_str(), F_OK)){
        std::cout << "Error loading text log from " << logFile << '\n';
        std::cout << "Error: " << std::strerror(errno) << '\n';
        return false;
    }

    msgPanels.clear();
    displayText.clear();
    msgPanels.reserve(maxMessages);
    displayText.reserve(maxMessages);

    FILE* inFILE = fopen(logFile.c_str(), "rb");

    size_t L, cSIZE;
    fread(&L, 1, sizeof(size_t), inFILE);
    int newTime [] = {0,0,0,0,0,0};
    bool userEntered;

    textLog.clear();                textLog.reserve(L);
    textLogUserEntered.clear();     textLogUserEntered.reserve(L);
    times.clear();                  times.reserve(L);

    for(size_t i = 0; i < L; ++i){
        fread(&cSIZE, 1, sizeof(size_t), inFILE);
        char* newStr = new char[cSIZE+1];
        newStr[cSIZE] = '\0';
        fread(newStr, cSIZE, sizeof(char), inFILE);
        textLog.emplace_back();
        textLog.back().assign(newStr, newStr + cSIZE);

        fread(newTime, 6, sizeof(int), inFILE);
        times.emplace_back(newTime);

        fread(&userEntered, 1, sizeof(bool), inFILE);
        textLogUserEntered.push_back(userEntered);
    }

    fclose(inFILE);

    int date = 0, newDate = 0;
    unsigned int loadBegin = L <= initialMessageCount ? 0 : L - initialMessageCount; // Load an initial count, then require user scroll

    for(size_t N = loadBegin; N < L; ++N){

        newDate = times[N].year() * times[N].month() * times[N].day();

        if(date != newDate){ // Show the date when days change
            sf::FloatRect textBounds;

            std::string dateString;
            TimePoint time_now;

            if(newDate == time_now.year() * time_now.month() * time_now.day()) dateString = "Today";
            else dateString = times[N].getDate();

            addTextEntry(textEntry(dateString,
                                templateTheme.font,
                               templateTheme.fontSize,
                               ALIGN_CENTER_BOTTOM,
                            getMsgPanelFillColor()),
                         sf::Vector2f(bounds.left + bounds.width/2, bounds.top + bounds.height - 48.0f));
            textBounds = displayText.back().getGlobalBounds();
            sf::Color dateColor = getMsgPanelFillColor();
            dateColor.a = 60;
            msgPanels.emplace_back(sf::Vector2f(textBounds.width + 4*msgPanelPaddingPx,
                                           textBounds.height + 2*msgPanelPaddingPx),
                                   10.0f*View->getViewScale());
            msgPanels.back().setPosition(sf::Vector2f(displayText.back().getPosition().x - 2*msgPanelPaddingPx,
                                                      displayText.back().getPosition().y - msgPanelPaddingPx/2
                                                      + getTextCenterOffsetY(displayText.back())/2));
            msgPanels.back().setFillColor(dateColor);

            for(size_t i = 0; i < msgPanels.size() - 1; ++i){
                msgPanels[i].move(sf::Vector2f(0.0f, -(msgPanels.back().getSize().y + 1.5*msgPanelPaddingPx)));
                displayText[i].move(sf::Vector2f(0.0f, -(msgPanels.back().getSize().y + 1.5*msgPanelPaddingPx)));
            }

            date = newDate;
        }

        addTextEntry(textEntry(textLog[N],
                               templateTheme.font,
                               templateTheme.fontSize,
                               templateTheme.alignment,
                               templateTheme.textColor),
                      sf::Vector2f(bounds.left, bounds.top));

        msgPanels.emplace_back(sf::Vector2f(0.0f,0.0f), 10.0f*View->getViewScale());
        msgPanels.back().setOutlineColor(getMsgPanelBorderColor());

        wrapLastMsg();

        sf::FloatRect textBounds = displayText.back().getGlobalBounds();

        float moveBump = 0.0f, shiftDist = msgPanels.back().getSize().y + msgPanelPaddingPx/2 + getTextCenterOffsetY(displayText.back())/2;
        if(N > 0){
            if((textLogUserEntered[N-1] & !textLogUserEntered[N]) ||
               (!textLogUserEntered[N-1] & textLogUserEntered[N])){
                moveBump = msgPanelPaddingPx;
            }
        }

        for(size_t i = 0; i < msgPanels.size() - 1; ++i){
            msgPanels[i].move(sf::Vector2f(0.0f, -(shiftDist + moveBump)));
            displayText[i].move(sf::Vector2f(0.0f, -(shiftDist + moveBump)));
        }

        if(textLogUserEntered[N]){
            switch(userTextAlign){
                case ALIGN_LEFT:{
                    displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                        bounds.top + bounds.height + scrollOffsetY));
                    break;
                }
                default:{ // Right
                    displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - textBounds.width,
                                                        bounds.top + bounds.height + scrollOffsetY));
                    break;
                }
            }
            msgPanels.back().setFillColor(getMsgPanelFillColor());
        }
        else{
            switch(otherTextAlign){
                case ALIGN_RIGHT:{
                    displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.92f - textBounds.width,
                                                        bounds.top + bounds.height + scrollOffsetY));
                    break;
                }
                default:{ // Left
                    displayText.back().setPosition(sf::Vector2f(bounds.left + bounds.width*0.05f,
                                                        bounds.top + bounds.height + scrollOffsetY));
                    break;
                }
            }
            msgPanels.back().setFillColor(getIncomingMsgPanelFillColor());
            displayText.back().setFillColor(getOtherTextColor());
        }

        msgPanels.back().setPosition(displayText.back().getPosition().x - msgPanelPaddingPx,
                                            displayText.back().getPosition().y - msgPanelPaddingPx/2 +
                                     + getTextCenterOffsetY(displayText.back())/2);

        displayText.back().move(
                          sf::Vector2f(0.0f,
                                       -(shiftDist + 12.0f)));
        msgPanels.back().move(
                      sf::Vector2f(0.0f,
                                   -(shiftDist + 12.0f)));

    }

    if(textLog.size() > 0){
        float scrollRange = 2*msgPanelPaddingPx + msgPanels.back().getPosition().y + msgPanels.back().getSize().y - msgPanels.front().getPosition().y
                    - bounds.height;

        if(scrollRange > 0.0f){
            scrollBar.setSize(sf::Vector2f(scrollBar.getSize().x,
                              0.95f*bounds.height*bounds.height/(scrollRange+bounds.height)));
            scrollBar.setPosition(sf::Vector2f(bounds.left + bounds.width*0.97f,
                                  bounds.top + bounds.height*0.975f - scrollBar.getSize().y));
        }
    }

    return true;

}

bool CVTextLog::draw(sf::RenderTarget* target){
    if(View->viewPort == nullptr) return false;
    if(!visible) return false;

    CV_DRAW_CLIP_BEGIN

    for(auto& item : panel){
        target->draw(item);
    }
    for(auto& sprite : spriteList){
        target->draw(sprite);
    }
    for(auto& panel : msgPanels){
        if(bounds.intersects(panel.getGlobalBounds()))
            target->draw(panel);
    }
    for(auto& text : displayText){
        if(bounds.intersects(text.getGlobalBounds()))
            target->draw(text);
    }
    for(auto& item : mask){
        target->draw(item);
    }
    target->draw(scrollBar);

    CV_DRAW_CLIP_END

    return true;
}

}
