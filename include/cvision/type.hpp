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

#ifndef CVIS_TYPEIO
#define CVIS_TYPEIO

#include <SFML/Graphics.hpp>

#include <hyper/toolkit/time.hpp>

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

namespace cvis
{

class CVTypeBox;
class CVScrollBar;

class CVISION_API CVTextLogMsg : public CVTextBox
{
public:

    CVISION_API CVTextLogMsg(CVView* View,
                             const TextEntry& newTheme = TextEntry("", ""));

    bool        bUser;
    bool        bMsg;

    hyperC::TimePoint   time;

    size_t      textLogIndex;

};

class CVISION_API CVTextLog: public CVTextBox
{
public:

    CVISION_API CVTextLog(CVView* View, const sf::Vector2f& position, const float& width, const float& height,
                          const TextEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
                          const float& borderWidth, const uint8_t& animType = CV_OBJ_ANIM_SLIDE,
                          const std::string& logFile = "", CVTypeBox* usrEntryBox = nullptr);

    CVISION_API ~CVTextLog();

    CVISION_API bool save();
    CVISION_API bool load();

    CVISION_API std::string getReadableLog() const; // Print an expanded, readable log

    inline void clear()
    {
        bClear = true;    // Handle in this thread at next opportunity
    }

    inline std::string getLogText(const unsigned int& index) const
    {
        if(index > textLog.size()) return std::string();
        return textLog[index];
    }

    CVISION_API std::string getUserLogText(const unsigned int& index, bool lastEntered = true) const;
    CVISION_API std::string getOtherLogText(const unsigned int& index, bool lastEntered = true) const;

    CVISION_API void replaceLastMessage(const std::string& search,
                                        const std::string& substitute,
                                        bool user = true); // Substitute string in last message (ie. password obfuscation)

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& distance);
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y)
    {
        setSize(sf::Vector2f(x, y));
    }

    inline void setPopupSpeed(const float& newSpeed)
    {
        msgPopupSpeed = newSpeed;
    }
    inline void setScrollSpeed(const float& newSpeed)
    {
        msgScrollSpeed = newSpeed;
    }
    inline void setScrollable(const bool& status)
    {
        bCanScroll = status;
    }

    inline void setUserEntrySource(CVTypeBox* entrySource)
    {
        usrEntryBox = entrySource;
    }

    inline void disableWidthTextWrap()
    {
        maxPanelWidthPct = NAN;
    }
    inline void setWidthTextWrap(const float& newPct)
    {
        maxPanelWidthPct = newPct;
    }
    inline void setTextPanelPadding(const float& newPx)
    {
        msgPanelPaddingPx = newPx;
    }

    inline void setUserTextAlign(const uint8_t& newAlignment)
    {
        userTextAlign = newAlignment;
    }
    inline void setOtherTextAlign(const uint8_t& newAlignment)
    {
        otherTextAlign = newAlignment;
    }

    inline void setInitialMsgLoadCount(const unsigned int& count)
    {
        initialMessageCount = count;
    }
    inline void setMessageLoadInc(const unsigned int& newInc)
    {
        messageLoadInc = newInc;
    }

    inline void setMessageAccept(const bool& status)
    {
        bMsgReady = status;
    }

    inline void setLogFile(const std::string& filename)
    {
        logFile = filename;
        load();
    }

    CVISION_API void addLogEntry(const std::string& text, bool userEntry, float updateDelay = 0.1f);
    CVISION_API void addToLastEntry(const std::string& text, bool userEntry);
    CVISION_API void editLastEntry(const std::string& text, bool userEntry);

    inline void removeLastLogEntry()  // Last in time is the beginning of the vector
    {
        printLock.lock();
        times.removeTimePoint(0);
        textLog.erase(textLog.begin());
        textLogUserEntered.erase(textLogUserEntered.begin());
        printLock.unlock();
    }

    inline bool getLastUserInput(std::string& output)
    {
        if(lastUserEntered.size() < 1) return false;
        output = lastUserEntered;
        lastUserEntered.clear();
        return true;
    }

    inline bool getLastOtherInput(std::string& output)
    {
        if(lastOtherEntered.size() < 1) return false;
        output = lastOtherEntered;
        lastOtherEntered.clear();
        return true;
    }

    inline const sf::Color& getMsgPanelFillColor() const
    {
        return colorTheme[2];
    }
    inline const sf::Color& getMsgPanelBorderColor() const
    {
        return colorTheme[3];
    }
    inline const sf::Color& getIncomingMsgPanelFillColor() const
    {
        return colorTheme[4];
    }
    inline const sf::Color& getHighlightPanelFillColor() const
    {
        return colorTheme[5];
    }
    inline const sf::Color& getOtherTextColor() const
    {
        return colorTheme[6];
    }

    inline void setTextColor(const sf::Color& newColor)
    {
        CVTextBox::setTextColor(newColor);
        templateTheme.textColor = newColor;
    }
    inline void setMsgPanelFillColor(const sf::Color& newColor)
    {
        while(colorTheme.size() < 3) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[2] = newColor;
    }
    inline void setMsgPanelBorderColor(const sf::Color& newColor)
    {
        while(colorTheme.size() < 4) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[3] = newColor;
    }
    inline void setIncomingMsgPanelFillColor(const sf::Color& newColor)
    {
        while(colorTheme.size() < 5) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[4] = newColor;
    }
    inline void setHighlightPanelFillColor(const sf::Color& newColor)
    {
        while(colorTheme.size() < 6) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[5] = newColor;
    }
    inline void setOtherTextColor(const sf::Color& newColor)
    {
        while(colorTheme.size() < 7) colorTheme.emplace_back(sf::Color::Black);
        colorTheme[6] = newColor;
    }

    inline unsigned int numVisibleMessages() const
    {
        return displayText.size();
    }
    inline unsigned int numTotalMessages() const
    {
        return textLog.size() + waitingText.size();
    }

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

