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

#include "cvision/view.hpp"
#include "cvision/app.hpp"
#include "cvision/viewpanel.hpp"

#include <boost/range/adaptors.hpp>

#if defined _WIN32 || defined _WIN32 || defined WIN32
#include <windows.h>
#elif defined __APPLE__
#include <Foundation/Foundation.h>
#include <AvailabilityMacros.h>
#import <Cocoa/Cocoa.h>
#endif

namespace cvis
{

CVView::CVView(unsigned int x, unsigned int y, std::string winName,
               uint32_t style, CVApp* mainApp,
               const sf::Vector2f& screenPosition,
               const sf::Color& backgroundColor):
    forceClose(false),
    bClosed(false),
    bElasticSelect(false),
    bWindowCreateWaiting(false),
    defaultViewScale(1920.0f*1080.0f),
    backgroundColor(backgroundColor),
    numPendingAnims(0),
    name(winName),
    style(style),
    appThread(nullptr),
    frameRateLimit(mainApp->frameRate),
    frameRate(0.0f),
    width(x), height(y),
    titleBarHeight(0.0f),
    viewHeight(height),
    resizeTarget(width, height),
    resizeSpeed(0,0),
    velocity(0,0),
    viewState(VIEW_STATE_STARTUP),
    viewPort(nullptr),
    mainApp(mainApp),
    tag(winName)
{

#ifdef __APPLE__
    // Construct the viewPort outside of the worker thread for OSX compatibility
    viewPort = new sf::RenderWindow();
    viewPort->setActive(false);
    viewPort->setVerticalSyncEnabled(true);
#endif

    textureBuffer.setActive();

    mainApp->viewThreads.emplace_back(new std::thread([&, x, y, winName, style, screenPosition]()
    {

        const float frameTime = 0.5f/frameRateLimit;

#ifndef __APPLE__
        viewPort = new sf::RenderWindow();
        viewPort->setVerticalSyncEnabled(true);
#endif

        moveTarget = viewPort->getPosition();
        sf::Vector2i intPos;

        std::chrono::high_resolution_clock::time_point t0 = TIME_NOW;
        const std::chrono::duration<float> frameUpdateLatency(frameTime);

        std::chrono::duration<float> duration;

        eventTrace.viewBounds = sf::FloatRect(moveTarget.x, moveTarget.y, x, y);
        eventTrace.lastViewBounds = eventTrace.viewBounds;
        eventTrace.lastFrameMousePosition =
            sf::Vector2f(sf::Mouse::getPosition(*viewPort));

        while(!bClosed)
        {

#ifdef __APPLE__
            drawLock.lock();
#endif
            if(!bClosed && viewPort && !viewPort->isOpen())
            {
                if(getState() == VIEW_STATE_MAIN)
                {
                    if(!isnan(screenPosition.x) && !isnan(screenPosition.y)) viewPort->setPosition(sf::Vector2i(screenPosition));
#ifndef __APPLE__
                    sf::ContextSettings newWinSettings;
                    newWinSettings.antialiasingLevel = 4;
                    viewPort->create(sf::VideoMode(x, y), winName, style, newWinSettings);
                    viewPort->setActive(true);
#endif
                }
                else
                {
#ifdef __APPLE__
                    drawLock.unlock();
#endif
                    goto nextFrame;
                }
            }
            else if(bClosed)
            {
#ifdef __APPLE__
                drawLock.unlock();
#endif
                break;

            }

#ifdef __APPLE__
            drawLock.unlock();
#endif

            if(forceClose)
            {
#ifndef __APPLE__
                viewPort->close();
#endif
                break;
            }

#ifdef __APPLE__
            while(!bClosed && !eventTrace.eventsProcessed)
            {
                std::this_thread::sleep_for(std::chrono::duration<float>(frameTime/10));
            } // Wait for main thread to process events
#endif // __APPLE__

            updateLock.lock();

            if(!bClosed && viewPort && viewPort->isOpen())
            {
                intPos = sf::Mouse::getPosition(*viewPort);
                mousePos.x = intPos.x;
                mousePos.y = intPos.y;
                update(eventTrace, mousePos);
            }

            updateLock.unlock();

            drawLock.lock();

            if(!bClosed && viewPort && viewPort->isOpen())
            {
                viewPort->clear(backgroundColor);
                draw(viewPort);
                viewPort->display();
            }

            drawLock.unlock();

            // Framerate limit cycle

nextFrame:
            ;

            duration = TIME_NOW - t0;

            if(duration < frameUpdateLatency)
            {
                std::this_thread::sleep_for(std::chrono::duration<float>(frameTime - duration.count()));
            }

            eventTrace.lastFrameTime = std::chrono::duration<float>(TIME_NOW - t0).count();
            t0 = TIME_NOW;
            frameRate = 1.0f/eventTrace.avgFrameTime;

        }

#ifdef __APPLE__
        // Wait for main thread to handle viewport close
//            while(viewPort && viewPort->isOpen()){
//                std::this_thread::sleep_for(std::chrono::duration<float>(frameTime));
//            }
#else
        if(viewPort->isOpen())
        {
            viewPort->close();
        }
#endif
        viewState = VIEW_STATE_CLOSED;

    }));
    appThread = mainApp->viewThreads.back(); // Establish reference to app thread for this view

#ifndef __APPLE__
    // Wait for the view to be constructed
    while(viewPort == nullptr)
    {
        std::this_thread::sleep_for(std::chrono::duration<float>(0.01f));
    }
#endif
}

CVElement* CVView::getElementById(const std::string& tag)
{

    for(auto& panel : viewPanels)
    {
        if(panel->tag() == tag)
        {
            return panel;
        }

        for(auto& element : panel->getElements())
        {
            if(element->tag() == tag)
            {
                return element;
            }
        }

    }

    return nullptr;
}

void CVView::setTopMargin(const float& newMargin)
{
    titleBarHeight = newMargin;
    viewHeight = height - titleBarHeight;
}

float CVView::getHeight() const
{
    return viewHeight;
}

float CVView::getTitleBarHeight() const
{
    return titleBarHeight;
}

float CVView::renderWidth() const
{
    return width;
}
float CVView::renderHeight() const
{
    return height;
}

sf::Vector2f CVView::getSize()
{
    return sf::Vector2f(width, viewHeight);
}

sf::FloatRect CVView::getBounds() const
{
    return sf::FloatRect(0.0f,titleBarHeight,width,viewHeight);
}

void CVView::getTexture(sf::Texture& texture)
{
    captureLock.lock();

    textureBuffer.create(width, height);
    textureBuffer.setView(sf::View(getBounds()));
    textureBuffer.clear();

    draw(&textureBuffer);

    textureBuffer.display();
    texture = textureBuffer.getTexture();

    captureLock.unlock();
}

void CVView::setVisible(const unsigned int index, bool newVisibleState)
{
    viewPanels[index]->setVisible(newVisibleState);
}

void CVView::isolateVisible(const unsigned int index)  // Focus visibility on one panel in view
{
    size_t L = viewPanels.size();
    for(size_t i = 0; i < L; ++i)
    {
        if(i == index) viewPanels[i]->setVisible(true);
        else viewPanels[i]->setVisible(false);
    }
}

void CVView::setState(uint8_t newState)
{

    if((newState == VIEW_STATE_MAIN) &&
            (viewState != newState))
    {

        bWindowCreateWaiting = true;

    }

    viewState = newState;

}

bool CVView::contains(const CVElement& element)
{
    sf::FloatRect viewBounds(0.0f,0.0f,width,height);
    return viewBounds.intersects(element.getBounds());
}

CVViewPanel* CVView::getTaggedPanel(const std::string s)
{
    return *taggedItem(s, viewPanels, panelTags);
}
CVViewPanel* CVView::operator[](const std::string s)
{
    return *taggedItem(s, viewPanels, panelTags);
}

CVView* CVView::taggedSubView(std::string tag)
{
    return *taggedItem(tag, subViews, subViewTags);
}

const float CVView::getViewScale() const
{
    return pow(sf::VideoMode::getDesktopMode().height*
               sf::VideoMode::getDesktopMode().width/
               defaultViewScale, 1.0f/3);
}
void CVView::setDefaultViewScale(const float& x, const float& y)
{
    defaultViewScale = x*y;
}

void CVView::addSubView(CVView* newSubView, std::string tag)
{
    if(addUnique(newSubView, subViews))
    {
        if(tag.size() > 0) subViewTags.push_back(tag);
        else subViewTags.push_back("Subview " + std::to_string(subViews.size()));
    }
}

void CVView::addPanel(CVViewPanel* newPanel, std::string tag, const unsigned int& index)
{
    if(addUnique(newPanel, viewPanels, index))
    {
        if(tag.size() > 0)
        {
            panelTags.push_back(tag);
            newPanel->setTag(tag);
        }
        else{
            newPanel->setTag("Panel " + std::to_string(viewPanels.size()));
            panelTags.push_back(newPanel->tag());
        }
    }
}

void CVView::minimize()
{
#if defined WIN32 || defined _WIN32 || defined __WIN32
    ShowWindow(viewPort->getSystemHandle(), SW_MINIMIZE);
#elif defined __APPLE__
    const NSWindow* viewWindow = (NSWindow*)viewPort->getSystemHandle();
    const id sender = viewWindow;
    [viewWindow miniaturize:sender];
#endif
}

void CVView::maximize()
{
#if defined WIN32 || defined _WIN32 || defined __WIN32
    ShowWindow(viewPort->getSystemHandle(), SW_MAXIMIZE);
#elif defined __APPLE__
    const NSWindow* viewWindow = (NSWindow*)viewPort->getSystemHandle();
    const id sender = viewWindow;

    viewWindow.collectionBehavior |= NSWindowCollectionBehaviorFullScreenPrimary;

    [viewWindow toggleFullScreen:sender];
#endif
}

void CVView::restore()
{
#if defined WIN32 || defined _WIN32 || defined __WIN32
    ShowWindow(viewPort->getSystemHandle(), SW_RESTORE);
#elif defined __APPLE

#endif
}

void CVView::setVisiblePanel(const unsigned int i)
{
    unsigned int L = 0;
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
            it != viewPanels.end(); ++it)
    {
        if(L != i) (*it)->setVisible(false);
        else (*it)->setVisible(true);
    }
}

