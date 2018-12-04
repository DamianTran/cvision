//////////////////////////////////////////////////////////////
//
// CVision: a multi-platform graphics interface libary for C++
//
//////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2018 Damian Tran
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
// Redistribution of CVision is permitted under accordance with
// the GNU general public licence (GPL) version 3.0 and the
// terms and conditions specified in CVlicence.txt

#pragma once

#ifndef CVIS_BASE
#define CVIS_BASE

#include "algorithm.hpp"
#include "cvision/cvis_anim.hpp"
#include "cvision/cvis_algorithm.hpp"
#include "cvision/primitives.hpp"

#include <boost/range/adaptor/reversed.hpp>
#include <boost/filesystem.hpp>

// Action flags

#define VIS_MOVABLE                     true
#define VIS_IMMOVABLE                   false

#define VIS_HIGHLIGHT                   true
#define VIS_NONHIGHLIGHT                false

#define VIS_ACTIVE                      true
#define VIS_INACTIVE                    false

#define VIS_CLICKABLE                   true
#define VIS_NONCLICKABLE                false

// Trigger ID toolkit ============================

// Flag IDs

#define CV_TRIGGER_FLAG_MISC            255_BIT
#define CV_TRIGGER_FLAG_NONE            0_BIT
#define CV_TRIGGER_FLAG_TYPE            1_BIT
#define CV_TRIGGER_FLAG_DIRECTION       2_BIT

// Direction flags

#define CV_TRIGGER_DIRECTION_NONE       0b0
#define CV_TRIGGER_DIRECTION_LEFT       0b1
#define CV_TRIGGER_DIRECTION_RIGHT      0b10
#define CV_TRIGGER_DIRECTION_UP         0b100
#define CV_TRIGGER_DIRECTION_DOWN       0b1000

// Type flags

#define CV_TRIGGER_FAIL                 0_BIT
#define CV_TRIGGER_CLICK                1_BIT
#define CV_TRIGGER_HOVER                2_BIT
#define CV_TRIGGER_HOLD                 3_BIT
#define CV_TRIGGER_BUTTON               4_BIT
#define CV_TRIGGER_TIME                 5_BIT
#define CV_TRIGGER_HOLD_RELEASE         6_BIT
#define CV_TRIGGER_MISC                 255_BIT

// Zone flags

#define CV_TRIGGER_ZONE_NONE            0_BIT
#define CV_TRIGGER_ZONE_CENTER          1_BIT
#define CV_TRIGGER_ZONE_BODY            2_BIT
#define CV_TRIGGER_ZONE_BORDER          3_BIT
#define CV_TRIGGER_ZONE_OUTSIDE         4_BIT

// ================================================

// Event handling =================================

#define CV_ZOOM_NONE                    0_BIT
#define CV_ZOOM_IN                      1_BIT
#define CV_ZOOM_OUT                     2_BIT
#define CV_ZOOM_RESET                   3_BIT
#define CV_ZOOM_IN_MAX                  4_BIT
#define CV_ZOOM_OUT_MAX                 5_BIT

#define CV_WHEEL_DIR_NONE                   0b0
#define CV_WHEEL_DIR_VERTICAL               0b1
#define CV_WHEEL_DIR_HORIZONTAL             0b10

#define CV_KEY_LEFT                     1
#define CV_KEY_RIGHT                    2
#define CV_KEY_UP                       3
#define CV_KEY_DOWN                     4
#define CV_KEY_BACKSPACE                8
#define CV_KEY_RETURN                   10
#define CV_KEY_DELETE                   127

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

#define ELEMENT_CENTER_TOP_LEFT         0_BIT
#define ELEMENT_CENTERED                1_BIT
#define ELEMENT_CENTER_TOP_RIGHT        2_BIT
#define ELEMENT_CENTER_BOTTOM_LEFT      3_BIT
#define ELEMENT_CENTER_BOTTOM_RIGHT     4_BIT
#define ELEMENT_CENTER_LEFT             5_BIT
#define ELEMENT_CENTER_RIGHT            6_BIT
#define ELEMENT_CENTER_TOP              7_BIT
#define ELEMENT_CENTER_BOTTOM           8_BIT

#define TEXT_ALIGN_VERTICAL             0_BIT
#define TEXT_ALIGN_HORIZONTAL           1_BIT

// Data transfer types ============================

#define CV_DATA_VOID                    0_BIT
#define CV_DATA_STRING                  1_BIT
#define CV_DATA_NUMERIC                 2_BIT

#define CV_EVENT_OFF                    0b0         // Event ended
#define CV_EVENT_ON                     0b1         // Event began
#define CV_EVENT_LMB                    0b10        // Left mouse
#define CV_EVENT_RMB                    0b100       // Right mouse
#define CV_EVENT_KEY                    0b1000      // Key pressed - requires additional parameter to specify

#define CV_CLASS_NONE                   0_BIT
#define CV_CLASS_CVIS                   1_BIT
#define CV_CLASS_SPRITE                 2_BIT
#define CV_CLASS_SHAPE                  3_BIT

// Movement =======================================

#define CV_LAYER_ALL                    0b11111111

#define CV_LAYER_SPRITES                0b1
#define CV_LAYER_SHAPES                 0b10
#define CV_LAYER_TEXT                   0b100
#define CV_LAYER_MEMBERS                0b1000

#define CV_EXIT_SUCCESS                 0_BIT
#define CV_INIT_FAIL                    0_BIT
#define CV_INIT_SUCCESS                 1_BIT

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

#ifdef __APPLE__
const static std::chrono::duration<float> VIEW_EVENT_WAIT(0.01f);
#endif

namespace cvis{

class GUI;
class GUIpanel;
class CVApp;
class CVView;
class CVElement;
class CVViewPanel;

struct CVData{
    uint8_t infoType;
    unsigned char releaseCond;
    sf::Keyboard::Key releaseKey;

    char* data;
    size_t length;

    CVData(const CVData& other);
    CVData& operator=(const CVData& other);

    CVData(const void* data, const size_t& length, const uint8_t& infoType,
           const unsigned char& releaseCond, const sf::Keyboard::Key& releaseKey = sf::Keyboard::Unknown);
    ~CVData();

};

struct CVCaptureRef{
    void* item;
    uint8_t type;
    unsigned char releaseCond;
    sf::Keyboard::Key releaseKey;

    CVCaptureRef():
        item(nullptr),
        type(CV_CLASS_NONE),
        releaseCond(0b0),
        releaseKey(sf::Keyboard::Unknown){ }