protected:

    std::vector<std::string>    textLog;
    std::vector<std::string>    waitingText;

    hyperC::TimeLog                times;

    CVTypeBox*                  usrEntryBox;

    std::string                 lastUserEntered;
    std::string                 lastOtherEntered;
    std::string                 logFile;

    unsigned int                selectedLogIndex;
    unsigned int                maxMessages;
    unsigned int                initialMessageCount;
    unsigned int                messageLoadInc;

    std::vector<bool>           textLogUserEntered;
    std::vector<bool>           textLogUserWaiting;

    std::vector<float>          updateAddDelay;

    std::mutex                  printLock;

    float                       timeWaiting;
    float                       msgPopupSpeed;
    float                       msgScrollSpeed;
    float                       maxPanelWidthPct;
    float                       msgPanelPaddingPx;

    bool                        bClear;
    bool                        bMsgReady;
    bool                        bCanScroll;

    uint8_t                     animType;
    uint8_t                     userTextAlign;
    uint8_t                     otherTextAlign;

    std::vector<CVTextLogMsg*>  msgPanels;

    CVTextLogMsg*               selectedEntry;
    CVScrollBar*                scroll_bar;

    CVISION_API float scrollOffsetY() const noexcept;

    TextEntry                   templateTheme;

    CVISION_API void shiftMsgs(const float& dist);

};

class CVTypeBox: public CVTextBox
{
public:

    CVISION_API CVTypeBox(CVView* View, const sf::Vector2f& position, const float width, const float height,
                          const TextEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
                          const float borderWidth, const uint8_t& animType = CV_CURSOR_ANIM_FADE,
                          const uint8_t& textFitType = CV_TEXT_FIT_NONE,
                          const unsigned char& expandType = CV_TEXT_EXPAND_NONE,
                          const float& expandRate = 0.2f);
    CVISION_API ~CVTypeBox();

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

    CVISION_API void setSize(const sf::Vector2f& newSize);
    CVISION_API void setSize(const float& x, const float& y);

    inline const bool& canEnterLine() const
    {
        return bEnterLine;
    }
    inline void setEnterLine(const bool& state)
    {
        bEnterLine = state;
    }

    inline sf::Vector2f getCursorPos() const
    {
        return cursor.getPosition();
    }
    inline sf::Vector2f getCursorSize() const
    {
        return cursor.getSize();
    }

    inline void setCursorBlinkFrequency(const float& newFreq)
    {
        blinkFreq = newFreq;
    }
    inline void setCursorBlinkAnim(const uint8_t& newAnim)
    {
        animType = newAnim;
    }
    inline void setTextWrap(const uint8_t& wrapType)
    {
        textFitType = wrapType;
    }
    inline void setTextExpand(const unsigned char& newParams)
    {
        expandType = newParams;
    }
    inline void setExpandRate(const float& newRate)
    {
        expandRate = newRate;
    }
    inline void setEncode(const bool& status = true)
    {
        bEncode = status;
    }
    inline void setInnerPadding(const float& newPadding)
    {
        xPadding = newPadding;
    }

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