void CVView::setVisiblePanel(const std::string tag)
{
    std::vector<std::string>::iterator strIT = panelTags.begin();
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
            it != viewPanels.end(); ++it)
    {
        if(*strIT != tag) (*it)->setVisible(false);
        else (*it)->setVisible(true);
        ++strIT;
    }
}

void CVView::setVisiblePanel(CVViewPanel* panel)
{
    for(std::vector<CVViewPanel*>::iterator it = viewPanels.begin();
            it != viewPanels.end(); ++it)
    {
        if(*it != panel) (*it)->setVisible(false);
        else (*it)->setVisible(true);
    }
}

void CVView::setPosition(const sf::Vector2i& position)
{
    if(parentView != nullptr)  // Relative to parent if parent exists
    {
        sf::Vector2i parentPosition(parentView->viewPort->getPosition());
        parentPosition.x += position.x;
        parentPosition.y += position.y;
        viewPort->setPosition(parentPosition);
    }
    else viewPort->setPosition(position);
}

void CVView::setScreenPosition(sf::Vector2i newPosition)
{
    viewPort->setPosition(newPosition);
}

void CVView::draw(sf::RenderTarget* target)
{
    if(bClosed || (viewPort == nullptr) || !viewPort->isOpen()) return;
    switch(viewState)
    {
    case VIEW_STATE_STARTUP:
    {
        break;
    }
    case VIEW_STATE_MAIN:
    {
        for(auto& item : viewPanels)
        {
            if(item->isVisible()) item->draw(target);
        }
        break;
    }
    default:
        break;
    }
}

