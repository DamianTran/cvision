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

#include "cvision/view.hpp"
#include "cvision/app.hpp"
#include "cvision/viewpanel.hpp"

#if defined _WIN32 || defined _WIN32 || defined WIN32

#include <windows.h>
#include <ole2.h>
#include <oleidl.h>
#define WM_OLEDROP WM_USER + 1

#elif defined __APPLE__

#import <Foundation/Foundation.h>
#include <AvailabilityMacros.h>
#import <Cocoa/Cocoa.h>

#endif

using namespace std;

namespace cvis
{

#if defined _WIN32 || defined _WIN32 || defined WIN32

CVDropTarget::CVDropTarget(CVView* view):
    FReferences(1),
    bAcceptFormat(false),
    bDragActive(false),
    viewHandle(view)
{

}

void CVDropTarget::HandleDrop(HDROP hDrop)
{
    int num_files = DragQueryFile(hDrop, 0xFFFFFFFF,
                                  (LPSTR)NULL, NULL);

    if(num_files)
    {

        dropLock.lock();

        for(int i = 0; i < num_files; ++i)
        {
            int newDataLength = DragQueryFile(hDrop, i, NULL, NULL) + 1;
            char * newData = new char[newDataLength];
            DragQueryFile(hDrop, i, newData, newDataLength);
            waiting_data.emplace_back(newData);
            delete(newData);
        }

        dropLock.unlock();

    }
}

bool CVDropTarget::getWaitingData(vector<string>& output)
{

    if(waiting_data.empty())
    {
        return false;
    }

    dropLock.lock();

    output.insert(output.end(),
                  waiting_data.begin(),
                  waiting_data.end());
    waiting_data.clear();

    dropLock.unlock();

    return true;

}

const bool& CVDropTarget::mouse_drag() const
{
    return bDragActive;
}

STDMETHODIMP CVDropTarget::QueryInterface(REFIID iid, void FAR* FAR* ppv)
{
    if((iid == IID_IUnknown) || (iid == IID_IDropTarget))
    {
        *ppv = this;
        AddRef();
        return NOERROR;
    }

    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) CVDropTarget::AddRef()
{
    return ++FReferences;
}

STDMETHODIMP_(ULONG) CVDropTarget::Release()
{
    if(!(--FReferences))
    {
        delete this;
        return 0;
    }
    return FReferences;
}

// Check for data compatibility and other pre-initialization flags

STDMETHODIMP CVDropTarget::DragEnter(LPDATAOBJECT pDataObj,
                                     DWORD grfKeyState,
                                     POINTL pt,
                                     LPDWORD pdwEffect)
{
    FORMATETC fmtetc;

    fmtetc.cfFormat     =   CF_HDROP;
    fmtetc.ptd          =   NULL;
    fmtetc.dwAspect     =   DVASPECT_CONTENT;
    fmtetc.lindex       =   -1;
    fmtetc.tymed        =   TYMED_HGLOBAL;

    if(pDataObj->QueryGetData(&fmtetc) == NOERROR)
    {
        bAcceptFormat = true;
    }
    else
    {
        bAcceptFormat = false;
    }

    bDragActive = true;

    return NOERROR;
}

// Send a signal for the window to visually respond to the drag state

STDMETHODIMP CVDropTarget::DragOver(DWORD grfKeyState,
                                    POINTL pt,
                                    LPDWORD pdwEffect)
{

    return NOERROR;

}

// Notify the window that the drag state has been left

STDMETHODIMP CVDropTarget::DragLeave()
{
    bAcceptFormat = false;
    bDragActive = false;
    return NOERROR;
}

// Implement the data transfer

STDMETHODIMP CVDropTarget::Drop(LPDATAOBJECT pDataObj,
                             DWORD grfKeyState,
                             POINTL pt,
                             LPDWORD pdwEffect)
{

    FORMATETC fmtetc;

    fmtetc.cfFormat     =   CF_HDROP;
    fmtetc.ptd          =   NULL;
    fmtetc.dwAspect     =   DVASPECT_CONTENT;
    fmtetc.lindex       =   -1;
    fmtetc.tymed        =   TYMED_HGLOBAL;

    STGMEDIUM medium;
    HRESULT hr = pDataObj->GetData(&fmtetc, &medium);

    if(!FAILED(hr))
    {

        HGLOBAL hFiles = medium.hGlobal;
        HDROP hDrop = (HDROP)GlobalLock(hFiles);

        HandleDrop(hDrop);

        GlobalUnlock(hFiles);
        ReleaseStgMedium(&medium);

    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
        return hr;
    }

    return NOERROR;

}
#elif defined __APPLE__

CVDropTarget::CVDropTarget(CVView * View):
    viewHandle(View){ }

bool CVDropTarget::getWaitingData(vector<string>& output)
{

    NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];

    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        int L = [files count];

