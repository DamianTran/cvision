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

#include "cvision/event.hpp"
#include "cvision/element.hpp"
#include "cvision/algorithm.hpp"

namespace cvis
{

CVData::CVData(const CVData& other)
{
    infoType = other.infoType;
    releaseCond = other.releaseCond;
    releaseKey = other.releaseKey;
    length = other.length;

    data = new char[length+1];
    data[length] = '\0';
    memcpy(data, other.data, length);
}

CVData& CVData::operator=(const CVData& other)
{

    delete[] data;

    infoType = other.infoType;
    releaseCond = other.releaseCond;
    releaseKey = other.releaseKey;
    length = other.length;

    data = new char[length+1];
    data[length] = '\0';
    memcpy(data, other.data, length);

    return *this;

}

CVData::CVData(const void* data, const size_t& length, const uint8_t& infoType,
               const unsigned char& releaseCond, const sf::Keyboard::Key& releaseKey):
    infoType(infoType),
    releaseCond(releaseCond),
    releaseKey(releaseKey),
    length(length)
{
    this->data = new char[length+1];
    this->data[length] = '\0';
    memcpy(this->data, data, length);
}

CVData::~CVData()
{
    delete[] data;
}

bool operator==(const CVCaptureRef& capref, const CVCaptureRef& other)
{
    return capref.item == other.item;
}

bool CVEvent::captureMouse()
{
    if(viewHasFocus)
    {
        if(!mouseCaptured)
        {
            mouseCaptured = true;
            return true;
        }
        else return false;
    }
    return false;
}

bool CVEvent::captureFocus()
{
    if(viewHasFocus)
    {
        if(!focusCaptured)
        {
            focusCaptured = true;
            return true;
        }
        else return false;
    }
    return false;
}

void CVEvent::releaseFocus()
{
    focusCaptured = false;
}

bool CVEvent::checkCond(const unsigned char& cond, const sf::Keyboard::Key& keyCond)
{
    bool status = false;

    if(cond & CV_EVENT_LMB)
    {
        status |= LMBhold;
    }
    if(cond & CV_EVENT_RMB)
    {
        status |= RMBhold;
    }
    if(cond & CV_EVENT_KEY)
    {
        status |= sf::Keyboard::isKeyPressed(keyCond);
    }

    if(!(cond & CV_EVENT_ON)) status = !status;

    return status;
}

void CVEvent::moveCapturedShapes()
{
    for(auto& capref : mouseCapturedShapes)
    {
        if(capref.type == CV_CLASS_CVIS)
        {
            ((CVElement*)capref.item)->move(currentMousePosition - lastFrameMousePosition);
        }
        else if(capref.type == CV_CLASS_SPRITE)
        {
            ((sf::Sprite*)capref.item)->move(currentMousePosition - lastFrameMousePosition);
        }
        else if(capref.type == CV_CLASS_SHAPE)
        {
            ((sf::Shape*)capref.item)->move(currentMousePosition - lastFrameMousePosition);
        }
    }
}

bool CVEvent::leftClick(const float& duration) const
{
    return LMBholdTime >= duration;
}

bool CVEvent::rightClick(const float& duration) const
{
    return RMBholdTime >= duration;
}

bool CVEvent::leftClickReleased(const float& duration) const
{
    return LMBholdTime >= duration;
}

bool CVEvent::rightClickReleased(const float& duration) const
{
    return RMBholdTime >= duration;
}

bool CVEvent::focusFree()
{
    return !focusCaptured;
}

void CVEvent::clearKeys()
{
    keyLog.clear();
}

bool CVEvent::keyIsPressed(const char& key)
{
    for(auto& k : keyLog)
    {
        if(k == key) return true;
    }
    return false;
}

float CVEvent::mouseVelocity() const
{
    return getVelocity(mouseTraceBuffer[mouseTraceBuffer.size() - 2], currentMousePosition, avgFrameTime*2);
}

float CVEvent::mouseMoveAngle() const
{
    return get_angle(mouseTraceBuffer[mouseTraceBuffer.size() - 2], currentMousePosition);
}

void CVEvent::releaseData(const unsigned char& cond)
{
    for(size_t i = 0; i < transferData.size();)
    {
        if(transferData[i].releaseCond & cond)
        {
            transferData.erase(transferData.begin() + i);
        }
        else ++i;
    }
}

void CVEvent::tossData()
{
    for(size_t i = 0; i < transferData.size();)
    {
        if(checkCond(transferData[i].releaseCond))
        {
            transferData.erase(transferData.begin() + i);
        }
        else ++i;
    }
}

void CVEvent::reset()
{
    LMBhold = false;
    RMBhold = false;
    LMBreleased = false;
    RMBreleased = false;
    viewClosed = false;
    viewHasFocus = true;
    mouseCaptured = false;
    focusCaptured = false;
    viewResized = false;
    eventsProcessed = false;
    keyPressed = false;
    LMBholdFrames = 0;
    RMBholdFrames = 0;
    LMBreleaseFrames = 0;
    RMBreleaseFrames = 0;
    timeLastKey = 0.0f;
    lastFrameTime = 0.0f;
    zoomDelta = 0.0f;
    avgFrameTime = 0.0f;
    zoomState = CV_ZOOM_NONE;
    wheelDirection = CV_WHEEL_DIR_NONE;
    viewResizeScale = sf::Vector2f(1.0f,1.0f);
    LMBpressPosition = sf::Vector2f(0.0f,0.0f);
    RMBpressPosition = sf::Vector2f(0.0f,0.0f);
    LMBreleasePosition = sf::Vector2f(0.0f,0.0f);
    RMBreleasePosition = sf::Vector2f(0.0f,0.0f);
    lastFrameMousePosition = sf::Vector2f(0.0f,0.0f);
    currentMousePosition = sf::Vector2f(0.0f,0.0f);
    viewBounds = sf::FloatRect(0.0f,0.0f,0.0f,0.0f);
    lastViewBounds = viewBounds;
    keyLog.clear();
    mouseCapturedShapes.clear();
}

#ifdef __APPLE__
void CVEvent::transfer(const CVEvent& other)
{
    LMBhold = other.LMBhold;
    RMBhold = other.RMBhold;
    LMBreleased = other.LMBreleased;
    RMBreleased = other.RMBreleased;
    viewClosed = other.viewClosed;
    viewHasFocus = other.viewHasFocus;
    viewResized = other.viewResized;
    LMBholdFrames = other.LMBholdFrames;
    RMBholdFrames = other.RMBholdFrames;
    LMBreleaseFrames = other.LMBreleaseFrames;
    RMBreleaseFrames = other.RMBreleaseFrames;
    LMBpressPosition = other.LMBpressPosition;
    RMBpressPosition = other.RMBpressPosition;
    lastLMBpressPosition = other.lastLMBpressPosition;
    lastRMBpressPosition = other.lastRMBpressPosition;
    LMBreleasePosition = other.LMBreleasePosition;
    RMBreleasePosition = other.RMBreleasePosition;
    LMBholdTime = other.LMBholdTime;
    RMBholdTime = other.RMBholdTime;
    LMBreleaseTime = other.LMBreleaseTime;
    RMBreleaseTime = other.RMBreleaseTime;
    zoomDelta = other.zoomDelta;
    zoomState = other.zoomState;
    viewResizeScale = other.viewResizeScale;
    viewBounds = other.viewBounds;
    lastViewBounds = other.lastViewBounds;
    keyLog = other.keyLog;
    transferData = other.transferData;
    mouseCapturedShapes = other.mouseCapturedShapes;
    mouseTraceBuffer = other.mouseTraceBuffer;
    wheelDirection = other.wheelDirection;
}
#endif

CVEvent::CVEvent():
    LMBhold(false),
    RMBhold(false),
    LMBreleased(false),
    RMBreleased(false),
    viewClosed(false),
    viewHasFocus(true),
    mouseCaptured(false),
    focusCaptured(false),
    viewResized(false),
    eventsProcessed(false),
    keyPressed(false),
    closeSignal(false),
    LMBholdFrames(0),
    RMBholdFrames(0),
    LMBreleaseFrames(0),
    RMBreleaseFrames(0),
    maxMouseTrace(14),
    numFrameAvg(60),
    timeLastKey(0.0f),
    lastFrameTime(0.0f),
    zoomDelta(0.0f),
    avgFrameTime(0.0f),
    LMBholdTime(0.0f),
    RMBholdTime(0.0f),
    LMBreleaseTime(0.0f),
    RMBreleaseTime(0.0f),
    zoomState(CV_ZOOM_NONE),
    wheelDirection(CV_WHEEL_DIR_NONE),
    viewResizeScale(1.0f,1.0f),
    LMBpressPosition(0.0f,0.0f),
    RMBpressPosition(0.0f,0.0f),
    lastFrameMousePosition(0.0f,0.0f),
    currentMousePosition(0.0f,0.0f),
    LMBreleasePosition(0.0f,0.0f),
    RMBreleasePosition(0.0f,0.0f),
    mouseWheelDelta(0.0f,0.0f),
    lastFrameGlobalMousePosition(0.0f,0.0f),
    lastLMBpressPosition(0.0f, 0.0f),
    lastRMBpressPosition(0.0f, 0.0f),
    mouseTraceBuffer(maxMouseTrace, sf::Vector2f(0.0f, 0.0f)),
    viewBounds(0.0f,0.0f,0.0f,0.0f),
    lastViewBounds(viewBounds)
{ }

std::ostream& operator<<(std::ostream& output, const CVEvent& event)
{
    output << "LMB pressed: " << event.LMBhold <<
           "\nLMB hold frames: " << event.LMBholdFrames <<
           "\nLMB released: " << event.LMBreleased <<
           "\nLMB release frames: " << event.LMBreleaseFrames <<
           "\nRMB pressed: " << event.RMBhold <<
           "\nRMB hold frames: " << event.RMBholdFrames <<
           "\nRMB released: " << event.RMBreleased <<
           "\nRMB release frames: " << event.RMBreleaseFrames <<
           "\nView closed: " << event.viewClosed <<
           "\nView has focus: " << event.viewHasFocus <<
           "\nMouse captured: " << event.mouseCaptured <<
           "\nView resized: " << event.viewResized <<
           "\nEvents processed: " << event.eventsProcessed <<
           "\nTime last key: " << event.timeLastKey <<
           "\nLast frame time: " << event.lastFrameTime <<
           "\nZoom delta: " << event.mouseWheelDelta.y <<
           "\nZoom state: " << (int)event.zoomState <<
           "\nKey log: " << event.keyLog <<
           "\nView resize scale: " << event.viewResizeScale.x << ' ' << event.viewResizeScale.y <<
           "\nLMB press position: " << event.LMBpressPosition.x << ' ' << event.LMBpressPosition.y <<
           "\nRMB press position: " << event.RMBpressPosition.x << ' ' << event.RMBpressPosition.y <<
           "\nLast frame mouse pos: " << event.lastFrameMousePosition.x << ' ' << event.lastFrameMousePosition.y <<
           "\nCurrent frame mouse pos: " << event.currentMousePosition.x << ' ' << event.currentMousePosition.y;
    return output;
}


}
