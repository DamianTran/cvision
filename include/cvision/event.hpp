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

#ifndef CVIS_EVENT
#define CVIS_EVENT

#include <string>
#include <vector>

#include "cvision/lib.hpp"

#include <SFML/Graphics.hpp>

#include <EZC/algorithm.hpp>

#define CV_ZOOM_NONE                    0
#define CV_ZOOM_IN                      1
#define CV_ZOOM_OUT                     2
#define CV_ZOOM_RESET                   3
#define CV_ZOOM_IN_MAX                  4
#define CV_ZOOM_OUT_MAX                 5

#define CV_EVENT_OFF                    0b0         // Event ended
#define CV_EVENT_ON                     0b1         // Event began
#define CV_EVENT_LMB                    0b10        // Left mouse
#define CV_EVENT_RMB                    0b100       // Right mouse
#define CV_EVENT_KEY                    0b1000      // Key pressed - requires additional parameter to specify

#define CV_WHEEL_DIR_NONE               0b0
#define CV_WHEEL_DIR_VERTICAL           0b1
#define CV_WHEEL_DIR_HORIZONTAL         0b10

#define CV_KEY_LEFT                     1
#define CV_KEY_RIGHT                    2
#define CV_KEY_UP                       3
#define CV_KEY_DOWN                     4
#define CV_KEY_BACKSPACE                8
#define CV_KEY_TAB                      9
#define CV_KEY_RETURN                   10
#define CV_KEY_DELETE                   127

// Data transfer types ============================

#define CV_DATA_VOID                    0
#define CV_DATA_STRING                  1
#define CV_DATA_NUMERIC                 2

#define CV_CLASS_NONE                   0
#define CV_CLASS_CVIS                   1
#define CV_CLASS_SPRITE                 2
#define CV_CLASS_SHAPE                  3

namespace cvis
{

#ifdef __APPLE__
const static std::chrono::duration<float> VIEW_EVENT_WAIT(0.01f);
#endif

class CVElement;

struct CVISION_API CVData
{
    uint8_t infoType;
    unsigned char releaseCond;
    sf::Keyboard::Key releaseKey;

    char* data;
    size_t length;

    CVISION_API CVData(const CVData& other);
    CVISION_API CVData& operator=(const CVData& other);

    CVISION_API CVData(const void* data, const size_t& length, const uint8_t& infoType,
           const unsigned char& releaseCond, const sf::Keyboard::Key& releaseKey = sf::Keyboard::Unknown);
    CVISION_API ~CVData();

};

struct CVISION_API CVCaptureRef
{
    void* item;
    uint8_t type;
    unsigned char releaseCond;
    sf::Keyboard::Key releaseKey;

    CVCaptureRef():
        item(nullptr),
        type(CV_CLASS_NONE),
        releaseCond(0b0),
        releaseKey(sf::Keyboard::Unknown) { }

    CVISION_API friend bool operator==(const CVCaptureRef& capref, const CVCaptureRef& other);
    template<typename T> friend bool operator==(const CVCaptureRef& capref, const T& other)
    {
        return capref.item == (void*)&other;
    }

    template<typename T> void capture(T& item, const unsigned char& releaseCond = CV_EVENT_LMB,
                                      const sf::Keyboard::Key& releaseKey = sf::Keyboard::Unknown)
    {
        if(dynamic_cast<CVElement*>(&item) != nullptr) type = CV_CLASS_CVIS;
        else if(dynamic_cast<sf::Sprite*>(&item) != nullptr) type = CV_CLASS_SPRITE;
        else if(dynamic_cast<sf::Shape*>(&item) != nullptr) type = CV_CLASS_SHAPE;

        this->item = &item;
        this->releaseCond = releaseCond;
        this->releaseKey = releaseKey;
    }
};

class CVISION_API CVEvent
{
public:

    bool                LMBhold;
    bool                RMBhold;
    bool                LMBreleased;
    bool                RMBreleased;
    bool                viewClosed;
    bool                viewHasFocus;
    bool                mouseCaptured;
    bool                focusCaptured;
    bool                viewResized;
    bool                eventsProcessed;
    bool                keyPressed;
    bool                closeSignal;

    unsigned int        LMBholdFrames;
    unsigned int        RMBholdFrames;
    unsigned int        LMBreleaseFrames;
    unsigned int        RMBreleaseFrames;
    unsigned int        maxMouseTrace;          // Buffer length for mouse frame positions
    unsigned int        numFrameAvg;            // Number of frames to average the frame time over

    std::string keyLog;
    CVISION_API void clearKeys();       // Capture all keys
    CVISION_API bool keyIsPressed(const char& key);

    CVISION_API bool checkCond(const unsigned char& cond, const sf::Keyboard::Key& keyCond = sf::Keyboard::Unknown);  // Check if condition flags are met

    std::vector<CVData> transferData; // Transfer data between elements over multiple update cycles

    template<typename T> bool getData(T& output)
    {
        if(EZC::getTypeID(output) == EZC::getTypeID<std::string>())
        {
            for(size_t i = 0; i < transferData.size(); ++i)
            {
                if((transferData[i].infoType == CV_DATA_STRING) && checkCond(transferData[i].releaseCond))
                {
                    output = (char*)transferData[i].data;
                    transferData.erase(transferData.begin() + i);
                    return true;
                }
            }
        }
        return false;
    }
    template<typename T> void sendData(T* input, const size_t& length,
                                       const unsigned char& releaseCond,
                                       const sf::Keyboard::Key& releaseKey = sf::Keyboard::Unknown)
    {
        if(EZC::getTypeID(*input) == EZC::getTypeID<char>())
        {
            transferData.emplace_back(input, length, CV_DATA_STRING,
                                      releaseCond, releaseKey);
        }
    }

