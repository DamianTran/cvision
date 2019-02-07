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

#ifndef CVIS_TYPEIO
#define CVIS_TYPEIO

#include <SFML/Graphics.hpp>

#include "cvision/time.hpp"
#include "cvision/textbox.hpp"

#include <mutex>

#ifndef CVIS_ANIM_MACROS
#define CVIS_ANIM_MACROS

#define CV_OBJ_ANIM_NONE                0
#define CV_OBJ_ANIM_SLIDE               1
#define CV_OBJ_ANIM_ACCEL               2
#define CV_OBJ_ANIM_DECEL               3

#define CV_OBJ_ANIM_FADE_IN             4
#define CV_OBJ_ANIM_FADE_OUT            5
#define CV_OBJ_ANIM_ROTATE              6
#endif

// Type box

#define CV_CURSOR_ANIM_BLINK            0
#define CV_CURSOR_ANIM_FADE             1

#define CV_TEXT_FIT_NONE                0
#define CV_TEXT_FIT_LATERAL             1
#define CV_TEXT_FIT_WRAP                2

#define CV_TEXT_EXPAND_NONE             0b0
#define CV_TEXT_EXPAND_RIGHT            0b1
#define CV_TEXT_EXPAND_BOTTOM           0b10
#define CV_TEXT_EXPAND_TOP              0b100
#define CV_TEXT_EXPAND_LEFT             0b1000
#define CV_TEXT_EXPAND_LATERAL          CV_TEXT_EXPAND_LEFT |   CV_TEXT_EXPAND_RIGHT
#define CV_TEXT_EXPAND_VERTICAL         CV_TEXT_EXPAND_TOP  |   CV_TEXT_EXPAND_BOTTOM
#define CV_TEXT_EXPAND_ALL              0b11111111

// =============================================

namespace cvis{

class CVTypeBox;

class CVISION_API CVTextLogMsg : public CVTextBox
{
public:

    CVISION_API CVTextLogMsg(CVView* View,
                 const textEntry& newTheme = textEntry("", ""));

    bool bUser;
    TimePoint time;

};

class CVISION_API CVTextLog: public CVTextBox{
public:

    CVISION_API bool save();
    CVISION_API bool load();

    CVISION_API std::string getReadableLog() const; // Print an expanded, readable log

    inline void clear(){ bClear = true; } // Handle in this thread at next opportunity

    inline std::string getLogText(const unsigned int& index) const{
        if(index > textLog.size()) return std::string();
        return textLog[index]; }

    CVISION_API std::string getUserLogText(const unsigned int& index, bool lastEntered = true) const;
    CVISION_API std::string getOtherLogText(const unsigned int& index, bool lastEntered = true) const;

    CVISION_API void replaceLastMessage(const std::string& search,
                            const std::string& substitute,
                            bool user = true); // Substitute string in last message (ie. password obfuscation)

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& distance);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    inline void setPopupSpeed(const float& newSpeed){ msgPopupSpeed = newSpeed; }
    inline void setScrollSpeed(const float& newSpeed){ msgScrollSpeed = newSpeed; }
    inline void setScrollable(const bool& status){ bCanScroll = status; }

    inline void setUserEntrySource(CVTypeBox* entrySource){ usrEntryBox = entrySource; }

    inline void disableWidthTextWrap(){ maxPanelWidthPct = NAN; }
    inline void setWidthTextWrap(const float& newPct){ maxPanelWidthPct = newPct; }
    inline void setTextPanelPadding(const float& newPx){ msgPanelPaddingPx = newPx; }

    inline void setUserTextAlign(const uint8_t& newAlignment){ userTextAlign = newAlignment; }
    inline void setOtherTextAlign(const uint8_t& newAlignment){ otherTextAlign = newAlignment; }

    inline void setInitialMsgLoadCount(const unsigned int& count){ initialMessageCount = count; }
    inline void setMessageLoadInc(const unsigned int& newInc){ messageLoadInc = newInc; }

    inline void setMessageAccept(const bool& status){ bMsgReady = status; }

    inline void setLogFile(const std::string& filename){
        logFile = filename;
        load();
    }

    CVISION_API void addLogEntry(const std::string& text, bool userEntry, float updateDelay = 0.1f);
    CVISION_API void addToLastEntry(const std::string& text, bool userEntry);
    CVISION_API void editLastEntry(const std::string& text, bool userEntry);