bool CVView::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(bClosed || (viewPort == nullptr) || !viewPort->isOpen()) return false;

    // Handle screenshots in the main update thread
    if(saveRequestFiles.size() > 0)
    {
        sf::Texture screenshot;
        screenshot.create(getWidth(), getHeight());

        while(saveRequestFiles.size() > 0)
        {
            screenshot.update(*viewPort);
            screenshot.copyToImage().saveToFile(saveRequestFiles.front());
            saveRequestFiles.erase(saveRequestFiles.begin());
        }
    }

    for(size_t i = 0; i < numPendingAnims;)
    {
        if(pendingAnims[i].finished())
        {
            pendingAnims.erase(pendingAnims.begin() + i);
            --numPendingAnims;
        }
        else
        {
            pendingAnims[i].animFrame(event.lastFrameTime);
            ++i;
        }
    }

#ifndef __APPLE__
    if(!handleViewEvents(event)) return false;
#else
    while(!event.eventsProcessed)
    {
        if(bClosed || (!viewPort))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::duration<float>(0.01f));
    }
#endif

    if(viewPort->getPosition() != moveTarget)
    {
        sf::Vector2i viewPos(viewPort->getPosition());
        if(velocity.x != 0)
        {
            if(viewPos.x < moveTarget.x)
            {
                if(viewPos.x + velocity.x > moveTarget.x)
                {
                    viewPos.x = moveTarget.x;
                    velocity.x = 0;
                }
                else viewPos.x += velocity.x;
            }
            if(viewPos.x > moveTarget.x)
            {
                if(viewPos.x + velocity.x < moveTarget.x)
                {
                    viewPos.x = moveTarget.x;
                    velocity.x = 0;
                }
                else viewPos.x += velocity.x;
            }
        }

        if(velocity.y != 0)
        {
            if(viewPos.y < moveTarget.y)
            {
                if(viewPos.y + velocity.y > moveTarget.y)
                {
                    viewPos.y = moveTarget.y;
                    velocity.y = 0;
                }
                else viewPos.y += velocity.y;
            }
            else if(viewPos.y > moveTarget.y)
            {
                if(viewPos.y + velocity.y < moveTarget.y)
                {
                    viewPos.y = moveTarget.y;
                    velocity.y = 0;
                }
                else viewPos.y += velocity.y;
            }
        }
        viewPort->setPosition(viewPos);
    }

    if(viewPort->getSize() != resizeTarget)
    {
        sf::Vector2u viewSize = viewPort->getSize();
        bool resizeFLAG = false;

        if(resizeSpeed.x != 0)
        {
            if(viewSize.x < resizeTarget.x)
            {
                if(viewSize.x + resizeSpeed.x > resizeTarget.x)
                {
                    viewSize.x = resizeTarget.x;
                    resizeSpeed.x = 0;
                }
                else viewSize.x += resizeSpeed.x;
            }
            else if(viewSize.x > resizeTarget.x)
            {
                if(viewSize.x + resizeSpeed.x < resizeTarget.x)
                {
                    viewSize.x = resizeTarget.x;
                    resizeSpeed.x = 0;
                }
                else viewSize.x += resizeSpeed.x;
            }
            resizeFLAG = true;
        }

        if(resizeSpeed.y != 0)
        {
            if(viewSize.y < resizeTarget.y)
            {
                if(viewSize.y + resizeSpeed.y > resizeTarget.y)
                {
                    viewSize.y = resizeTarget.y;
                    resizeSpeed.y = 0;
                }
                else viewSize.y += resizeSpeed.y;
            }
            else if(viewSize.y > resizeTarget.y)
            {
                if(viewSize.y + resizeSpeed.y < resizeTarget.y)
                {
                    viewSize.y = resizeTarget.y;
                    resizeSpeed.y = 0;
                }
                else viewSize.y += resizeSpeed.y;
            }
            resizeFLAG = true;
        }

        if(resizeFLAG) viewPort->setSize(viewSize);
    }