    inline void setKeyboardEntryStatus(const bool& status)
    {
        bEnterLine = status;
    }
    inline void setClearOnKeyboardEntry(const bool& state = true)
    {
        bClearOnEnterLine = state;
    }

    CVISION_API void enterString();

    inline void setFontSize(const unsigned int& newSize)
    {
        displayText.front().setCharacterSize(newSize);
    }

    inline void setTextLogTarget(CVTextLog* textLog)
    {
        this->textLog = textLog;
    }
    inline bool has_text_target() const
    {
        return textLog != nullptr;
    }
    inline bool getTextLogString(const unsigned int& index, bool user = true)
    {
        if(has_text_target())
        {
            std::string tmp;
            if(user) tmp = textLog->getUserLogText(index);
            else tmp = textLog->getOtherLogText(index);

            if(tmp.size() > 0)
            {
                setTypeString(tmp);
                return true;
            }
            return false;
        }
        else return false;
    }
    inline bool modified() const
    {
        return bModified;
    }

    inline void getEnteredString(std::string& output)
    {
        output = enteredString;
        enteredString.clear();
    }
    inline bool checkStringEntered() const noexcept{ return !enteredString.empty(); }

    inline bool checkTypeStringChanged() noexcept
    {
        if(bUserTyped)
        {
            bUserTyped = false;
            return true;
        }
        return false;
    }

    CVISION_API void setAlignment(const uint8_t& alignment);

    inline void sendEnteredString()
    {
        if(enteredString.size() < 1) return;
        if(textLog != nullptr) textLog->addLogEntry(enteredString, true);
        enteredString.clear();
    }

    CVISION_API void setBackgroundString(const std::string& newString);
    CVISION_API void setTypeString(const std::string& newString);
    CVISION_API void setTypeString(std::wstring newString);
    CVISION_API void clearTypeString();

    CVISION_API std::string getTypeString() const;
    inline std::string getDisplayString() const
    {
        return displayText.front().getString();
    }
    inline const sf::Text& getTextElement() const
    {
        return displayText.front();
    }

    CVISION_API void make_suggestion(const std::string& text);  // Highlight suggested text ahead of the user's input
    CVISION_API void clear_suggestion();
    CVISION_API const bool& can_suggest() const;

    inline void enable_edit()
    {
        bCanEdit = true;
    }
    inline void disable_edit()
    {
        bCanEdit = false;
    }

    CVISION_API const std::string& current_suggestion() const;  // Get access to the currently suggested string
    CVISION_API std::string string_at_cursor() const;           // Get delimited text at or surrounding the current position of the cursor
    CVISION_API unsigned int cursor_string_position() const;    // Get the position of the cursor within the current word
    CVISION_API bool cursor_at_word_end() const;
    CVISION_API bool cursor_at_word_begin() const;

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

    inline void setPosition(const sf::Vector2f& position)
    {
        move(position - getPosition());
    }
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    inline void setJumpTarget(CVElement* target)
    {
        jumpTarget = target;
    }

    inline CVElement* getJumpTarget()
    {
        return jumpTarget;
    }

    inline void clearJumpTarget()
    {
        jumpTarget = nullptr;
    }

protected:

    std::wstring         typeString;
    std::wstring         bkgString;

    std::string         enteredString;
    std::string         suggested;

    float               xPadding;
    float               timeLastBlink;
    float               blinkFreq;
    float               expandRate;

    bool                bBlink;
    bool                bTypeStringChanged;
    bool                bAcceptSuggestions;
    bool                bCanEdit;
    bool                bModified;
    bool                bEncode;
    bool                bEnterLine;             // Allow info be entered with the enter key
    bool                bClearOnEnterLine;      // Should the field be cleared if data is entered?
    bool                bUserTyped;             // Flag for external check for a type action

    uint8_t             animType;
    uint8_t             textFitType;

    unsigned char       expandType;

    sf::FloatRect       initBounds;

    unsigned int        cursorPos;
    unsigned int        typeFrameStart;
    unsigned int        typeFrameEnd;
    unsigned int        logTextIndex;
    unsigned int        selectionStart;
    unsigned int        selectionLineStart;
    unsigned int        selectionLineEnd;

    sf::RectangleShape  cursor;

    CVTextLog*          textLog;
    CVElement*          jumpTarget; // Target to send focus to on tab press if applicable

private:

    std::vector<std::vector<unsigned int>> lineFrames;

};

}

#endif // CVIS_TYPEIO