    friend bool operator==(const CVCaptureRef& capref, const CVCaptureRef& other);
    template<typename T> friend bool operator==(const CVCaptureRef& capref, const T& other){
        return capref.item == (void*)&other;
    }

    template<typename T> void capture(T& item, const unsigned char& releaseCond = CV_EVENT_LMB,
                                      const sf::Keyboard::Key& releaseKey = sf::Keyboard::Unknown){
        if(dynamic_cast<CVElement*>(&item) != nullptr) type = CV_CLASS_CVIS;
        else if(dynamic_cast<sf::Sprite*>(&item) != nullptr) type = CV_CLASS_SPRITE;
        else if(dynamic_cast<sf::Shape*>(&item) != nullptr) type = CV_CLASS_SHAPE;

        this->item = &item;
        this->releaseCond = releaseCond;
        this->releaseKey = releaseKey;
    }
};

class CVEvent{
public:
    bool LMBhold,
        RMBhold,
        LMBreleased,
        RMBreleased,
        viewClosed,
        viewHasFocus,
        mouseCaptured,
        focusCaptured,
        viewResized,
        eventsProcessed,
        keyPressed,
        closeSignal;

    unsigned int LMBholdFrames,
            RMBholdFrames,
            LMBreleaseFrames,
            RMBreleaseFrames,
            maxMouseTrace,          // Buffer length for mouse frame positions
            numFrameAvg;            // Number of frames to average the frame time over

    std::string keyLog;
    void clearKeys();       // Capture all keys
    bool keyIsPressed(const char& key);

    bool checkCond(const unsigned char& cond, const sf::Keyboard::Key& keyCond = sf::Keyboard::Unknown);  // Check if condition flags are met

    std::vector<CVData> transferData; // Transfer data between elements over multiple update cycles

    template<typename T> bool getData(T& output){
        if(AIALG::getTypeID(output) == AIALG::getTypeID<std::string>()){
            for(size_t i = 0; i < transferData.size(); ++i){
                if((transferData[i].infoType == CV_DATA_STRING) && checkCond(transferData[i].releaseCond)){
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
                                       const sf::Keyboard::Key& releaseKey = sf::Keyboard::Unknown){
        if(AIALG::getTypeID(*input) == AIALG::getTypeID<char>()){
            transferData.emplace_back(input, length, CV_DATA_STRING,
                                      releaseCond, releaseKey);
        }
    }

    void releaseData(const unsigned char& cond = 0b11111111);
    void tossData(); // Remove data if conditions are met and not collected

    float timeLastKey,
            lastFrameTime,
            zoomDelta,
            avgFrameTime,
            LMBholdTime,
            RMBholdTime,
            LMBreleaseTime,
            RMBreleaseTime;

    AIALG::BYTE zoomState,
                wheelDirection;

    inline float avgFrameRate() const{ return 1.0f/avgFrameTime; }

    std::vector<CVCaptureRef> mouseCapturedShapes; // Captured shapes will trail mouse
    inline void releaseCapturedShapes(){ mouseCapturedShapes.clear(); }

    template<typename T> inline void mouse_capture(T& item){
        for(auto& ref : mouseCapturedShapes){
            if(ref == item) return;
        }
        mouseCapturedShapes.emplace_back();
        mouseCapturedShapes.back().capture(item);
    }
    template<typename T> inline bool isCaptured(const T& item) const{
        for(auto& CVref : mouseCapturedShapes){
            if(CVref == item) return true;
        }
        return false;
    }
    template<typename T> inline void release_mouse_capture(const T& item){
        for(size_t i = 0; i < mouseCapturedShapes.size();){
            if(mouseCapturedShapes[i] == item){
                mouseCapturedShapes.erase(mouseCapturedShapes.begin() + i);
            }
            else ++i;
        }
    }
    template<typename T> inline void freeze_capture(T& item){
        if(isCaptured(item)) item.move(lastFrameMousePosition - currentMousePosition); // Counter motion
    }
    template<typename T> inline void releaseMouseCapture(const T& item){
        for(size_t i = 0; i < mouseCapturedShapes.size(); ++i){
            if(mouseCapturedShapes[i] == item){
                mouseCapturedShapes.erase(mouseCapturedShapes.begin() + i);
                return;
            }
        }
    }
    inline bool noCapture() const{ return mouseCapturedShapes.empty(); }

    void moveCapturedShapes(); // Move captured shapes using mouse frame distance


    float mouseVelocity() const;
    float mouseMoveAngle() const;

    sf::Vector2f viewResizeScale,
                LMBpressPosition,
                RMBpressPosition,
                lastFrameMousePosition,
                currentMousePosition,
                LMBreleasePosition,
                RMBreleasePosition,
                mouseWheelDelta,
                lastFrameGlobalMousePosition;

    std::vector<sf::Vector2f> mouseTraceBuffer; // Record of mouse positions for the last N frames

    sf::FloatRect viewBounds,
                lastViewBounds;

    bool captureMouse(); // Steal mouse input from other items that may compete within a view (ie. overlapping widgets)
    bool captureFocus(); // Steal focus from other items that may compete within a view (ie. lower panels)
    void releaseFocus(); // Allow lower panels to capture focus
    bool focusFree();   // Check focus availability without stealing away from lower panels
    void reset();

    bool leftClick(const float& duration = 0.0f) const;
    bool rightClick(const float& duration = 0.0f) const;

    bool leftClickReleased(const float& duration = 0.0f) const;
    bool rightClickReleased(const float& duration = 0.0f) const;

    inline void close(){ closeSignal = true; }
    inline const bool& closed() const{ return closeSignal; }

    #ifdef __APPLE__
    void transfer(const CVEvent& other);
    #endif

    friend std::ostream& operator<<(std::ostream& output, const CVEvent& event);

    CVEvent();
    ~CVEvent() = default;
};

typedef std::map<std::string, int> KeyMapping; // General function-key mapping by index

// SFML Expansion

template<typename T1, typename T2> constexpr bool operator==(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    return ((T1)RHS.x == LHS.x) && ((T1)RHS.y == LHS.y);
}

template<typename T1, typename T2> constexpr bool operator!=(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    return ((T1)RHS.x != LHS.x) || ((T1)RHS.y != LHS.y);
}

template<typename T1, typename T2> constexpr sf::Vector2<T1> operator+(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    return sf::Vector2<T1>(LHS.x + RHS.x, LHS.y + RHS.y);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator-(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    return sf::Vector2<T1>(LHS.x - RHS.x, LHS.y - RHS.y);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator*(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    return sf::Vector2<T1>(LHS.x * RHS.x, LHS.y * RHS.y);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator/(const sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    return sf::Vector2<T1>(LHS.x / RHS.x, LHS.y / RHS.y);
}

template<typename T1, typename T2> constexpr sf::Vector2<T1> operator+(const sf::Vector2<T1>& LHS, const T2& RHS){
    return sf::Vector2<T1>(LHS.x / RHS, LHS.y / RHS);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator-(const sf::Vector2<T1>& LHS, const T2& RHS){
    return sf::Vector2<T1>(LHS.x / RHS, LHS.y / RHS);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator*(const sf::Vector2<T1>& LHS, const T2& RHS){
    return sf::Vector2<T1>(LHS.x / RHS, LHS.y / RHS);
}
template<typename T1, typename T2> constexpr sf::Vector2<T1> operator/(const sf::Vector2<T1>& LHS, const T2& RHS){
    return sf::Vector2<T1>(LHS.x / RHS, LHS.y / RHS);
}

template<typename T1, typename T2> inline void operator+=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS += T1(RHS);
}
template<typename T1, typename T2> inline void operator-=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS -= T1(RHS);
}
template<typename T1, typename T2> inline void operator*=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS *= T1(RHS);
}
template<typename T1, typename T2> inline void operator/=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS /= T1(RHS);
}

template<typename T1, typename T2> inline void operator+=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x += T1(RHS);
    LHS.y += T1(RHS);
}
template<typename T1, typename T2> inline void operator-=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x -= T1(RHS);
    LHS.y -= T1(RHS);
}
template<typename T1, typename T2> inline void operator*=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x *= T1(RHS);
    LHS.y *= T1(RHS);
}
template<typename T1, typename T2> inline void operator/=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x /= T1(RHS);
    LHS.y /= T1(RHS);
}

// Package managers

class FontManager{
public:
    std::vector<sf::Font> fonts;
    std::vector<std::string> fontTags;