    inline void removeLastLogEntry(){ // Last in time is the beginning of the vector
        printLock.lock();
        times.removeTimePoint(0);
        textLog.erase(textLog.begin());
        textLogUserEntered.erase(textLogUserEntered.begin());
        printLock.unlock();
    }

    inline bool getLastUserInput(std::string& output){
        if(lastUserEntered.size() < 1) return false;
        output = lastUserEntered;
        lastUserEntered.clear();
        return true;
    }

    inline bool getLastOtherInput(std::string& output){
        if(lastOtherEntered.size() < 1) return false;
        output = lastOtherEntered;
        lastOtherEntered.clear();
        return true;
    }

    inline const sf::Color& getMsgPanelFillColor() const{ return colorTheme[2]; }
    inline const sf::Color& getMsgPanelBorderColor() const{ return colorTheme[3]; }
    inline const sf::Color& getIncomingMsgPanelFillColor() const{ return colorTheme[4]; }
    inline const sf::Color& getHighlightPanelFillColor() const{ return colorTheme[5]; }
    inline const sf::Color& getOtherTextColor() const{ return colorTheme[6]; }

    inline void setMsgPanelFillColor(const sf::Color& newColor){
        while(colorTheme.size() < 3) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[2] = newColor;
    }
    inline void setMsgPanelBorderColor(const sf::Color& newColor){
        while(colorTheme.size() < 4) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[3] = newColor;
    }
    inline void setIncomingMsgPanelFillColor(const sf::Color& newColor){
        while(colorTheme.size() < 5) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[4] = newColor;
    }
    inline void setHighlightPanelFillColor(const sf::Color& newColor){
        while(colorTheme.size() < 6) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[5] = newColor;
    }
    inline void setOtherTextColor(const sf::Color& newColor){
        while(colorTheme.size() < 7) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[6] = newColor;
    }

    inline unsigned int numVisibleMessages() const{ return displayText.size(); }
    inline unsigned int numTotalMessages() const{ return textLog.size() + waitingText.size(); }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API CVTextLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float& borderWidth, const uint8_t& animType = CV_OBJ_ANIM_SLIDE,
              const std::string& logFile = "", CVTypeBox* usrEntryBox = nullptr);

    CVISION_API CVTextLog(const CVTextLog& other);
    CVISION_API CVTextLog& operator=(const CVTextLog& other);

    ~CVTextLog();

protected:

    std::vector<std::string> textLog,
                waitingText;

    TimeLog     times;

    CVTypeBox*  usrEntryBox;

    std::string lastUserEntered,
                lastOtherEntered,
                logFile;

    unsigned int selectedLogIndex,
                    maxMessages,
                    initialMessageCount,
                    messageLoadInc;

    std::vector<bool> textLogUserEntered,
                        textLogUserWaiting;
    std::vector<float> updateAddDelay;

    std::mutex printLock;

    float timeWaiting,
        scrollOffsetY,
        msgPopupSpeed,
        msgScrollSpeed,
        maxPanelWidthPct,
        msgPanelPaddingPx;

    bool bClear,
        bMsgReady,
        bCanScroll;

    uint8_t animType,
            userTextAlign,
            otherTextAlign;

    std::vector<CVTextLogMsg*> msgPanels;
    rounded_rectangle scrollBar;

    textEntry templateTheme;

    CVISION_API void wrapLastMsg();
    CVISION_API void shiftMsgs(const float& dist);

};

class CVTypeBox: public CVTextBox{
private:

    std::vector<std::vector<unsigned int>> lineFrames;

protected:
    std::string typeString,
                bkgString,
                enteredString,
                suggested;

    float xPadding,
            timeLastBlink,
            blinkFreq,
            expandRate;

    bool bBlink,
        bTypeStringChanged,
        bAcceptSuggestions,
        bCanEdit,
        bModified,
        bEncode;

    uint8_t animType,
            textFitType;

    unsigned char expandType;
    sf::FloatRect initBounds;

    unsigned int cursorPos,
                typeFrameStart,
                typeFrameEnd,
                logTextIndex,
                selectionStart,
                selectionLineStart,
                selectionLineEnd;

    bool enterLine; // Allow info be entered with the enter key

    sf::RectangleShape cursor;