#ifndef __APPLE__
    if(bClosed || (viewPort == nullptr))
    {
        close();
        return false;
    }
#endif

    if(event.viewHasFocus)
    {
        event.currentMousePosition = mousePos;
    }
    else
    {
        event.currentMousePosition = sf::Vector2f(NAN, NAN);
    }

    if(event.LMBreleased)
    {
        event.releaseCapturedShapes();
    }

    event.mouseTraceBuffer.emplace_back(mousePos);
    event.mouseTraceBuffer.erase(event.mouseTraceBuffer.begin());

    // Open the mouse pointer to capture
    event.mouseCaptured = false;

    // Open the view focus to capture
    event.focusCaptured = false;

    for(auto& element : boost::adaptors::reverse(popUpElements))
    {
        element->update(event, mousePos);
    }
    for(auto& panel : boost::adaptors::reverse(viewPanels))
    {
        panel->update(event, mousePos);
    }

    if(event.closed())  // Check for a close signal
    {
        bClosed = true;
        return false;
    }

    event.moveCapturedShapes();

    event.lastFrameMousePosition = mousePos;
    event.lastFrameGlobalMousePosition = sf::Vector2f(sf::Mouse::getPosition());
    event.timeLastKey += event.lastFrameTime;
    event.avgFrameTime *= event.numFrameAvg;
    event.avgFrameTime += event.lastFrameTime;
    event.avgFrameTime /= (event.numFrameAvg + 1);

    if(event.LMBhold)
    {
        event.LMBholdTime += event.lastFrameTime;
    }
    else
    {
        event.LMBreleaseTime += event.lastFrameTime;
    }

    if(event.RMBhold)
    {
        event.RMBholdTime += event.lastFrameTime;
    }
    else
    {
        event.RMBreleaseTime += event.lastFrameTime;
    }

    event.tossData(); // Toss data if conditions are met and not picked up
    event.eventsProcessed = false;

    return true;
}