    const sf::Font* operator[](const unsigned int& i) const{
        if(i >= fonts.size()) throw std::out_of_range("Font Manager: Index out of range of available fonts");
        return &fonts[i];
    }
    const sf::Font* operator[](const std::string& s) const{
        if(s.empty()) return nullptr;
        unsigned int index = 0;
        for(auto& tag : fontTags){
            if(tag == s) return &fonts[index];
            ++index;
        }
        throw std::invalid_argument("Font Manager: Font tag does not correspond to a font in Font Manager");
    }

    bool addFont(std::string fileDir, std::string tag);
    bool addFont(const void* binaries, const size_t& size,
                           const std::string& tag);

    FontManager(){ }

};

// Info constructs

struct textEntry{
    std::string text, font;
    unsigned int fontSize, alignment;
    sf::Color textColor;
    sf::Text::Style textStyle;

    inline void operator=(std::string newText){ text = newText; }
    inline void operator=(unsigned int newAlignment){ alignment = newAlignment; }
    inline void operator=(sf::Color newColor){ textColor = newColor; }

    textEntry(){ }
    textEntry(std::string text,
              std::string font,
              unsigned int size = 12,
              unsigned int alignment = ALIGN_CENTER,
              sf::Color textColor = sf::Color::Black,
              sf::Text::Style textStyle = sf::Text::Style::Regular):
                  text(text),
                  font(font),
                  fontSize(size),
                  alignment(alignment),
                  textColor(textColor),
                  textStyle(textStyle){ }

};

class CVTrigger{ // Base common trigger format - instantiate at trigger point and pass reference along
protected:
    std::string infoArray,      // Bytes of flag values
                    flags,       // Flag codes
                    tag;         // A tag associated with the trigger

public:
    inline void addFlag(const char& newFlag, const char& newValue = 0b0){
        infoArray.push_back(newValue);
        flags.push_back(newFlag);
    }
    inline void removeFlag(const char& flag){
        for(size_t i = 0; i < flags.size(); ++i){
            if(flags[i] == flag){
                flags.erase(flags.begin() + i);
                infoArray.erase(infoArray.begin() + i);
                return;
            }
        }
    }
    inline void changeFlagValue(const char& flag, const char& newValue){
        for(size_t i = 0; i < flags.size(); ++i){
            if(flags[i] == flag) infoArray[i] = newValue;
        }
    }
    inline void setFlagValue(const unsigned int& flagIndex, const char& newValue){
        infoArray[flagIndex] = newValue;
    }
    inline bool getFlagValue(const char& flag, char& output) const{
        for(size_t i = 0; i < flags.size(); ++i){
            if(flags[i] == flag){
                output = infoArray[i];
                return true;
            }
        }
        return false;
    }
    inline std::string getTag() const{ return tag; }
    inline void setTag(const std::string& newTag){ tag = newTag; }

    CVTrigger(const std::string& flags, const std::string& infoArray, const std::string& tag = std::string()):
        infoArray(infoArray),
        flags(flags),
        tag(tag){ }
    ~CVTrigger() = default;

};

typedef std::vector<sf::Texture> TextureList;
typedef std::vector<sf::Image> ImageList;

class ImageManager{
protected:
    TextureList textures;
    std::vector<std::string> textureTags;
    unsigned int numTextures;

    ImageList images;
    std::vector<std::string> imageTags;
    unsigned int numImages;

public:
    const sf::Texture* indexTexture(const unsigned int i) const{ return &textures[i]; }
    const sf::Image* indexImage(const unsigned int i) const{ return &images[i]; }

    const sf::Texture* taggedTexture(const std::string& s) const;
    const sf::Image* taggedImage(const std::string& s) const;

    std::string getTextureName(const sf::Texture* texture) const;

    bool addImage(std::string fileName, std::string tag);
    bool addTexture(std::string fileName, std::string tag);

    bool addTexture(const void* binaries, const size_t& size,
                           const std::string& tag);

    ImageManager():
        numTextures(0),
        numImages(0){ }
    ~ImageManager(){ }

};

// Base classes

class GUIpanel{ // Panels organize elements
protected:
    bool visible;
    std::vector<sf::Text> overlayText; // Free-floating text

public:

    GUI* parentGUI;
    std::vector<CVElement*> panelElements;

    inline void addElement(CVElement* newElement){ panelElements.push_back(newElement); }
    void addText(const textEntry& newText, const sf::Vector2f& position);
    void addText(const textEntry& newText, const uint8_t position);

