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

#ifndef CVIS_APP
#define CVIS_APP

#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include <SFML/Graphics.hpp>

#include "cvision/import.hpp"

#define CV_EXIT_SUCCESS                 0_BIT
#define CV_INIT_FAIL                    0_BIT
#define CV_INIT_SUCCESS                 1_BIT

namespace cvis
{

class CVView;

class CVISION_API CVApp
{
private:

    CVApp(const CVApp& other) = delete;
    CVApp& operator=(const CVApp& other) = delete;

protected:
    bool running;
    std::thread* mainUpdateThread;
    std::string defaultFont;
    std::mutex appSyncLock;

    std::vector<std::string> unhandledEvents;

#ifdef __APPLE__
    std::mutex eventLock;
#endif

    inline void lock()
    {
        appSyncLock.lock();
    }
    inline void unlock()
    {
        appSyncLock.unlock();
    }

public:

    std::vector<CVView*> viewList;
    std::vector<std::string> viewTags;
    std::vector<std::thread*> viewThreads;

    CVISION_API bool getEvent(std::string& output);
    CVISION_API void pushEvent(const std::string& tag);
    CVISION_API void clearEvents();

    unsigned int frameRate;
    float frameTime;

    float leftClickLatency;
    float rightClickLatency;

    inline const bool& isOpen() const
    {
        return running;
    }

    inline void setRunning(bool runState)
    {
        running = runState;
    }
    inline const bool getRunState()
    {
        return running;
    }

    FontManager fonts;
    ImageManager bitmaps;

    inline const std::string& getDefaultFont() const
    {
        return defaultFont;
    }

    // App virtuals

    virtual uint8_t loadPackages() = 0; // Load data and media required for this app to run
    virtual uint8_t initMainLoop(uint8_t runMode) = 0; // Initiate a thread consisting of the main data update loop for this app
    virtual bool initUI(unsigned int videoX, unsigned int videoY) = 0; // Initiate the UI required for this app before main loop begins

    inline CVView* getView(const std::string& tag)
    {
        unsigned int index = 0;
        for(auto viewTag : viewTags)
        {
            if(viewTag == tag)
            {
                return viewList[index];
            }
            ++index;
        }
        return nullptr;
    }
    inline bool viewExists(const std::string& tag)
    {
        for(auto viewTag : viewTags)
        {
            if(viewTag == tag)
            {
                return true;
            }
        }
        return false;
    }

    CVISION_API void initView(unsigned int x, unsigned int y,
                  std::string winName = "",
                  uint32_t style = sf::Style::Default);

    CVISION_API bool addView(CVView* View, const std::string& viewTag);

#ifdef __APPLE__
    // Send event polling to the main thread on OSX
    void handleViewEvents();
#endif

    CVISION_API void closeView(const unsigned int viewIndex);
    CVISION_API void closeView(const std::string& tag);
    CVISION_API void closeAll();

    CVISION_API void mainUpdate();

    explicit CVApp():
        running(false),
        mainUpdateThread(nullptr),
        defaultFont(),
        frameRate(60),
        frameTime(1.0f/60),
        leftClickLatency(0.5f),
        rightClickLatency(0.5f) { }
    explicit CVApp(unsigned int frameRate,
                   float leftClickLatency,
                   float rightClickLatency,
                   const std::string& defaultFont):
        running(false),
        mainUpdateThread(nullptr),
        defaultFont(defaultFont),
        frameRate(frameRate),
        frameTime(1.0f/frameRate),
        leftClickLatency(leftClickLatency),
        rightClickLatency(rightClickLatency) { }

    ~CVApp();
};

}

#endif // CVIS_APP
