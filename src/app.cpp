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

#include "cvision/app.hpp"
#include "cvision/view.hpp"

#include <EZC/algorithm.hpp>

using namespace EZC;

namespace cvis
{

bool CVApp::getEvent(std::string& output)
{

    if(!unhandledEvents.empty())
    {

        output = unhandledEvents.front();
        unhandledEvents.erase(unhandledEvents.begin());

        return true;

    }

    return false;

}

void CVApp::pushEvent(const std::string& tag)
{

    unhandledEvents.emplace_back(tag);

}

void CVApp::clearEvents()
{

    unhandledEvents.clear();

}

#ifdef __APPLE__
void CVApp::handleViewEvents()
{
    this->eventLock.lock();
    for(auto& view : viewList)
    {
        if(!view->eventTrace.eventsProcessed)
        {
            view->updateLock.lock();
            view->handleViewEvents(view->eventTrace);
            view->updateLock.unlock();
        }
    }
    this->eventLock.unlock();
}
#endif

void CVApp::mainUpdate()
{

#ifdef __APPLE__
    for(auto& view : viewList)
    {

        if(view->bWindowCreateWaiting)
        {

            sf::ContextSettings newWinSettings;
            view->viewPort->
            create(sf::VideoMode(view->width, view->height),
                   view->name, view->style, newWinSettings);
            view->viewPort->setActive(true);

            view->bWindowCreateWaiting = false;

        }

    }
#endif

}

void CVApp::closeView(const unsigned int viewIndex)
{
    viewList[viewIndex]->close();

    viewList[viewIndex]->appThread->join();

    for(size_t i = 0; i < viewThreads.size(); ++i)
    {
        if(viewThreads[i] == viewList[viewIndex]->appThread)
        {
            delete(viewThreads[i]);
            viewThreads.erase(viewThreads.begin() + i);
        }
    }

    delete(viewList[viewIndex]);
    viewList.erase(viewList.begin() + viewIndex);
    viewTags.erase(viewTags.begin() + viewIndex);
}

void CVApp::initView(unsigned int x, unsigned int y,
                  std::string winName, uint32_t style)
{
    if(winName.size() < 1) winName = "View " + std::to_string(viewList.size());
    viewList.push_back(new CVView(x, y, winName, style, this));
    viewTags.push_back(winName);
}

void CVApp::closeView(const std::string& tag)
{
    unsigned int index = 0;
    for(auto& word : viewTags)
    {
        if(word == tag)
        {
            closeView(index);
            return;
        }
        ++index;
    }
}

void CVApp::closeAll()
{
    while(!viewList.empty())
    {
        closeView(0);
    }
}

CVApp::~CVApp()
{
    running = false;
    closeAll();
    if(mainUpdateThread) mainUpdateThread->join();
}

bool CVApp::addView(CVView* View, const std::string& viewTag)
{

    if(!anyEqual(View, viewList))
    {

        viewList.push_back(View);

        if(EZC::anyEqual(viewTag, viewTags))
        {

            unsigned int nameIndex = viewTags.size();
            std::string newViewTag;

            do
            {
                newViewTag = viewTag + "_" + std::to_string(nameIndex);
                ++nameIndex;
            }
            while(anyEqual(newViewTag, viewTags));

            viewTags.emplace_back(newViewTag);

        }
        else
        {

            viewTags.push_back(viewTag);

        }

        return true;

    }

    else return false;

}

}