    inline void removeText(const unsigned int index){ overlayText.erase(overlayText.begin() + index); }
    inline sf::Text* getText(const unsigned int index){ return &overlayText[index]; }
    inline void clearText(){ overlayText.clear(); }
    void clear();

    void draw(sf::RenderTarget* target);
    inline const bool isVisible(){ return visible; }
    inline void setVisible(bool visibleState){ visible = visibleState; }

    GUIpanel():
        visible(true){ }
    GUIpanel(GUI* parentGUI):
        visible(true),
        parentGUI(parentGUI){ }
    ~GUIpanel();
};

class GUI{ // GUI organizes panels overlayed on a view
protected:
    bool visible;
    std::vector<GUIpanel> panelList;
    std::vector<std::string> panelTags;

public:
    CVApp* mainApp;
    CVView* View;
    sf::RenderWindow* viewPort;

    inline void newPanel(std::string panelName){
        panelList.emplace_back(this);
        panelTags.push_back(panelName); }
    void removePanel(std::string panelName);

    void addPanelElement(std::string panelName, CVElement* newElement);
    GUIpanel* taggedPanel(const std::string& panelName);
    GUIpanel* operator[](const std::string& panelName){ return taggedPanel(panelName); }

    void draw(sf::RenderTarget* target);
    inline bool isVisible(){ return visible; }
    inline void setVisible(bool visibleState){ visible = visibleState; }

    bool update(CVEvent& event, const sf::Vector2f& mousePos){ return false; }

    GUI():visible(true){ }
    GUI(sf::RenderWindow* window, CVView* View = nullptr);
    ~GUI(){ }
};

class CVElement{
protected:

    bool visible;

    bool canMove;
    bool canHighlight;
    bool canClick;
    bool canDrag;
    bool active;

    bool mouseHovering;
    bool clickHeld;
    bool highlighted;

    bool bHasFocus,
        bFollowMouseX,
        bFollowMouseY,
        bFade,
        bMove,
        bSpriteOnly,
        bExpand,
        bStop,              // When velocity reaches zero, stop instead of turning
        bBounce,            // Bounce off the view boundaries instead of flying off
        bForceOnScreen,     // Keep this item on screen
        bHasShadow,         // Has stationary texture copy
        bDragAndDrop,       // Allow drag-and-drop mechanics
        bClipBounds;        // Clip draw region to bounds

    std::string IDtag;

    std::string triggerID;
    void addTypeID(const std::string& ID);

    std::vector<sf::Sprite> spriteList;    // Sprites under the control of this element, add to this list (ie. pawns)
    sf::Texture shadowTexture,              // Texture storage for shadow item
                clipTexture;                // Texture draw target for clipping

    sf::Sprite shadow,                      // Shadow of this item: use for drag and drop, etc.
                drawMask;                   // Clipping mask if region outside of bounds is to be excluded from the render

    ColorTheme colorTheme;      // Main element colors
    sf::Color highlightColor;   // Highlight color if applicable

    sf::Vector2f pinnedOffset;
    sf::Vector2f origin,
                createdOrigin,
                destination,
                velocity,
                acceleration;   // Absolute acceleration (ie. gravity, buoyancy)

    float moveAngle,
            elasticity, // 0 - 1; how much momentum is conserved during bounce interactions
            friction;       // coefficient of friction translating to -pixels/s^2

    uint8_t state,
            targetAlpha;

    unsigned char fadeLayers; // Layers to apply fade to

    unsigned int stateNum,
                numStates;

    int fadeRate;

    sf::FloatRect bounds;
    virtual void updateBounds();

    KeyMapping controls;

public:
    CVView* View;
    sf::RenderWindow* drawTarget;
    CVViewPanel* viewPanel;

    const CVApp* mainApp() const;

    template<typename T> bool isType(){
        return dynamic_cast<T*>(this) != nullptr;
    }
    const sf::Texture* appTexture(const std::string& tag) const;
    const sf::Image* appImage(const std::string& tag) const;

    inline const bool& isVisible() const{ return visible; }
    inline void setVisible(bool visibleState){ visible = visibleState; }

    float viewScale() const;

    inline const unsigned int& getCurrentState() const{ return stateNum; }
    inline const unsigned int& getNumStates() const{ return numStates; }
    inline void setNumStates(const unsigned int& N){ numStates = N; }

    inline uint8_t getState(){ return state; };
    inline void setState(uint8_t newState){ state = newState; }
    void setFade(const uint8_t& alpha, const int& rate, const unsigned char& flags = CV_LAYER_ALL);
    inline void setFadeStatus(const bool& state, const unsigned char& flags = CV_LAYER_ALL){ bFade = state; }
    void setDrawClipping(const bool& state);

    inline bool clickable(){ return canClick; }
    inline const bool& highlightable() const{ return canHighlight; }
    inline const bool& isActive() const{ return active; }
    inline bool draggable(){ return canDrag; }
    inline bool stationary(){ return canMove; }

    inline void highlight(const bool& state){ highlighted = state; }
    virtual inline void setHighlightableStatus(const bool& state){ canHighlight = state; }
    inline void setDraggableStatus(bool state){
        if(state && !canMove) canMove = true;
        if(state && !canClick) canClick = true;
        canDrag = state; }
    virtual inline void setDragAndDrop(const bool& state){ bDragAndDrop = state; }

    virtual inline void setHighlightColor(const sf::Color& color){ highlightColor = color; }

    inline bool isHighlighted(){ return highlighted; }
    inline bool isClickHeld(){ return clickHeld; }
    inline bool hasMouseHover(){ return mouseHovering; }
    inline const bool& hasFocus() const{ return bHasFocus; }

    virtual void setClickState(bool state){ }
    virtual void setHighlightState(bool state){ }
    virtual void setMoveState(bool state){ }
    virtual void setActiveState(bool state){ }

    inline void setSpriteOnly(const bool& state = true){ bSpriteOnly = state; }

    virtual inline void setFocus(const bool& state){ bHasFocus = state; }
    inline void setMouseFollow(const bool& followX, const bool& followY){
        bFollowMouseX = followX;
        bFollowMouseY = followY;
    }

    // Interactivity

    inline std::string tag(){ return IDtag; }
    inline void setTag(const std::string& newTag){ IDtag = newTag; }

    // Positioning

