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

#ifndef CVIS_VIEW
#define CVIS_VIEW

#include "cvision/lib.hpp"

#include <vector>
#include <string>
#include <thread>
#include <mutex>

#if defined WIN32 || defined _WIN32 || defined __WIN32
#include <windows.h>
#endif

#include <SFML/Graphics.hpp>

#include "cvision/event.hpp"
#include "cvision/anim.hpp"
#include "cvision/algorithm.hpp"

// Automatic view positioning =====================

#define VIEW_POSITION_CENTER            0_BIT
#define VIEW_POSITION_LEFT              1_BIT
#define VIEW_POSITION_RIGHT             2_BIT
#define VIEW_POSITION_TOP               3_BIT
#define VIEW_POSITION_BOTTOM            4_BIT
#define VIEW_POSITION_CENTER_LEFT       5_BIT
#define VIEW_POSITION_CENTER_RIGHT      6_BIT
#define VIEW_POSITION_CENTER_BOTTOM     7_BIT
#define VIEW_POSITION_CENTER_TOP        8_BIT
#define VIEW_POSITION_TOP_LEFT          9_BIT
#define VIEW_POSITION_TOP_RIGHT         10_BIT
#define VIEW_POSITION_BOTTOM_LEFT       11_BIT
#define VIEW_POSITION_BOTTOM_RIGHT      12_BIT

#define VIEW_STATE_STARTUP              0_BIT
#define VIEW_STATE_LOADING              1_BIT
#define VIEW_STATE_MAIN                 2_BIT
#define VIEW_STATE_CLOSING              3_BIT
#define VIEW_STATE_CLOSED               255_BIT

namespace cvis
{

class CVApp;
class CVViewPanel;
class CVTextBox;
class CVView;

#if defined WIN32 || defined _WIN32 || defined __WIN32
class CVISION_API CVDropTarget : public IDropTarget
{
public:

    CVISION_API CVDropTarget(CVView* view);
    virtual ~CVDropTarget() = default;

    bool getWaitingData(std::vector<std::string>& output); // Attempt to transfer waiting drop requests
    const bool& mouse_drag() const;

protected:

    // IUnknown interface virtuals
    CVISION_API STDMETHOD(QueryInterface)(REFIID iid, void FAR* FAR* ppvobj);
    CVISION_API STDMETHOD_(ULONG, AddRef)();
    CVISION_API STDMETHOD_(ULONG, Release)();

    // IDropTarget interface virtuals
    CVISION_API STDMETHOD(DragEnter)(LPDATAOBJECT pDataObj, DWORD grfKeyState,
                         POINTL pt, LPDWORD pdweffect);
    CVISION_API STDMETHOD(DragOver)(DWORD grfKeyState,
                        POINTL pt,
                        LPDWORD pdweffect);
    CVISION_API STDMETHOD(DragLeave)();
    CVISION_API STDMETHOD(Drop)(LPDATAOBJECT pDataObj,
                    DWORD grfKeyState,
                    POINTL pt,
                    LPDWORD pdweffect);

private:

    unsigned long FReferences;

    bool bAcceptFormat;
    bool bDragActive;

    CVView* viewHandle;

    std::vector<std::string> waiting_data;

    CVISION_API void HandleDrop(HDROP hDrop);

    std::mutex dropLock;

    // No copy to allow for mutex lock
    CVDropTarget(const CVDropTarget& other) = delete;
    CVDropTarget& operator=(const CVDropTarget& other) = delete;

};
#endif

class CVISION_API CVView
{
private:

    CVView(const CVView& other) = delete;
    CVView& operator=(const CVView& other) = delete;

protected:

    friend class CVApp;
    friend class CVElement;

#if defined WIN32 || defined _WIN32 || defined __WIN32
    CVDropTarget* dropTarget;
#endif

    bool                        forceClose;
    bool                        render;
    bool                        bCloseSignal;
    bool                        bClosed;
    bool                        bElasticSelect;
    bool                        bWindowCreateWaiting;
    bool                        bDropable;                 // Signal to the OS that this window can accept drop input
    bool                        bCursorOverride;           // Hide the native cursor and draw the cursor sprite instead
    bool                        bShadow;                   // Draw the shadow texture (usually bound to the cursor)

