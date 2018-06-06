#pragma once

#ifndef CVIS_TYPEIO
#define CVIS_TYPEIO

#include <SFML/Graphics.hpp>

#include "cvision/cvis_time.hpp"
#include "cvision/cvis_base.hpp"

// Type box

#define CV_CURSOR_ANIM_BLINK            0_BIT
#define CV_CURSOR_ANIM_FADE             1_BIT

#define CV_TEXT_FIT_NONE                0_BIT
#define CV_TEXT_FIT_LATERAL             1_BIT
#define CV_TEXT_FIT_WRAP                2_BIT

// =============================================

namespace cvis{

class CVTypeBox;

class CVTextLog: public CVTextBox{
protected:
    AIALG::StringVector textLog,
                waitingText;

    TimeLog     times;

    CVTypeBox*  usrEntryBox;

    std::string lastUserEntered,
                lastOtherEntered,
                logFile;

    unsigned int selectedLogIndex;

    std::vector<bool> textLogUserEntered,
                        textLogUserWaiting;
    std::vector<float> updateAddDelay;

    float timeWaiting,
        scrollOffsetY,
        msgPopupSpeed,
        msgScrollSpeed;

    bool bClear;

    std::vector<sf::RectangleShape> msgPanels;
    sf::RectangleShape scrollBar;

    textEntry templateTheme;

public:

    bool save();
    bool load();

    inline void clear(){ bClear = true; } // Handle in this thread at next opportunity

    inline std::string getLogText(const unsigned int& index) const{
        if(index > textLog.size()) return std::string();
        return textLog[index]; }

    inline std::string getUserLogText(const unsigned int& index, bool lastEntered = true) const{
        unsigned int j = 0;
        if(lastEntered){
            for(size_t i = textLog.size(); i > 0; --i){
                if(textLogUserEntered[i-1]){
                    if(j == index) return textLog[i-1];
                    else ++j;
                }
            }

        }
        else{
            for(size_t i = 0; i < textLog.size(); ++i){
                if(textLogUserEntered[i]){
                    if(j == index) return textLog[i];
                    else ++j;
                }
            }
        }
        return std::string();
    }
    inline std::string getOtherLogText(const unsigned int& index, bool lastEntered = true) const{
        unsigned int j = 0;
        if(lastEntered){
            for(size_t i = textLog.size(); i > 0; --i){
                if(!textLogUserEntered[i-1]){
                    if(j == index) return textLog[i-1];
                    else ++j;
                }
            }

        }
        else{
            for(size_t i = 0; i < textLog.size(); ++i){
                if(!textLogUserEntered[i]){
                    if(j == index) return textLog[i];
                    else ++j;
                }
            }
        }
        return std::string();
    }

    inline void setPopupSpeed(const float& newSpeed){ msgPopupSpeed = newSpeed; }
    inline void setScrollSpeed(const float& newSpeed){ msgScrollSpeed = newSpeed; }

    inline void setUserEntrySource(CVTypeBox* entrySource){ usrEntryBox = entrySource; }

    inline void setLogFile(const std::string& filename){
        logFile = filename;
        load();
    }

    inline void addLogEntry(const std::string& text, bool userEntry, float updateDelay = 0.1f){
        waitingText.push_back(text);
        textLogUserWaiting.push_back(userEntry);
        updateAddDelay.push_back(updateDelay);
        if(userEntry) lastUserEntered = text;
        else lastOtherEntered = text;
    }

    inline void removeLastLogEntry(){ // Last in time is the beginning of the vector
        times.removeTimePoint(0);
        textLog.erase(textLog.begin());
        textLogUserEntered.erase(textLogUserEntered.begin());
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

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw();

    CVTextLog(view* View, const sf::Vector2f& position, const float& width, const float& height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float& borderWidth, const uint8_t& animType = CV_OBJ_ANIM_SLIDE,
              const std::string& logFile = "", CVTypeBox* usrEntryBox = nullptr);

};

class CVTypeBox: public CVTextBox{
protected:
    std::string typeString,
                bkgString,
                enteredString;

    float xPadding,
            timeLastBlink,
            blinkFreq;

    bool bHasFocus,
        bBlink,
        bWrapExpandDown;
    uint8_t animType, textFitType;

    unsigned int cursorPos,
                typeFrameStart,
                typeFrameEnd,
                logTextIndex,
                selectionStart;

    bool enterLine; // Allow info be entered with the enter key

    sf::RectangleShape cursor;

    CVTextLog* textLog;

public:

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw();

    inline const bool& canEnterLine() const{ return enterLine; }

    inline void setCursorBlinkFrequency(const float& newFreq){ blinkFreq = newFreq; }
    inline void setCursorBlinkAnim(const uint8_t& newAnim){ animType = newAnim; }
    inline void setTextWrap(const uint8_t& wrapType){ textFitType = wrapType; }
    inline void setTextWrapDir(const bool& dirDown){ bWrapExpandDown = dirDown; }

    inline void setKeyboardEntryStatus(bool status){ enterLine = status; }
    inline void enterString(){
        enteredString = typeString;
        selectionStart = UINT_MAX;
        typeString.clear();
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

    inline void getEnteredString(std::string& output){
        output = enteredString;
        enteredString.clear();
    }

    inline void sendEnteredString(){
        if(enteredString.size() < 1) return;
        if(textLog != nullptr) textLog->addLogEntry(enteredString, true);
        enteredString.clear();
    }

    inline void setBackgroundString(const std::string& newString){ bkgString = newString; }
    inline void setTypeString(std::string newString){
        if(textFitType != CV_TEXT_FIT_WRAP){
            for(size_t i = 0; i < newString.size();){
                if(newString[i] == '\n'){
                    if(((i > 0) && (newString[i-1] == ' ')) ||
                       (i < L-1) && (newString[i+1] == ' ')){
                        newString.erase(newString + i);
                    }
                    else{
                        newString[i] = ' ';
                        ++i;
                    }
                }
                else ++i;
            }
        }
        this->typeString = newString;
        selectionStart = UINT_MAX;
        typeFrameStart = 0;
        typeFrameEnd = newString.size();
        if(cursorPos >= newString.size()) cursorPos = newString.size()-1;
    }

    inline const std::string& getTypeString() const{ return typeString; }

    inline void move(const sf::Vector2f& distance){
        CVTextBox::move(distance);
        cursor.move(distance);
    }
    inline void setPosition(const sf::Vector2f& position){
        move(position - getPosition());
    }
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }


    CVTypeBox(view* View, const sf::Vector2f& position, const float width, const float height,
              const textEntry& textInfo, const sf::Color& fillColor, const sf::Color& borderColor,
              const float borderWidth, const uint8_t& animType = CV_CURSOR_ANIM_FADE,
              const uint8_t& textFitType = CV_TEXT_FIT_NONE);

};

}

#endif // CVIS_TYPEIO