    virtual void move(const sf::Vector2f& offset) = 0;
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x,y));
    }

    virtual void setPosition(const sf::Vector2f& position) = 0;
    inline void setPosition(const float& posX, const float& posY){
        setPosition(sf::Vector2f(posX, posY));
    }

    virtual void setExpand(const bool& state);
    inline const sf::FloatRect& getBounds() const{ return bounds; }
    inline const sf::FloatRect& getGlobalBounds() const{ return getBounds(); } // Interface with templated SFML functions

    virtual inline sf::Vector2f getPosition() const{
        return sf::Vector2f(bounds.left, bounds.top) + origin; }
    inline const sf::Vector2f& getOrigin() const{ return origin; }
    inline void setOrigin(const sf::Vector2f& newOrigin){ origin = newOrigin; }

    virtual sf::Vector2f getSize() = 0;
    virtual void setSize(const sf::Vector2f& newSize) = 0;
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    virtual float getOutlineThickness() const = 0;

    virtual void rotate(const float& angle,
                        const unsigned char& flags = CV_LAYER_ALL,
                        const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) = 0;
    virtual void setRotation(const float& angle,
                             const unsigned char& flags = CV_LAYER_ALL,
                             const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) = 0;
    virtual float getRotation(const unsigned char& flags = CV_LAYER_ALL) const = 0;

    // Movement and physics

    inline void setBounce(const bool& state){ bBounce = state; }
    inline void forceOnScreen(const bool& state = true){ bForceOnScreen = state; }

    void setElasticity(const float& newElasticity);
    inline void setFriction(const float& newCoefficient){ friction = newCoefficient; }
    inline void setVelocity(const sf::Vector2f& newVelocity){ velocity = newVelocity; }
    inline void setAcceleration(const sf::Vector2f& newAcceleration){ acceleration = newAcceleration; }

    inline float speed(){ return scalar(velocity); }

    void stop();                                    // Stop all motion
    void move_to(const sf::Vector2f& position,
                    const float& velocity,
                    const float& drag = 0.0f);    // Push toward destination
    void push(const float& angle,
                 const float& velocity,
                 const float& drag = 0.0f);        // Push in a direction

    // Subelements and texturing

    void addSprite(const sf::Texture* texture,
                   const sf::Vector2f& position,
                   const sf::Vector2f& size = sf::Vector2f(0,0),
                   const sf::Color& fillColor = sf::Color::White);       // Add a new sprite
    void removeSprites(const std::string& tag);                         // Remove all sprites with this tag
    bool has_sprite(const std::string& tag) const;                     // Check if a sprite of this type has been added

    inline const std::vector<sf::Sprite>& sprites() const{ return spriteList; }
    sf::Sprite& lastSprite();
    sf::Sprite& firstSprite();
    sf::Sprite& getSprite(const unsigned int& index);

    inline void removeAllSprites(){
        spriteList.clear();
    }
    inline void removeLastSprite(){
        if(spriteList.size() > 0) spriteList.pop_back();
    }
    inline void removeFirstSprite(){
        if(spriteList.size() > 0) spriteList.erase(spriteList.begin());
    }

    virtual void setFillTexture(sf::Texture* texture) = 0;
    inline sf::Color baseFillColor() const{
        if(colorTheme.size() > 0)
            return colorTheme.front();
        return sf::Color::Transparent;
    }
    inline sf::Color baseOutlineColor() const{
        if(colorTheme.size() > 1)
            return colorTheme[1];
        return sf::Color::Transparent;
    }

    virtual bool update(CVEvent& event, const sf::Vector2f& mousePos); // Core update function for time-dependent activities (ie. Animations)
    virtual bool draw(sf::RenderTarget* target);

    void getTexture(sf::Texture& output); // Get an image of the current draw state
    void createShadow(const uint8_t& alpha, const sf::Vector2f& scale = sf::Vector2f(1.0f,1.0f)); // Create a shadow using a draw texture
    void removeShadow();    // Disable shadow rendering

    virtual void sendData(CVEvent& event) const = 0;

    virtual void setColor(const sf::Color& newColor){
        if(colorTheme.size() < 1) colorTheme.emplace_back(newColor);
        else colorTheme.front() = newColor;

        for(auto& sprite : spriteList){
            sprite.setColor(newColor);
        }

        highlightColor = newColor;
        brighten(highlightColor, 40);
    }

    inline void setControls(const KeyMapping& newControls){ controls = newControls; }
    inline void setControlKey(const std::string& controlID, const sf::Keyboard::Key& key){
        controls[controlID] = key;
    }
    inline int getKey(const std::string& controlID){
        try{
            return controls[controlID];
        }
        catch(...){
            return sf::Keyboard::Unknown;
        }
    }

    CVElement();
    CVElement(CVView* View,
               bool canMove = false,
               bool canHighlight = false,
               bool canClick = false,
               bool canDrag = false,
               bool active = true);


    virtual ~CVElement();
};

class CVView{
private:

    CVView(const CVView& other) = delete;
    CVView& operator=(const CVView& other) = delete;

protected:

    friend class CVApp;

    bool forceClose, render,
        bClosed,
        bElasticSelect;

    float defaultViewScale; // Allow scaling based on view dimensions

    AIALG::StringVector saveRequestFiles;

    CVEvent eventTrace;

    sf::Color backgroundColor;

    std::vector<CVElement*> popUpElements;
    std::vector<CVElement*> waitingPopUpElements;

    std::vector<CVViewPanel*> viewPanels;
    std::vector<CVViewPanel*> waitingViewPanels;
    std::vector<std::string> panelTags;

    std::vector<CVElement*> splashElements;

    std::vector<CVAnim> pendingAnims;
    unsigned int numPendingAnims;

    std::vector<CVView*> subViews;
    std::vector<std::string> subViewTags;
    CVView* parentView;
    std::thread* appThread;

    unsigned int frameRateLimit;
    float frameRate,
            width, height,
            titleBarHeight,
            viewHeight;

    sf::Vector2i resizeTarget,
            resizeSpeed,
            moveTarget,
            velocity;

    // Render states

    uint8_t viewState;

    // Event handling

    std::chrono::high_resolution_clock::time_point timeLeftClickPressed;
    std::chrono::high_resolution_clock::time_point timeLastLeftClick;
    std::chrono::high_resolution_clock::time_point timeRightClickPressed;
    std::chrono::high_resolution_clock::time_point timeLastRightClick;

    void handleTriggerEvent(const AIALG::BYTE& eventID);

public:

    std::mutex drawLock,        // Prevent actions during window draw
                updateLock,     // Prevent actions during update
                captureLock;    // Resolve competing access over texture buffer