    float defaultViewScale;                         // Allow scaling based on view dimensions

    std::vector<std::string> saveRequestFiles;

    CVEvent eventTrace;

    sf::Color backgroundColor;

    sf::Cursor                  cursor_rep;         // Representation of current native cursor (non-iOS/Android)
    sf::Cursor::Type            OS_cursor_type;     // Current native cursor type

    sf::Sprite                  cursor;             // Cursor override if applicable
    sf::Sprite                  shadow;             // Cursor shadow if applicable (ie drag and drop, etc.)

    sf::Texture                 shadowTexture;      // Storage for a dynamically-captured shadow texture (ie. drag-and-drop)

    std::string                 name;
    uint32_t                    style;

    std::vector<CVViewPanel*>   viewPanels;
    std::vector<CVViewPanel*>   waitingViewPanels;
    std::vector<std::string>    panelTags;

    std::vector<CVElement*>     splashElements;

    std::vector<CVAnim>         pendingAnims;
    unsigned int                numPendingAnims;

    CVView*                     parentView;
    std::thread*                appThread;

    unsigned int                frameRateLimit;

    float                       frameRate;
    float                       width;
    float                       height;
    float                       titleBarHeight;
    float                       viewHeight;

    sf::Vector2i                resizeTarget;
    sf::Vector2i                resizeSpeed;
    sf::Vector2i                moveTarget;
    sf::Vector2i                velocity;

    // Render states

    uint8_t viewState;

    // Event handling

    std::chrono::high_resolution_clock::time_point timeLeftClickPressed;
    std::chrono::high_resolution_clock::time_point timeLastLeftClick;
    std::chrono::high_resolution_clock::time_point timeRightClickPressed;
    std::chrono::high_resolution_clock::time_point timeLastRightClick;

    CVISION_API void handleTriggerEvent(const unsigned char& eventID);
    CVISION_API void activateWindow();

public:

    void init();

    CVISION_API CVElement* getElementById(const std::string& name); // Retrieve an element by its tag

    std::mutex          drawLock;               // Prevent actions during window draw
    std::mutex          updateLock;             // Prevent actions during update
    std::mutex          captureLock;            // Resolve competing access over texture buffer

    sf::RenderWindow*   viewPort;
    sf::RenderTexture   textureBuffer;          // For capture of the current draw state (screenshot) or for masking/clipping, etc.

    inline bool captureRenderContext()
    {
        if(viewPort && viewPort->isOpen())
        {
            return viewPort->setActive(true);
        }
        return false;
    }

    CVISION_API void getTexture(sf::Texture& output);
    template<typename T> void releaseMouseCapture(const T& item)
    {
        eventTrace.releaseMouseCapture(item);
    }

    CVApp* mainApp;

    CVISION_API const sf::Font* appFont(const std::string& font) const;
    CVISION_API const sf::Texture* appTexture(const std::string& tag) const;
    CVISION_API const sf::Image* appImage(const std::string& tag) const;
    CVISION_API const sf::Color& appColor(const std::string& tag) const;

    sf::Vector2f mousePos;

    virtual bool preDrawProcess(){ return true; }        // Override to insert update events before the draw cycle
    virtual void postDrawProcess(){ }                    // Override to insert update events following the draw cycle

    CVISION_API void setTopMargin(const float& margin);
    CVISION_API void setDropable(const bool& status = true);

    inline void requestFocus()
    {
        viewPort->requestFocus();
    }
    inline void setBackgroundColor(const sf::Color& newColor)
    {
        backgroundColor = newColor;
    }

    inline const std::thread* mainAppThreadID() const noexcept
    {
        return appThread;
    }
    inline const CVEvent& getEventTrace() const noexcept
    {
        return eventTrace;
    }
    inline const float& getFrameRate() const noexcept
    {
        return frameRate;
    }

    inline const bool& closed() const noexcept
    {
        return bClosed;
    }