bool CVView::handleViewEvents(CVEvent& event)
{
    event.mouseWheelDelta.x = 0.0f;
    event.mouseWheelDelta.y = 0.0f;
    event.keyLog.clear();
    event.viewResized = false;
    event.keyPressed = false;

    if((viewPort == nullptr) || (!viewPort->isOpen())) return false;

    sf::Event SFevent;
    event.viewHasFocus = viewPort->hasFocus();

    while(viewPort->pollEvent(SFevent))
    {
        switch(SFevent.type)
        {
        case sf::Event::Closed:
        {
            bClosed = true;
            return false;
            break;
        }
        case sf::Event::Resized:
        {
            event.lastViewBounds = event.viewBounds;
            viewPort->setView(sf::View(sf::FloatRect(0, 0, SFevent.size.width, SFevent.size.height)));
            event.viewBounds = sf::FloatRect(viewPort->getPosition().x, viewPort->getPosition().y,
                                             viewPort->getSize().x, viewPort->getSize().y);
            event.lastViewBounds = event.viewBounds;
            event.viewResized = true;
            break;
        }
        case sf::Event::MouseButtonPressed:
        {
            switch(SFevent.mouseButton.button)
            {
            case sf::Mouse::Left:
            {
                if(!event.LMBhold)
                {
                    event.LMBreleased = false;
                    event.LMBhold = true;
                    event.LMBholdFrames = 0;
                    event.LMBholdTime = 0.0f;
                    event.lastLMBpressPosition = event.LMBpressPosition;
                    event.LMBpressPosition = mousePos;
                }
                break;
            }
            case sf::Mouse::Right:
            {
                if(!event.RMBhold)
                {
                    event.RMBreleased = false;
                    event.RMBhold = true;
                    event.RMBholdFrames = 0;
                    event.RMBholdTime = 0.0f;
                    event.lastRMBpressPosition = event.lastRMBpressPosition;
                    event.RMBpressPosition = mousePos;
                }
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            switch(SFevent.mouseButton.button)
            {
            case sf::Mouse::Left:
            {
                event.LMBhold = false;
                event.LMBreleased = true;
                event.LMBreleaseFrames = 0;
                event.LMBreleaseTime = 0.0f;
                event.LMBreleasePosition = mousePos;
                break;
            }
            case sf::Mouse::Right:
            {
                event.RMBhold = false;
                event.RMBreleased = true;
                event.RMBreleaseFrames = 0;
                event.RMBreleaseTime = 0.0f;
                event.RMBreleasePosition = mousePos;
                break;
            }
            default:
            {
                break;
            }
            }
            break;
        }
        default:
            break;
        }

        if(event.viewHasFocus)  // Only processed if view has focus
        {
            switch(SFevent.type)
            {
            case sf::Event::TextEntered:
            {
                event.timeLastKey = 0.0f;
                event.keyPressed = true;
                if(!ctrlPressed() && (SFevent.text.unicode < 127))
                {
                    event.keyLog += static_cast<char>(SFevent.text.unicode);
                }
                break;
            }
            case sf::Event::KeyPressed:
            {
                event.keyPressed = true;
                event.timeLastKey = 0.0f;

                if(ctrlPressed())
                {
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) event.keyLog += 'a';
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) event.keyLog += 'd';
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)) event.keyLog += 'v';
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) event.keyLog += 'c';
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) event.keyLog += 'z';
                    break;
                }

                switch(SFevent.key.code)
                {
                case sf::Keyboard::Delete:
                {
                    event.keyLog += static_cast<char>(CV_KEY_DELETE);
                    break;
                }
                case sf::Keyboard::Left:
                {
                    event.keyLog += static_cast<char>(CV_KEY_LEFT);
                    break;
                }
                case sf::Keyboard::Right:
                {
                    event.keyLog += static_cast<char>(CV_KEY_RIGHT);
                    break;
                }
                case sf::Keyboard::Up:
                {
                    event.keyLog += static_cast<char>(CV_KEY_UP);
                    break;
                }
                case sf::Keyboard::Down:
                {
                    event.keyLog += static_cast<char>(CV_KEY_DOWN);
                    break;
                }
                case sf::Keyboard::Return:
                {
                    event.keyLog += static_cast<char>(CV_KEY_RETURN);
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if(SFevent.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    event.mouseWheelDelta.y = SFevent.mouseWheelScroll.delta;
                }
                if(SFevent.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
                {
                    event.mouseWheelDelta.x = SFevent.mouseWheelScroll.delta;
                }
                break;
            }
            default:
                break;
            }
        }
    }

    if(event.LMBhold) ++event.LMBholdFrames;
    else
    {
        ++event.LMBreleaseFrames;
    }

    if(event.RMBhold) ++event.RMBholdFrames;
    else ++event.RMBreleaseFrames;

    event.eventsProcessed = true;
    return true;
}

void CVView::close()
{

    if(!bClosed)
    {
#ifdef __APPLE__
        // Require main thread to call close() on OSX
        if(viewPort && viewPort->isOpen())
        {
            drawLock.lock();
            bClosed = true;
            viewPort->close();
            delete(viewPort);
            viewPort = nullptr;
            drawLock.unlock();
        }
#else
        bClosed = true;
#endif // __APPLE__
    }

    forceClose = true; // Let app master handle destructor from main thread
}

CVView::~CVView()
{
    for(auto& S : subViews)
    {
        S->close();
        delete(S);
    }
    for(auto& popUp : popUpElements)
    {
        delete(popUp);
    }
    for(auto& panel : viewPanels)
    {
        delete(panel);
    }

    close();

}

}