    sf::RenderWindow* viewPort;
    sf::RenderTexture textureBuffer;        // For capture of the current draw state (screenshot) or for masking/clipping, etc.

    void getTexture(sf::Texture& output);
    template<typename T> void releaseMouseCapture(const T& item){
        eventTrace.releaseMouseCapture(item);
    }

    GUI* overlay;
    CVApp* mainApp;

    sf::Vector2f mousePos;

    void setTopMargin(const float& margin);

    inline void requestFocus(){ viewPort->requestFocus(); }
    inline void setBackgroundColor(const sf::Color& newColor){ backgroundColor = newColor; }

    inline const std::thread* mainAppThreadID() const{ return appThread; }
    inline const CVEvent& getEventTrace() const{ return eventTrace; }
    inline const float& getFrameRate() const{ return frameRate; }

    inline bool closed() const{ return viewState == VIEW_STATE_CLOSED; }

    inline void setElasticSelectState(bool newState){ bElasticSelect = newState; }
    inline bool canElasticSelect() const{ return bElasticSelect; }

    inline void setState(uint8_t newState){ viewState = newState; }
    inline uint8_t getState(){ return viewState; }

    void setVisible(const unsigned int index, bool newVisibleState);
    void isolateVisible(const unsigned int index);

    bool contains(const CVElement& element);
    virtual inline sf::FloatRect getBounds() const;

    inline std::vector<CVViewPanel*> selectTaggedPanels(const std::string& tag){
        size_t L = panelTags.size();
        std::vector<CVViewPanel*> output;
        for(size_t i = 0; i < L; ++i){
            if(panelTags[i].find(tag) < panelTags[i].size()) output.push_back(viewPanels[i]);
        }
        return output;
    }

    inline void bringToFront(CVViewPanel* panel){ AIALG::bringToFront(panel, viewPanels); }
    inline void sendToBack(CVViewPanel* panel){ AIALG::sendToBack(panel, viewPanels); }
    inline void bringForward(CVViewPanel* panel){ AIALG::bringForward(panel, viewPanels); }
    inline void sendBackward(CVViewPanel* panel){ AIALG::sendBackward(panel, viewPanels); }

    inline void animate(sf::Transformable* object, const std::vector<CVAnimCheckpoint>& checkpoints){
        pendingAnims.emplace_back(object, checkpoints);
        ++numPendingAnims;
    }
    inline void anim_to(sf::Transformable* object, const sf::Vector2f& destination,
                        const float& speed, const uint8_t& animType = CV_OBJ_ANIM_SLIDE,
                        bool resetPath = true){

        unsigned int index = 0;
        for(auto& anim : pendingAnims){
            if(anim.object() == object){
                if(resetPath){
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
                          bool cumulative = true){
        for(auto& anim : pendingAnims){
            if(anim.object() == object){
                anim.addCheckpoints(std::vector<CVAnimCheckpoint>({
                                        CVAnimCheckpoint(object->getPosition() + distance, speed, animType)}), cumulative);
                return;
            }
        }
        pendingAnims.emplace_back(object,
                                  std::vector<CVAnimCheckpoint>({
                                        CVAnimCheckpoint(object->getPosition() + distance,
                                                         speed, animType) }));
        ++numPendingAnims;
    }

    inline void anim_passive(sf::Transformable* object, const float& rate, const uint8_t animType = CV_OBJ_ANIM_FADE_OUT){
        for(auto& anim : pendingAnims){
            if(anim.object() == object){
                anim.setPassiveAnim(animType, rate);
                return;
            }
        }

        pendingAnims.emplace_back(object,
                                  std::vector<CVAnimCheckpoint>({
                                        CVAnimCheckpoint(object->getPosition(), 0.0f, CV_OBJ_ANIM_NONE) }));
        ++numPendingAnims;
    }
    inline void resetPassiveAnim(sf::Transformable* object){
        for(auto& anim : pendingAnims){
            if(anim.object() == object){
                anim.resetPassiveAnim();
                return;
            }
        }
    }

    std::string tag;
    unsigned int appThreadID;

    inline void setGUI(GUI* newOverlay){ overlay = newOverlay; }
    inline GUI* getOverlay(){ return overlay; }

    inline bool ready(){ return viewPort != nullptr; }

    inline float getWidth() const{ return width; }
    float renderWidth() const;
    float getHeight() const;
    float renderHeight() const;

    sf::Vector2f getSize();

    inline void setSize(const sf::Vector2u& newSize){ viewPort->setSize(newSize); }
    inline void setSize(const sf::Vector2f& newSize){ viewPort->setSize(sf::Vector2u(newSize)); }

    template<typename T> void setSize(const T& width, const T& height){ viewPort->setSize(sf::Vector2u(width, height)); }

    template<typename T1, typename T2> void resize_to(const T1& width, const T1& height, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1)){
        resizeTarget = sf::Vector2i(width, height);
        resizeSpeed = speed;
        if(resizeTarget.x >= getSize().x) resizeSpeed.x = abs(resizeSpeed.x);
        else resizeSpeed.x = -abs(resizeSpeed.x);

        if(resizeTarget.y >= getSize().y) resizeSpeed.y = abs(resizeSpeed.y);
        else resizeSpeed.y = -abs(resizeSpeed.y);
    }
    template<typename T1, typename T2> void resize_to(const sf::Vector2<T1>& coords, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1)){
        resizeTarget = sf::Vector2i(coords);
        resizeSpeed = speed;

        if(resizeTarget.x >= getSize().x) resizeSpeed.x = abs(resizeSpeed.x);
        else resizeSpeed.x = -abs(resizeSpeed.x);

        if(resizeTarget.y >= getSize().y) resizeSpeed.y = abs(resizeSpeed.y);
        else resizeSpeed.y = -abs(resizeSpeed.y);
    }

    template<typename T1, typename T2> void move_to(const T1& width, const T1& height, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1)){
        moveTarget = sf::Vector2i(width, height);
        velocity = speed;

        if(moveTarget.x >= getPosition().x) velocity.x = abs(velocity.x);
        else velocity.x = -abs(velocity.x);

        if(moveTarget.y >= getPosition().y) velocity.y = abs(velocity.y);
        else velocity.y = -abs(velocity.y);
    }