    CVTextLog* textLog;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setSize(const sf::Vector2f& newSize);
    CVISION_API void setSize(const float& x, const float& y);

    inline const bool& canEnterLine() const{ return enterLine; }

    inline sf::Vector2f getCursorPos() const{ return cursor.getPosition(); }
    inline sf::Vector2f getCursorSize() const{ return cursor.getSize(); }

    inline void setCursorBlinkFrequency(const float& newFreq){ blinkFreq = newFreq; }
    inline void setCursorBlinkAnim(const uint8_t& newAnim){ animType = newAnim; }
    inline void setTextWrap(const uint8_t& wrapType){ textFitType = wrapType; }
    inline void setTextExpand(const unsigned char& newParams){ expandType = newParams; }
    inline void setExpandRate(const float& newRate){ expandRate = newRate; }
    inline void setEncode(const bool& status = true){ bEncode = status; }

    CVISION_API void setFillColor(const sf::Color& newColor);
    inline void setFillColor(const int& r, const int& g, const int& b, const int& a = 255)
    {
        setFillColor(sf::Color(r, g, b, a));
    }
    CVISION_API void setColor(const sf::Color& newColor);
    inline void setColor(const int& r, const int& g, const int& b, const int& a = 255)
    {
        setColor(sf::Color(r, g, b, a));
    }

    inline void setKeyboardEntryStatus(bool status){ enterLine = status; }
    inline void enterString(){
        enteredString = typeString;
        selectionStart = UINT_MAX;
        typeString.clear();
        bTypeStringChanged = true;
        cursorPos = 0;
    }

    inline void setFontSize(const unsigned int& newSize){
        displayText.front().setCharacterSize(newSize);
    }

    inline void setTextLogTarget(CVTextLog* textLog){ this->textLog = textLog; }
    inline bool has_text_target() const{ return textLog != nullptr; }
    inline bool getTextLogString(const unsigned int& index, bool user = true){
        if(has_text_target()){
            std::string tmp;
            if(user) tmp = textLog->getUserLogText(index);
            else tmp = textLog->getOtherLogText(index);

            if(tmp.size() > 0){
                setTypeString(tmp);
                return true;
            }
            return false;
        }
        else return false;
    }
    inline bool modified() const{ return bModified; }

    inline void getEnteredString(std::string& output){
        output = enteredString;
        enteredString.clear();
    }

    CVISION_API void setAlignment(const uint8_t& alignment);

    inline void sendEnteredString(){
        if(enteredString.size() < 1) return;
        if(textLog != nullptr) textLog->addLogEntry(enteredString, true);
        enteredString.clear();
    }

    inline void setBackgroundString(const std::string& newString){ bkgString = newString; }
    CVISION_API void setTypeString(std::string newString);

    inline const std::string& getTypeString() const{ return typeString; }
    inline std::string getDisplayString() const{ return displayText.front().getString(); }
    inline const sf::Text& getTextElement() const{ return displayText.front(); }

    CVISION_API void make_suggestion(const std::string& text);  // Highlight suggested text ahead of the user's input
    CVISION_API void clear_suggestion();
    CVISION_API const bool& can_suggest() const;

    inline void enable_edit(){ bCanEdit = true; }
    inline void disable_edit(){ bCanEdit = false; }

    CVISION_API const std::string& current_suggestion() const;  // Get access to the currently suggested string
    CVISION_API std::string string_at_cursor() const;           // Get delimited text at or surrounding the current position of the cursor
    CVISION_API unsigned int cursor_string_position() const;    // Get the position of the cursor within the current word
    CVISION_API bool cursor_at_word_end() const;
    CVISION_API bool cursor_at_word_begin() const;

    inline void move(const sf::Vector2f& distance){
        CVTextBox::move(distance);
        cursor.move(distance);
        initBounds.left += distance.x;
        initBounds.top += distance.y;
    }
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    inline void setPosition(const sf::Vector2f& position){
        move(position - getPosition());
    }
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API CVTypeBox(CVView* View, const sf::Vector2f& position, const float width, const float height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float borderWidth, const uint8_t& animType = CV_CURSOR_ANIM_FADE,
              const uint8_t& textFitType = CV_TEXT_FIT_NONE,
              const unsigned char& expandType = CV_TEXT_EXPAND_NONE,
              const float& expandRate = 0.2f);

};

}

#endif // CVIS_TYPEIO