    inline bool window_open() const
    {
        return viewPort && viewPort->isOpen();
    }

    inline void setElasticSelectState(bool newState) noexcept
    {
        bElasticSelect = newState;
    }
    inline bool canElasticSelect() const noexcept
    {
        return bElasticSelect;
    }

    CVISION_API void setState(uint8_t newState);
    inline uint8_t getState()
    {
        return viewState;
    }

    // Alter the window cursor state

    CVISION_API void setCursor(const sf::Cursor::Type& newCursor); //  Switch the native cursor type
    CVISION_API void setCursor(const sf::Texture* texture,  //  Replace the native cursor with a custom graphic
                               const sf::Vector2f& size = sf::Vector2f(24.0f, 24.0f),
                               const sf::Color& fillColor = sf::Color::White,
                               const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f));
    CVISION_API void setCursor(const std::string& texture,  //  Replace the native cursor with a custom graphic
                               const sf::Vector2f& size = sf::Vector2f(24.0f, 24.0f),
                               const sf::Color& fillColor = sf::Color::White,
                               const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f));

    CVISION_API void clearCursor();                         //  Clear the custom graphic and restore the native cursor
    inline const bool& cursor_overriden() const
    {
        return bCursorOverride;
    }

    // Create a shadow for drag-and-drop effects, etc.

    CVISION_API void setShadow(CVElement& element,
                               const uint8_t& alpha = 180,
                               const float& scale = 1.0f);
    CVISION_API void setShadow(const sf::Texture* texture,
                               const sf::Vector2f& size,
                               const uint8_t& alpha = 180,
                               const sf::Vector2f& origin = sf::Vector2f(NAN, NAN));
    CVISION_API void setShadow(const std::string& texture,
                          const sf::Vector2f& size,
                          const uint8_t& alpha = 180,
                          const sf::Vector2f& origin = sf::Vector2f(NAN, NAN));
    CVISION_API void clearShadow();

    CVISION_API void setVisible(const unsigned int index, bool newVisibleState);
    CVISION_API void isolateVisible(const unsigned int index);

    CVISION_API bool contains(const CVElement& element);
    CVISION_API virtual sf::FloatRect getBounds() const;

    inline std::vector<CVViewPanel*> selectTaggedPanels(const std::string& tag)
    {
        size_t L = panelTags.size();
        std::vector<CVViewPanel*> output;
        for(size_t i = 0; i < L; ++i)
        {
            if(panelTags[i].find(tag) < panelTags[i].size()) output.push_back(viewPanels[i]);
        }
        return output;
    }

    inline void bringToFront(CVViewPanel* panel)
    {
        hyperC::bringToFront(panel, viewPanels);
    }
    inline void sendToBack(CVViewPanel* panel)
    {
        hyperC::sendToBack(panel, viewPanels);
    }
    inline void bringForward(CVViewPanel* panel)
    {
        hyperC::bringForward(panel, viewPanels);
    }
    inline void sendBackward(CVViewPanel* panel)
    {
        hyperC::sendBackward(panel, viewPanels);
    }

    inline void animate(sf::Transformable* object, const std::vector<CVAnimCheckpoint>& checkpoints)
    {
        pendingAnims.emplace_back(object, checkpoints);
        ++numPendingAnims;
    }
    inline void anim_to(sf::Transformable* object, const sf::Vector2f& destination,
                        const float& speed, const uint8_t& animType = CV_OBJ_ANIM_SLIDE,
                        bool resetPath = true)
    {

        unsigned int index = 0;
        for(auto& anim : pendingAnims)
        {
            if(anim.object() == object)
            {
                if(resetPath)
                {
                    anim.changePath(std::vector<CVAnimCheckpoint>({CVAnimCheckpoint(destination, speed, animType)}));
                }
                else anim.addCheckpoints(std::vector<CVAnimCheckpoint>({CVAnimCheckpoint(destination, speed, animType)}));
                return;
            }
            ++index;
        }
        pendingAnims.emplace_back(object, std::vector<CVAnimCheckpoint>({CVAnimCheckpoint(destination, speed, animType)}));
        ++numPendingAnims;
    }

    inline void anim_move(sf::Transformable* object, const sf::Vector2f& distance,
                          const float& speed, const uint8_t animType = CV_OBJ_ANIM_SLIDE,
                          bool cumulative = true)
    {
        for(auto& anim : pendingAnims)
        {
            if(anim.object() == object)
            {
                anim.addCheckpoints(std::vector<CVAnimCheckpoint>(
                {
                    CVAnimCheckpoint(object->getPosition() + distance, speed, animType)
                }), cumulative);
                return;
            }
        }
        pendingAnims.emplace_back(object,
                                  std::vector<CVAnimCheckpoint>(
        {
            CVAnimCheckpoint(object->getPosition() + distance,
            speed, animType)
        }));
        ++numPendingAnims;
    }

    inline void anim_passive(sf::Transformable* object, const float& rate, const uint8_t animType = CV_OBJ_ANIM_FADE_OUT)
    {
        for(auto& anim : pendingAnims)
        {
            if(anim.object() == object)
            {
                anim.setPassiveAnim(animType, rate);
                return;
            }
        }

        pendingAnims.emplace_back(object,
                                  std::vector<CVAnimCheckpoint>(
        {
            CVAnimCheckpoint(object->getPosition(), 0.0f, CV_OBJ_ANIM_NONE)
        }));
        ++numPendingAnims;
    }
    inline void resetPassiveAnim(sf::Transformable* object)
    {
        for(auto& anim : pendingAnims)
        {
            if(anim.object() == object)
            {
                anim.resetPassiveAnim();
                return;
            }
        }
    }

    void stopAnim(sf::Transformable* element);

    std::string tag;
    unsigned int appThreadID;

    inline bool ready()
    {
        return viewPort != nullptr;
    }

    inline float getWidth() const
    {
        return width;
    }
    CVISION_API float renderWidth() const;
    CVISION_API float getHeight() const;
    CVISION_API float getTitleBarHeight() const;
    CVISION_API float renderHeight() const;

    CVISION_API sf::Vector2f getSize();

    inline void setSize(const sf::Vector2u& newSize)
    {
        viewPort->setSize(newSize);
    }
    inline void setSize(const sf::Vector2f& newSize)
    {
        viewPort->setSize(sf::Vector2u(newSize));
    }

    template<typename T> void setSize(const T& width, const T& height)
    {
        viewPort->setSize(sf::Vector2u(width, height));
    }

    template<typename T1, typename T2> void resize_to(const T1& width, const T1& height, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1))
    {
        resizeTarget = sf::Vector2i(width, height);
        resizeSpeed = speed;
        if(resizeTarget.x >= getSize().x) resizeSpeed.x = abs(resizeSpeed.x);
        else resizeSpeed.x = -abs(resizeSpeed.x);

        if(resizeTarget.y >= getSize().y) resizeSpeed.y = abs(resizeSpeed.y);
        else resizeSpeed.y = -abs(resizeSpeed.y);
    }
    template<typename T1, typename T2> void resize_to(const sf::Vector2<T1>& coords, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1))
    {
        resizeTarget = sf::Vector2i(coords);
        resizeSpeed = speed;

        if(resizeTarget.x >= getSize().x) resizeSpeed.x = abs(resizeSpeed.x);
        else resizeSpeed.x = -abs(resizeSpeed.x);

        if(resizeTarget.y >= getSize().y) resizeSpeed.y = abs(resizeSpeed.y);
        else resizeSpeed.y = -abs(resizeSpeed.y);
    }

    template<typename T1, typename T2> void move_to(const T1& width, const T1& height, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1))
    {
        moveTarget = sf::Vector2i(width, height);
        velocity = speed;

        if(moveTarget.x >= getPosition().x) velocity.x = abs(velocity.x);
        else velocity.x = -abs(velocity.x);

        if(moveTarget.y >= getPosition().y) velocity.y = abs(velocity.y);
        else velocity.y = -abs(velocity.y);
    }

    template<typename T1, typename T2> void move_to(const sf::Vector2<T1>& coords, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1))
    {
        moveTarget = sf::Vector2i(coords);
        velocity = speed;

        if(moveTarget.x >= getPosition().x) velocity.x = abs(velocity.x);
        else velocity.x = -abs(velocity.x);

        if(moveTarget.y >= getPosition().y) velocity.y = abs(velocity.y);
        else velocity.y = -abs(velocity.y);
    }

    template<typename T1, typename T2> void cascade(const sf::Vector2<T1>& newSizeDims,
            const T2& speed = sf::Vector2<T2>(1,1))  // Resize while anchoring bottom-right
    {
        resize_to(newSizeDims, speed);
        move_to(getPosition() + getSize() - newSizeDims, speed);
    }

    CVISION_API const float getViewScale() const;
    CVISION_API void setDefaultViewScale(const float& x, const float& y);

    inline CVView* getParentView()
    {
        return parentView;
    }
    inline void setParentView(CVView* newParentView)
    {
        parentView = newParentView;
    }
    CVISION_API void addSubView(CVView* newSubView, std::string tag = "");
    CVISION_API CVView* taggedSubView(std::string tag);

    inline CVViewPanel* getPanel(const unsigned int i)
    {
        return viewPanels[i];
    }

    CVISION_API CVViewPanel* getTaggedPanel(const std::string s);
    CVISION_API CVViewPanel* operator[](const std::string s);

    inline const std::vector<CVViewPanel*>& getPanels()
    {
        return viewPanels;
    }

    CVISION_API void addPanel(CVViewPanel* newPanel, std::string tag = "", const unsigned int& index = UINT_MAX);
    CVISION_API void setVisiblePanel(const unsigned int panelIndex);
    CVISION_API void setVisiblePanel(const std::string tag);
    CVISION_API void setVisiblePanel(CVViewPanel* panel);

    CVISION_API void minimize();
    CVISION_API void maximize();
    CVISION_API void restore();

    CVISION_API void setPosition(const sf::Vector2i& newPosition);
    inline void move(const sf::Vector2i& distance)
    {
        setPosition(getPosition() + distance);
    }

    inline void saveImage(const std::string& filename)  // Request the view thread to stop and save an image
    {
        saveRequestFiles.push_back(filename);
    }

    template<typename T> void setPosition(const T& x, const T& y)
    {
        setPosition(sf::Vector2i(x,y));
    }
    CVISION_API void setScreenPosition(sf::Vector2i newPosition); // Force absolute screen positioning

    inline sf::Vector2i getPosition() const
    {
        return viewPort->getPosition();
    }

    CVISION_API void close();

    CVISION_API void draw(sf::RenderTarget* target);
    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API bool handleViewEvents(CVEvent& event);

    CVISION_API CVView(unsigned int x, unsigned int y, std::string winName,
           uint32_t style, CVApp* mainApp, const sf::Vector2f& screenPosition = sf::Vector2f(NAN,NAN),
           const sf::Color& backgroundColor = sf::Color::Black);

    CVISION_API virtual ~CVView();
};

#define CV_DRAW_CLIP_BEGIN              sf::View init_view,\
                                                clip_region;\
                                        if(bClipBounds){\
                                            sf::FloatRect clip_bounds = panel.front().getGlobalBounds();\
                                            clip_bounds.top -= panel.front().getOutlineThickness();\
                                            clip_bounds.left -= panel.front().getOutlineThickness();\
                                            clip_bounds.width += 2*panel.front().getOutlineThickness();\
                                            clip_bounds.height += 2*panel.front().getOutlineThickness();\
                                            clip_region = sf::View(clip_bounds);\
                                            init_view = target->getView();\
                                            clip_region.setViewport(sf::FloatRect(clip_bounds.left / View->renderWidth(), \
                                                                                  clip_bounds.top / View->renderHeight(),\
                                                                    clip_bounds.width / View->renderWidth(), \
                                                                        clip_bounds.height / View->renderHeight()));\
                                            target->setView(clip_region);\
                                        }

#define CV_DRAW_CLIP_END                if(bClipBounds){\
                                            target->setView(init_view);\
                                        }

}

#endif // CVIS_VIEW