    template<typename T1, typename T2> void move_to(const sf::Vector2<T1>& coords, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1)){
        moveTarget = sf::Vector2i(coords);
        velocity = speed;

        if(moveTarget.x >= getPosition().x) velocity.x = abs(velocity.x);
        else velocity.x = -abs(velocity.x);

        if(moveTarget.y >= getPosition().y) velocity.y = abs(velocity.y);
        else velocity.y = -abs(velocity.y);
    }

    template<typename T1, typename T2> void cascade(const sf::Vector2<T1>& newSizeDims,
                                 const T2& speed = sf::Vector2<T2>(1,1)){ // Resize while anchoring bottom-right
        resize_to(newSizeDims, speed);
        move_to(getPosition() + getSize() - newSizeDims, speed);
    }

    inline void addPopUp(CVElement* popUpElement){ waitingPopUpElements.push_back(popUpElement); }
    inline unsigned int numPopUpElements(){ return popUpElements.size(); }

    inline void deletePopUp(CVElement* popUpElement){
        for(std::vector<CVElement*>::iterator it = popUpElements.begin();
            it != popUpElements.end();){
                if(*it == popUpElement){
                    delete(*it);
                    popUpElements.erase(it);
                }
                else ++it;
        }
    }

    const float getViewScale() const;
    void setDefaultViewScale(const float& x, const float& y);

    inline const unsigned int numSubViews(){ return subViews.size(); }
    inline CVView* getParentView(){ return parentView; }
    inline void setParentView(CVView* newParentView){ parentView = newParentView; }
    void addSubView(CVView* newSubView, std::string tag = "");
    CVView* taggedSubView(std::string tag);

    inline CVViewPanel* getPanel(const unsigned int i){ return viewPanels[i]; }

    CVViewPanel* getTaggedPanel(const std::string s);
    CVViewPanel* operator[](const std::string s);

    void addPanel(CVViewPanel* newPanel, std::string tag = "", const unsigned int& index = UINT_MAX);
    void setVisiblePanel(const unsigned int panelIndex);
    void setVisiblePanel(const std::string tag);
    void setVisiblePanel(CVViewPanel* panel);

    void minimize();
    void maximize();
    void restore();

    void setPosition(const sf::Vector2i& newPosition);
    inline void move(const sf::Vector2i& distance){
        setPosition(getPosition() + distance);
    }

    inline void saveImage(const std::string& filename){ // Request the view thread to stop and save an image
        saveRequestFiles.push_back(filename);
    }

    template<typename T> void setPosition(const T& x, const T& y){ setPosition(sf::Vector2i(x,y)); }
    void setScreenPosition(sf::Vector2i newPosition); // Force absolute screen positioning

    inline sf::Vector2i getPosition() const{ return viewPort->getPosition(); }

    void close();

    void draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    bool handleViewEvents(CVEvent& event);

    CVView(unsigned int x, unsigned int y, std::string winName,
          uint32_t style, CVApp* mainApp, const sf::Vector2f& screenPosition = sf::Vector2f(NAN,NAN),
          const sf::Color& backgroundColor = sf::Color::Black);

    ~CVView();
};

class CVApp{
private:

    CVApp(const CVApp& other) = delete;
    CVApp& operator=(const CVApp& other) = delete;

protected:
    bool running;
    std::thread* mainUpdateThread;
    std::string defaultFont;
    std::mutex appSyncLock;

    #ifdef __APPLE__
    std::mutex eventLock;
    #endif

    inline void lock(){ appSyncLock.lock(); }
    inline void unlock(){ appSyncLock.unlock(); }

public:
    std::vector<CVView*> viewList;
    std::vector<std::string> viewTags;
    std::vector<GUI*> GUIlist;
    std::vector<std::string> GUITags;

    std::vector<std::thread*> viewThreads;

    unsigned int frameRate;
    float frameTime;

    float leftClickLatency;
    float rightClickLatency;

    inline const bool& isOpen() const{ return running; }

    inline void setRunning(bool runState){ running = runState; }
    inline const bool getRunState(){ return running; }

    FontManager fonts;
    ImageManager bitmaps;

    inline const std::string& getDefaultFont() const{ return defaultFont; }

    // App virtuals

    virtual uint8_t loadPackages() = 0; // Load data and media required for this app to run
    virtual uint8_t initMainLoop(uint8_t runMode) = 0; // Initiate a thread consisting of the main data update loop for this app
    virtual bool initUI(unsigned int videoX, unsigned int videoY) = 0; // Initiate the UI required for this app before main loop begins

    inline CVView* getView(const std::string& tag){
        unsigned int index = 0;
        for(auto viewTag : viewTags){
            if(viewTag == tag){
                return viewList[index];
            }
            ++index;
        }
        return nullptr;
    }
    inline bool viewExists(const std::string& tag){
        for(auto viewTag : viewTags){
            if(viewTag == tag){
                return true;
            }
        }
        return false;
    }

    inline GUI* getOverlay(const std::string& tag){
        unsigned int index = 0;
        for(auto GUItag : GUITags){
            if(GUItag == tag){
                return GUIlist[index];
            }
            ++index;
        }
        return nullptr;
    }
    inline bool overlayExists(const std::string& tag){
        for(auto GUITag : GUITags){
            if(GUITag == tag){
                return true;
            }
        }
        return false;
    }

    inline void addGUI(GUI* newGUI, std::string GUITag = ""){
        GUIlist.push_back(newGUI);
        GUITags.push_back(GUITag); }

    void initView(unsigned int x, unsigned int y,
                  std::string winName = "",
                  uint32_t style = sf::Style::Default){
        if(winName.size() < 1) winName = "View " + std::to_string(viewList.size());
        viewList.push_back(new CVView(x, y, winName, style, this));
        viewTags.push_back(winName);
    }

    bool addView(CVView* View, const std::string& viewTag);

    inline void setViewGUI(CVView* View, GUI* viewGUI){
        View->overlay = viewGUI;
    }
    void setViewGUI(std::string viewTag, std::string GUItag);

    #ifdef __APPLE__
    // Send event polling to the main thread on OSX
    void handleViewEvents();
    #endif

    void closeView(const unsigned int viewIndex);
    void closeView(const std::string& tag);
    void closeAll();

    void mainUpdate();

    explicit CVApp():
        running(false),
        mainUpdateThread(nullptr),
        defaultFont(),
        frameRate(60),
        frameTime(1.0f/60),
        leftClickLatency(0.5f),
        rightClickLatency(0.5f){ }
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
        rightClickLatency(rightClickLatency){ }

    ~CVApp();
};

// Derived classes

class CVShape: public CVElement{
protected:

    rounded_rectangle shapeMask;

    bool bMasked;

public:

    void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void setMask(const sf::Texture* texture,
                 const sf::Color& fillColor = sf::Color(255,255,255,100));
    void hideMask();
    void showMask();

    inline void setMaskColor(const sf::Color& newColor){ shapeMask.setFillColor(newColor); }
    void setMaskAlpha(const uint8_t& newAlpha);
    void setSpriteColor(const sf::Color& newColor);

    void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    virtual void setSize(const sf::Vector2f& newSize){
        bounds.width = newSize.x;
        bounds.height = newSize.y;
    }
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    virtual void sendData(CVEvent& event) const{ }

    CVShape(CVView* View);
    virtual ~CVShape(){ }

};

class CVBox: public CVShape{
protected:

    std::vector<rounded_rectangle> panel;
    std::vector<rounded_rectangle> mask;

    bool bNoFill; // For proper fade handling

public:

    void updateTriggers(){ }

    bool draw(sf::RenderTarget* target);

    void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setColor(const sf::Color& newColor);
    void setFillColor(const sf::Color& newColor);
    void setOutlineColor(const sf::Color& newColor);

    void setRounding(const float& newRadius,
                     const unsigned int& pointCount = 10,
                     const std::vector<bool>& states =
                     std::vector<bool>(4, true));
    const float& getRoundingRadius() const;
    const unsigned int& getRoundingPointCount() const;

    void setOutlineThickness(const float& newThickness);

    void setClearFill(const bool& state = true);

    void setExpand(const bool& state);
    void updateBounds();

    void rotate(const float& angle,
                const unsigned char& flags = CV_LAYER_ALL,
                const sf::Vector2f& origin = sf::Vector2f(NAN, NAN));
    void setRotation(const float& angle,
                     const unsigned char& flags = CV_LAYER_ALL,
                     const sf::Vector2f& origin = sf::Vector2f(NAN, NAN));
    void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    float getRotation(const unsigned char& flags = CV_LAYER_ALL) const;

    inline const rounded_rectangle& getPanelObject(const unsigned int order){ return panel[order]; }
    inline const rounded_rectangle& getTopPanel(){ return panel.back(); }
    inline const rounded_rectangle& getBottomPanel(){ return panel.front(); }
    inline unsigned int numPanelObjects() const{ return panel.size(); }

    inline float getOutlineThickness() const{ return panel.front().getOutlineThickness(); }

    void setFillTexture(sf::Texture* texture){ }
    inline sf::Vector2f getSize(){ return sf::Vector2f(bounds.width, bounds.height); }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVBox(CVView* View, sf::Vector2f position, float width, float height,
            sf::Color fillColor, sf::Color borderColor,
            float borderWidth);
    ~CVBox() = default;

};

class CVTextBox:public CVBox{
protected:
    uint8_t textAlignment;
    textEntry textInfo;

    std::vector<sf::Text> displayText;

    float textPadding;

    const sf::Font* textFont;
    const sf::Font* appFont(const std::string& font) const;

    bool bWrapText;

    virtual void alignText();
    void wrapText();

public:

    bool draw(sf::RenderTarget* target);
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setString(const std::string& newString);

    inline void setTextTemplate(const textEntry& textInfo){ this->textInfo = textInfo; }
    void setText(const unsigned int textIndex, std::string newText);
    void setTextSize(const unsigned int& newSize);

    void addTextEntry(textEntry newText, float padding = 12.0f, bool regular = true);
    void addTextEntry(const textEntry& newText, const sf::Vector2f& position);

    inline const textEntry& getTextInfo(){ return textInfo; }
    void setTextColor(const sf::Color& color);
    void setTextAlignment(const uint8_t& newAlignment);

    void setFont(const std::string& newFont);
    void setFont(const sf::Font& newFont);

    void setTextWrap(const bool& state);
    void setTextPadding(const float& newPadding);
    void setExpand(const bool& state);
    void updateBounds();

    void sendData(CVEvent& event) const;

    std::string getText() const;

    inline void removeTextEntry(const unsigned int textIndex){ displayText.erase(displayText.begin() + textIndex); }

    inline const sf::Color& baseTextColor() const{ return colorTheme[2]; }

    CVTextBox(CVView* View, sf::Vector2f position, float width, float height,
                textEntry textInfo, sf::Color fillColor, sf::Color borderColor,
                float borderWidth);
    CVTextBox(CVView* View, sf::Vector2f position, float width, float height,
               sf::Color fillColor, sf::Color borderColor,
                float borderWidth);

};

class CVViewPanel: public CVTextBox{
protected:

    std::vector<CVElement*> viewPanelElements;
    AIALG::StringVector viewPanelTags;

    bool bFitWindow,            // Fit to the window upon creation and upon resize
            bTransduceFade,     // Transduce fade events to panel elements
            bTransduceFocus,
            bDragAndDrop,       // Transfer focus to panel elements
            bOutOfBoundsDraw,   // Draw items outside of the panel boundary
            bOutOfBoundsUpdate; // Update items outside of the panel boundary

    sf::Sprite* dragShadow;     // If drag-and-drop, a sprite for the item shadow

    void updateBounds();

public:

    inline const std::vector<CVElement*>& getElements() const{ return viewPanelElements; }

    virtual void addPanelElement(CVElement* newElement, std::string newTag = "", const unsigned int& index = UINT_MAX);
    virtual void removePanelElement(CVElement* element);
    virtual void removePanelElement(const std::string& tag);
    virtual void removePanelElement(const unsigned int& index);

    virtual void clear();

    CVElement* getTaggedElement(const std::string& tag);
    bool elementExists(const std::string& tag);

    inline const std::vector<CVElement*>& getPanels() const{ return viewPanelElements; }
    virtual void setHighlightableStatus(const bool& status);

    inline void setOutOfBoundsUpdate(const bool& state = true){ bOutOfBoundsUpdate = state; }
    inline void setOutOfBoundsDraw(const bool& state = true){ bOutOfBoundsDraw = state; }

    void setFocus(const bool& state);

    inline unsigned int numPanels() const{ return viewPanelElements.size(); }

    void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y){
        setSize(sf::Vector2f(x, y));
    }

    void setHighlightColor(const sf::Color& color);

    void setExpand(const bool& state);

    virtual bool draw(sf::RenderTarget* target) = 0;
    virtual bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVViewPanel(CVView* parentView, std::string panelTag = "",
                sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                bool fitToWindow = true,
                const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));
    CVViewPanel(const CVViewPanel& other);
    ~CVViewPanel();
};

}

#endif // CVIS_BASE