        if(L > 0)
        {

           CVEvent& event = viewHandle->eventTrace;

           if(event.LMBholdFrames == 1)
           {

               for(int i = 0; i < L; ++i)
               {
                   NSString* newPath = [files objectAtIndex:i];
                   waiting_data.emplace_back([newPath UTF8String]);
               }

               [pboard clearContents];

           }

       }

    }

    if(waiting_data.empty())
    {
        return false;
    }
    else if(viewHandle->eventTrace.LMBreleased)
    {

        if(viewHandle->getBounds().contains(viewHandle->eventTrace.LMBreleasePosition))
        {

            dropLock.lock();

            output.insert(output.end(),
                          waiting_data.begin(),
                          waiting_data.end());

            dropLock.unlock();

            waiting_data.clear();

            return true;
        }

        waiting_data.clear();

    }

    return false;

}

#endif

CVView::CVView(unsigned int x, unsigned int y, string winName,
               uint32_t style, CVApp* mainApp,
               const sf::Vector2f& screenPosition,
               const sf::Color& backgroundColor):
    dropTarget(nullptr),
    forceClose(false),
    bClosed(false),
    bElasticSelect(false),
    bWindowCreateWaiting(false),
    bDropable(false),
    bCursorOverride(false),
    defaultViewScale(1920.0f*1080.0f),
    backgroundColor(backgroundColor),
    OS_cursor_type(sf::Cursor::Arrow),
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
    cout << "Activating OpenGL context\n";
    mainApp->setContextActive();
}

void CVView::init()
{

    cout << "Initializing CVView\n";

    sf::ContextSettings contextSettings;

    #ifdef __APPLE__
    contextSettings.antialiasingLevel = 0;
    #else
    contextSettings.antialiasingLevel = 4;
    #endif

    cout << "Creating render window (" << width << " x " << height << ")\n";
    cout << "Antialiasing: " << contextSettings.antialiasingLevel << "x\n";

    viewPort = new sf::RenderWindow(sf::VideoMode(width, height), name, style, contextSettings);

    cursor_rep.loadFromSystem(sf::Cursor::Arrow);

    viewPort->setVerticalSyncEnabled(true);
    viewPort->setMouseCursor(cursor_rep);
    viewPort->setFramerateLimit(frameRateLimit);

    const float frameTime = 0.5f/frameRateLimit;

    moveTarget = viewPort->getPosition();
    sf::Vector2i intPos;

    chrono::high_resolution_clock::time_point t0 = TIME_NOW;
    const chrono::duration<float> frameUpdateLatency(frameTime);

    chrono::duration<float> duration;

    cout << "Initializing event tracer\n";

    eventTrace.viewBounds = sf::FloatRect(moveTarget.x, moveTarget.y, width, height);
    eventTrace.lastViewBounds = eventTrace.viewBounds;
    eventTrace.lastFrameMousePosition =
        sf::Vector2f(sf::Mouse::getPosition(*viewPort));

    setState(VIEW_STATE_MAIN);

    cout << "Main sequence\n";

    while(!bClosed)
    {

        if(!preDrawProcess()) break;

        if(forceClose)
        {

            viewPort->close();

            break;
        }

        intPos = sf::Mouse::getPosition(*viewPort);
        mousePos.x = intPos.x;
        mousePos.y = intPos.y;

        if(mainApp->setContextActive()) // Activate the background context for texture updates
        {
            update(eventTrace, mousePos);
        }

        if(bClosed) break;

        if(viewPort->setActive(true))   // Activate the viewport context for draw
        {
            viewPort->clear(backgroundColor);
            draw(viewPort);
            viewPort->display();
        }

        postDrawProcess();

        nextFrame:;

        // Count actual frame time

        duration = TIME_NOW - t0;

        eventTrace.lastFrameTime = chrono::duration<float>(TIME_NOW - t0).count();
        t0 = TIME_NOW;
        frameRate = 1.0f/eventTrace.avgFrameTime;

    }

    if(viewPort && viewPort->isOpen()) viewPort->close();
}