    CVISION_API void releaseData(const unsigned char& cond = 0b11111111);
    CVISION_API void tossData(); // Remove data if conditions are met and not collected

    float           timeLastKey;
    float           lastFrameTime;
    float           zoomDelta;
    float           avgFrameTime;
    float           LMBholdTime;
    float           RMBholdTime;
    float           LMBreleaseTime;
    float           RMBreleaseTime;

    unsigned char   zoomState;
    unsigned char   wheelDirection;

    inline float avgFrameRate() const
    {
        return 1.0f/avgFrameTime;
    }

    std::vector<CVCaptureRef> mouseCapturedShapes; // Captured shapes will trail mouse
    inline void releaseCapturedShapes()
    {
        mouseCapturedShapes.clear();
    }

    template<typename T> inline void mouse_capture(T& item)
    {
        for(auto& ref : mouseCapturedShapes)
        {
            if(ref == item) return;
        }
        mouseCapturedShapes.emplace_back();
        mouseCapturedShapes.back().capture(item);
    }
    template<typename T> inline bool isCaptured(const T& item) const
    {
        for(auto& CVref : mouseCapturedShapes)
        {
            if(CVref == item) return true;
        }
        return false;
    }
    template<typename T> inline void release_mouse_capture(const T& item)
    {
        for(size_t i = 0; i < mouseCapturedShapes.size();)
        {
            if(mouseCapturedShapes[i] == item)
            {
                mouseCapturedShapes.erase(mouseCapturedShapes.begin() + i);
            }
            else ++i;
        }
    }
    template<typename T> inline void freeze_capture(T& item)
    {
        if(isCaptured(item)) item.move(lastFrameMousePosition - currentMousePosition); // Counter motion
    }
    template<typename T> inline void releaseMouseCapture(const T& item)
    {
        for(size_t i = 0; i < mouseCapturedShapes.size(); ++i)
        {
            if(mouseCapturedShapes[i] == item)
            {
                mouseCapturedShapes.erase(mouseCapturedShapes.begin() + i);
                return;
            }
        }
    }
    inline bool noCapture() const
    {
        return mouseCapturedShapes.empty();
    }

    CVISION_API void moveCapturedShapes(); // Move captured shapes using mouse frame distance


    CVISION_API float mouseVelocity() const;
    CVISION_API float mouseMoveAngle() const;

    sf::Cursor::Type            awaitingCursorType;
    inline void setCursor(const sf::Cursor::Type& newType){ awaitingCursorType = newType; }

    sf::Vector2f                viewResizeScale;
    sf::Vector2f                LMBpressPosition;
    sf::Vector2f                RMBpressPosition;
    sf::Vector2f                lastFrameMousePosition;
    sf::Vector2f                currentMousePosition;
    sf::Vector2f                LMBreleasePosition;
    sf::Vector2f                RMBreleasePosition;
    sf::Vector2f                mouseWheelDelta;
    sf::Vector2f                lastFrameGlobalMousePosition;
    sf::Vector2f                lastLMBpressPosition;
    sf::Vector2f                lastRMBpressPosition;

    std::vector<sf::Vector2f>   mouseTraceBuffer; // Record of mouse positions for the last N frames
    std::vector<std::string>    drop_data;         // Enqueued drop data from the host OS

    inline bool hasDropData() const
    {
        return !drop_data.empty();
    }

    inline size_t dropDataCount() const
    {
        return drop_data.size();
    }

    inline bool take_data(const unsigned int& index)
    {
        if(index >= dropDataCount()) return false;
        drop_data.erase(drop_data.begin() + index);
        return true;
    }

    inline bool take_data(const std::string& match)
    {
        for(size_t i = 0; i < dropDataCount(); ++i)
        {
            if(drop_data[i] == match)
            {
                take_data(i);
                return true;
            }
        }
        return false;
    }

    inline void clearDropData()
    {
        drop_data.clear();
    }

    sf::FloatRect viewBounds,
    lastViewBounds;

    CVISION_API bool captureMouse(); // Steal mouse input from other items that may compete within a view (ie. overlapping widgets)
    CVISION_API bool captureFocus(); // Steal focus from other items that may compete within a view (ie. lower panels)
    CVISION_API void releaseFocus(); // Allow lower panels to capture focus
    CVISION_API bool focusFree();   // Check focus availability without stealing away from lower panels
    CVISION_API void reset();

    CVISION_API bool leftClick(const float& duration = 0.0f) const;
    CVISION_API bool rightClick(const float& duration = 0.0f) const;

    CVISION_API bool leftClickReleased(const float& duration = 0.0f) const;
    CVISION_API bool rightClickReleased(const float& duration = 0.0f) const;

    inline void close()
    {
        closeSignal = true;
    }
    inline const bool& closed() const
    {
        return closeSignal;
    }

#ifdef __APPLE__
    CVISION_API void transfer(const CVEvent& other);
#endif

    CVISION_API friend std::ostream& operator<<(std::ostream& output, const CVEvent& event);

    CVISION_API CVEvent();
    ~CVEvent() = default;
};

}

#endif // CVIS_EVENT
