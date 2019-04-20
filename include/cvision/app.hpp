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

#ifndef CVIS_APP
#define CVIS_APP

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>

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

    sf::Context appGLContext;

    std::vector<std::string> unhandledEvents;

    std::chrono::duration<float> updateLatency;

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

    inline bool setContextActive(const bool& state = true)
    {
        return appGLContext.setActive(state);
    }

    unsigned int frameRate;
    float frameTime;

    float leftClickLatency;
    float rightClickLatency;

    inline const bool& isOpen() const
    {
        return running;
    }

    inline void setRunning(const bool& runState) noexcept
    {
        running = runState;
    }
    inline const bool getRunState() noexcept
    {
        return running;
    }

    FontManager fonts;
    ImageManager bitmaps;

    std::unordered_map<std::string, sf::Color> colors;
    std::unordered_map<std::string, std::string> font_panel;

    inline const std::string& getDefaultFont() const noexcept
    {
        return defaultFont;
    }

    CVISION_API const sf::Font* getTypeFont(const std::string& type) const;

    // App virtuals

    virtual uint8_t loadPackages() = 0; // Load data and media required for this app to run
    virtual uint8_t initMainLoop(uint8_t runMode) = 0; // Initiate a thread consisting of the main data update loop for this app
    virtual bool initUI() = 0; // Initiate the UI required for this app before main loop begins

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

    CVISION_API CVApp(unsigned int frameRate = 60,
           float leftClickLatency = 0.5,
           float rightClickLatency = 0.5,
           const std::string& defaultFont = "");

    CVISION_API ~CVApp();
};

}

#endif // CVIS_APP