CVElement* CVView::getElementById(const string& tag)
{

    if(tag.empty()) return nullptr;

    CVElement* output = nullptr;

    for(auto& panel : viewPanels)
    {

        if(panel->tag() == tag)
        {
            return panel;
        }

        if(output && (output == panel->getOwnedElementByID(tag)))
        {
            return output;
        }

    }

    return output;
}

void CVView::activateWindow()
{

//    viewPort->setActive(true);

}

void CVView::setDropable(const bool& status)
{

    bDropable = status;

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

    textureBuffer.setActive(true);

    textureBuffer.create(width, height);
    textureBuffer.setView(sf::View(getBounds()));
    textureBuffer.clear();

    draw(&textureBuffer);

    textureBuffer.display();
    texture = textureBuffer.getTexture();

    mainApp->setContextActive();

    captureLock.unlock();
}

const sf::Font* CVView::appFont(const string& font) const
{
    if(mainApp)
    {
        const sf::Font* output = mainApp->fonts[font];

        if(!output)
        {
            try
            {
                return mainApp->fonts[mainApp->font_panel.at(font)];
            }catch(...)
            {
                return nullptr;
            }
        }

        return output;
    }
    return nullptr;
}

const sf::Texture* CVView::appTexture(const string& tag) const
{
    if(mainApp) return mainApp->bitmaps.taggedTexture(tag);
    return nullptr;
}

const sf::Image* CVView::appImage(const string& tag) const
{
    if(mainApp) return mainApp->bitmaps.taggedImage(tag);
    return nullptr;
}

const sf::Color& CVView::appColor(const string& tag) const
{
    if(mainApp)
    {
        try
        {
            return mainApp->colors.at(tag);
        }catch(...)
        {
            throw invalid_argument("CVView: no color to map to tag \"" + tag + "\"");
        }
    }
    throw invalid_argument("CVVuiew: No app available to derive color from tag");
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

void CVView::setCursor(const sf::Cursor::Type& newCursor)
{
    cursor_rep.loadFromSystem(newCursor);
    viewPort->setMouseCursor(cursor_rep);
}

void CVView::setCursor(const sf::Texture* texture,
                       const sf::Vector2f& size,
                       const sf::Color& fillColor,
                       const sf::Vector2f& origin)
{

    cursor.setTexture(*texture, true);
    cursor.setOrigin((origin * texture->getSize())/size);
    cursor.setScale(size/texture->getSize());
    cursor.setColor(fillColor);
    bCursorOverride = true;
    viewPort->setMouseCursorVisible(false);

}

void CVView::setCursor(const string& texture,
                       const sf::Vector2f& size,
                       const sf::Color& fillColor,
                       const sf::Vector2f& origin)
{
    const sf::Texture* cursorTexture = mainApp->bitmaps.taggedTexture(texture);
    if(cursorTexture)
    {
        setCursor(cursorTexture, size, fillColor, origin);
    }
}

void CVView::clearCursor()
{
    bCursorOverride = false;
    viewPort->setMouseCursorVisible(true);
}

void CVView::setShadow(CVElement& element,
                       const uint8_t& alpha,
                       const float& scale)
{
    element.getTexture(shadowTexture);

    shadow.setTexture(shadowTexture, true);
    shadow.setOrigin(element.getOrigin());
    shadow.setScale(element.getSize()/shadowTexture.getSize() * scale);
    shadow.setPosition(element.getPosition());
    shadow.setColor(sf::Color(255,255,255,alpha));

    bShadow = true;
}

void CVView::setShadow(const sf::Texture* texture,
                       const sf::Vector2f& size,
                       const uint8_t& alpha,
                       const sf::Vector2f& origin)
{

    shadow.setTexture(*texture);

    if(isnan(origin.x) || isnan(origin.y))
    {
        shadow.setOrigin(sf::Vector2f(texture->getSize())/2);
    }
    else
    {
        shadow.setOrigin(origin/size * texture->getSize());
    }

    shadow.setScale(size/texture->getSize());
    shadow.setColor(sf::Color(255,255,255,alpha));

}

void CVView::setShadow(const string& texture,
                       const sf::Vector2f& size,
                       const uint8_t& alpha,
                       const sf::Vector2f& origin)
{
    const sf::Texture* shadow_t = mainApp->bitmaps.taggedTexture(texture);
    if(shadow_t)
    {
        setShadow(shadow_t, size, alpha, origin);
    }
}

void CVView::clearShadow()
{
    bShadow = false;
}

bool CVView::contains(const CVElement& element)
{
    sf::FloatRect viewBounds(0.0f,0.0f,width,height);
    return viewBounds.intersects(element.getBounds());
}

CVViewPanel* CVView::getTaggedPanel(const string s)
{
    return *taggedItem(s, viewPanels, panelTags);
}
CVViewPanel* CVView::operator[](const string s)
{
    return *taggedItem(s, viewPanels, panelTags);
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

void CVView::addPanel(CVViewPanel* newPanel, string tag, const unsigned int& index)
{
    if(addUnique(newPanel, viewPanels, index))
    {
        if(tag.size() > 0)
        {
            panelTags.push_back(tag);
            newPanel->setTag(tag);
        }
        else{
            newPanel->setTag("Panel " + to_string(viewPanels.size()));
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
    for(vector<CVViewPanel*>::iterator it = viewPanels.begin();
            it != viewPanels.end(); ++it)
    {
        if(L != i) (*it)->setVisible(false);
        else (*it)->setVisible(true);
    }
}

void CVView::setVisiblePanel(const string tag)
{
    vector<string>::iterator strIT = panelTags.begin();
    for(vector<CVViewPanel*>::iterator it = viewPanels.begin();
            it != viewPanels.end(); ++it)
    {
        if(*strIT != tag) (*it)->setVisible(false);
        else (*it)->setVisible(true);
        ++strIT;
    }
}

void CVView::setVisiblePanel(CVViewPanel* panel)
{
    for(vector<CVViewPanel*>::iterator it = viewPanels.begin();
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
    switch(viewState)
    {
    case VIEW_STATE_STARTUP:
    {
        break;
    }
    case VIEW_STATE_MAIN:
    {

        mainApp->setContextActive();

        for(auto& item : viewPanels)
        {
            if(item->isVisible())
            {
                item->draw(target);
            }
        }
        if(bShadow)
        {
            target->draw(shadow);
        }
        if(bCursorOverride)
        {
            target->draw(cursor);
        }

        break;
    }
    default:
        break;
    }
}

void CVView::stopAnim(sf::Transformable* element)
{
    for(size_t i = 0; i < pendingAnims.size();)
    {
        if(pendingAnims[i].animObject == element)
        {
            pendingAnims.erase(pendingAnims.begin() + i);
        }
        else ++i;
    }
}

bool CVView::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(bClosed || (viewPort == nullptr) || !viewPort->isOpen()) return false;

    // Handle cursor override

    if(bCursorOverride)
    {
        cursor.setPosition(mousePos);
    }

    // Handle drag and drop

    if(bDropable && !dropTarget)
    {

        dropTarget = new CVDropTarget(this);
        #if defined WIN32 || defined __WIN32 || defined _WIN32

        HWND winHandle = viewPort->getSystemHandle();

        RegisterDragDrop(winHandle, (LPDROPTARGET)dropTarget);

        #elif defined __APPLE__

        const NSWindow* viewWindow = (NSWindow*)viewPort->getSystemHandle();


        #endif // WIN32
    }
    else if(!bDropable && dropTarget)
    {
        #if defined WIN32 || defined __WIN32 || defined _WIN32

        HWND winHandle = viewPort->getSystemHandle();
        delete(dropTarget);
        dropTarget = nullptr;

        RevokeDragDrop(winHandle);

        #elif defined __APPLE__

        #endif // WIN32

        delete(dropTarget);
        dropTarget = nullptr;
    }

    if(dropTarget)
    {
        dropTarget->getWaitingData(eventTrace.drop_data);
    }

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

    for(size_t i = 0; i < pendingAnims.size();)
    {
        if(pendingAnims[i].finished())
        {
            pendingAnims.erase(pendingAnims.begin() + i);
        }
        else
        {
            pendingAnims[i].animFrame(event.lastFrameTime);
            ++i;
        }
    }

    if(!handleViewEvents(event)) return false;

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

    if(bClosed || (viewPort == nullptr))
    {
        close();
        return false;
    }

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

    // Reset the native cursor type to default
    event.awaitingCursorType = sf::Cursor::Arrow;

    for(int i = viewPanels.size() - 1; (i >= 0) && !viewPanels.empty(); --i)
    {
        if(viewPanels[i]->shouldDelete())
        {
            delete(viewPanels[i]);
            viewPanels.erase(viewPanels.begin() + i);
        }
        else
        {
            viewPanels[i]->update(event, mousePos);
        }
    }

    if(event.closed())  // Check for a close signal
    {
        bClosed = true;
        return false;
    }

    if(OS_cursor_type != event.awaitingCursorType) // Only load the cursor if different from last frame
    {
        if(!bCursorOverride)
        {
            cursor_rep.loadFromSystem(event.awaitingCursorType);
            viewPort->setMouseCursor(cursor_rep);
        }
        OS_cursor_type = event.awaitingCursorType;
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

    if(event.LMBreleaseFrames > 5)
    {
        event.tossData(); // Toss data if conditions are met and not picked up
    }
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
//            viewPort->setView(sf::View(sf::FloatRect(0, 0, SFevent.size.width, SFevent.size.height)));
            event.viewBounds = sf::FloatRect(viewPort->getPosition().x, viewPort->getPosition().y,
                                             viewPort->getSize().x, viewPort->getSize().y);
            event.lastViewBounds = event.viewBounds;
            event.viewResized = true;

//            resize(event.lastViewBounds, event.viewBounds);
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
                if(!ctrlPressed())
                {

                    switch(SFevent.text.unicode)
                    {
                    case 13: // Carriage-return
                        {
                            event.keyLog.push_back(CV_KEY_RETURN);
                            break;
                        }
                    default:
                        {
                            event.keyLog.push_back(SFevent.text.unicode);
                        }
                    }

                }
                break;
            }
            case sf::Event::KeyPressed:
            {
                event.keyPressed = true;
                event.timeLastKey = 0.0f;

                if(ctrlPressed())
                {
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) event.keyLog.emplace_back(uint32_t('a'));
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) event.keyLog.emplace_back(uint32_t('d'));
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::V)) event.keyLog.emplace_back(uint32_t('v'));
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)) event.keyLog.emplace_back(uint32_t('c'));
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) event.keyLog.emplace_back(uint32_t('z'));
                    break;
                }

                switch(SFevent.key.code)
                {
                case sf::Keyboard::Delete:
                    {
                        event.keyLog.push_back(CV_KEY_DELETE);
                        break;
                    }
                case sf::Keyboard::Left:
                    {
                        event.keyLog.push_back(CV_KEY_LEFT);
                        break;
                    }
                case sf::Keyboard::Right:
                    {
                        event.keyLog.push_back(CV_KEY_RIGHT);
                        break;
                    }
                case sf::Keyboard::Up:
                    {
                        event.keyLog.push_back(CV_KEY_UP);
                        break;
                    }
                case sf::Keyboard::Down:
                    {
                        event.keyLog.push_back(CV_KEY_DOWN);
                        break;
                    }
                default:
                    {
                        break;
                    }
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
        if(viewPort && viewPort->isOpen())
        {
            bClosed = true;
            viewPort->close();
            delete(viewPort);
            viewPort = nullptr;
        }
    }
    else
    {
        if(viewPort)
        {
            viewPort->close();
            delete(viewPort);
            viewPort = nullptr;
        }
    }

    forceClose = true; // Let app master handle destructor from main thread
}

CVView::~CVView()
{

    for(auto& panel : viewPanels)
    {
        delete(panel);
    }

    if(dropTarget)
    {
        delete(dropTarget);
    }

    close();

}

}
