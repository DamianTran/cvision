#pragma once

#ifndef CVIS_BASE
#define CVIS_BASE

#include "algorithm.hpp"
#include "cvision/cvis_anim.hpp"

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

// Item states

#define STATE_NONE                      0_BIT
#define STATE_DRAG                      1_BIT

// Event handling =================================

#define CV_ZOOM_NONE                    0_BIT
#define CV_ZOOM_IN                      1_BIT
#define CV_ZOOM_OUT                     2_BIT
#define CV_ZOOM_RESET                   3_BIT
#define CV_ZOOM_IN_MAX                  4_BIT
#define CV_ZOOM_OUT_MAX                 5_BIT

#define CV_KEY_LEFT                     1
#define CV_KEY_RIGHT                    2
#define CV_KEY_UP                       3
#define CV_KEY_DOWN                     4

// Automatic view positioning

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

// ================================================

#define CV_EXIT_SUCCESS                 0_BIT
#define CV_INIT_FAIL                    0_BIT
#define CV_INIT_SUCCESS                 1_BIT

namespace cvis{

class GUI;
class GUIpanel;
class APP;
class view;
class visElement;
class CVViewPanel;

struct CVEvent{
    bool LMBhold,
        RMBhold,
        LMBreleased,
        RMBreleased,
        viewClosed,
        viewHasFocus,
        mouseCaptured,
        viewResized;

    unsigned int LMBholdFrames,
            RMBholdFrames,
            LMBreleaseFrames,
            RMBreleaseFrames;

    std::string keyLog;

    float timeLastKey,
            lastFrameTime,
            zoomDelta;

    BYTE zoomState;

    std::vector<void*> mouseCapturedShapes; // Captured shapes will trail mouse
    inline void releaseCapturedShapes(){ mouseCapturedShapes.clear(); }

    template<typename T> inline void mouse_capture(const T& item){
        mouseCapturedShapes.push_back((void*)&item);
    }
    template<typename T> inline bool isCaptured(const T& item) const{
        for(auto& ptr : mouseCapturedShapes){
            if(ptr == (void*)&item) return true;
        }
        return false;
    }
    template<typename T> inline void release_mouse_capture(const T& item){
        for(size_t i = 0; i < mouseCapturedShapes.size();){
            if(mouseCapturedShapes[i] == (void*)&item){
                mouseCapturedShapes.erase(mouseCapturedShapes.begin() + i);
            }
            else ++i;
        }
    }

    sf::Vector2f viewResizeScale,
                LMBpressPosition,
                RMBpressPosition,
                lastFrameMousePosition,
                currentMousePosition;

    sf::FloatRect viewBounds,
                lastViewBounds;

    inline bool captureMouse(){ // Steal focus from other items that may compete within a view (ie. lower layers)
        if(viewHasFocus){
            if(!mouseCaptured){
                mouseCaptured = true;
                return true;
            }
            else return false;
        }
        return false;
    }

    CVEvent():
        LMBhold(false),
        RMBhold(false),
        LMBreleased(false),
        RMBreleased(false),
        viewClosed(false),
        viewHasFocus(true),
        mouseCaptured(false),
        viewResized(false),
        LMBholdFrames(0),
        RMBholdFrames(0),
        LMBreleaseFrames(0),
        RMBreleaseFrames(0),
        timeLastKey(0.0f),
        lastFrameTime(0.0f),
        zoomDelta(0.0f),
        zoomState(CV_ZOOM_NONE),
        viewResizeScale(1.0f,1.0f),
        LMBpressPosition(0.0f,0.0f),
        RMBpressPosition(0.0f,0.0f),
        lastFrameMousePosition(0.0f,0.0f),
        currentMousePosition(0.0f,0.0f),
        viewBounds(0.0f,0.0f,0.0f,0.0f),
        lastViewBounds(viewBounds)
        { }

    ~CVEvent() = default;
};

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

template<typename T1, typename T2> constexpr void operator+=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS += T1(RHS);
}
template<typename T1, typename T2> constexpr void operator-=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS -= T1(RHS);
}
template<typename T1, typename T2> constexpr void operator*=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS *= T1(RHS);
}
template<typename T1, typename T2> constexpr void operator/=(sf::Vector2<T1>& LHS, const sf::Vector2<T2>& RHS){
    LHS /= T1(RHS);
}

template<typename T1, typename T2> constexpr void operator+=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x += T1(RHS);
    LHS.y += T1(RHS);
}
template<typename T1, typename T2> constexpr void operator-=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x -= T1(RHS);
    LHS.y -= T1(RHS);
}
template<typename T1, typename T2> constexpr void operator*=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x *= T1(RHS);
    LHS.y *= T1(RHS);
}
template<typename T1, typename T2> constexpr void operator/=(sf::Vector2<T1>& LHS, const T2& RHS){
    LHS.x /= T1(RHS);
    LHS.y /= T1(RHS);
}

// Package managers

class FontManager{
public:
    std::vector<sf::Font> fonts;
    std::vector<std::string> fontTags;

    const sf::Font* operator[](const unsigned int& i){
        return &fonts[i];
    }
    const sf::Font* operator[](const std::string& s){
        unsigned int index = 0;
        for(auto& tag : fontTags){
            if(tag == s) return &fonts[index];
            ++index;
        }
        throw std::invalid_argument("Font tag does not correspond to a font in Font Manager");
    }

    bool addFont(std::string fileDir, std::string tag);

    FontManager(){ }

};

// Info constructs

typedef std::vector<sf::Color> ColorTheme;

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

    ImageList images;
    std::vector<std::string> imageTags;

public:
    inline const sf::Texture* indexTexture(const unsigned int i){ return &textures[i]; }
    inline const sf::Image* indexImage(const unsigned int i){ return &images[i]; }

    inline const sf::Texture* taggedTexture(const std::string& s){
        if(s.size() < 1) return nullptr;
        unsigned int index = 0;
        for(auto& tag : textureTags){
            if(tag == s) return &textures[index];
            ++index;
        }
        return nullptr;
    }
    inline const sf::Image* taggedImage(const std::string& s){
        if(s.size() < 1) return nullptr;
        unsigned int index = 0;
        for(auto& tag : imageTags){
            if(tag == s) return &images[index];
            ++index;
        }
        return nullptr;
    }

    inline bool addImage(std::string fileName, std::string tag){
        images.emplace_back();
        if(!images.back().loadFromFile(fileName)){
            images.pop_back();
            return false;
        }
        imageTags.push_back(tag);
        return true;
    }
    inline bool addTexture(std::string fileName, std::string tag){
        textures.emplace_back();
        if(!textures.back().loadFromFile(fileName)){
            textures.pop_back();
            return false;
        }
        textures.back().setSmooth(true);
        textureTags.push_back(tag);
        return true;
    }

    ImageManager(){ }
    ~ImageManager(){ }

};

// Base classes

class GUIpanel{ // Panels organize elements
protected:
    bool visible;
    std::vector<sf::Text> overlayText; // Free-floating text

public:

    GUI* parentGUI;
    std::vector<visElement*> panelElements;

    inline void addElement(visElement* newElement){ panelElements.push_back(newElement); }
    void addText(const textEntry& newText, const sf::Vector2f& position);
    void addText(const textEntry& newText, const uint8_t position);

    inline void removeText(const unsigned int index){ overlayText.erase(overlayText.begin() + index); }
    inline sf::Text* getText(const unsigned int index){ return &overlayText[index]; }
    inline void clearText(){ overlayText.clear(); }
    void clear();

    void draw();
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
    APP* mainApp;
    view* View;
    sf::RenderWindow* viewPort;

    inline void newPanel(std::string panelName){
        panelList.emplace_back(this);
        panelTags.push_back(panelName); }
    void removePanel(std::string panelName);

    void addPanelElement(std::string panelName, visElement* newElement);
    GUIpanel* taggedPanel(const std::string& panelName);
    GUIpanel* operator[](const std::string& panelName){ return taggedPanel(panelName); }

    void draw();
    inline bool isVisible(){ return visible; }
    inline void setVisible(bool visibleState){ visible = visibleState; }

    bool update(CVEvent& event, const sf::Vector2f& mousePos){ return false; }

    GUI():visible(true){ }
    GUI(sf::RenderWindow* window, view* View = nullptr);
    ~GUI(){ }
};

class visElement{
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

    std::string IDtag;
    std::string triggerID;

    std::vector<sf::Sprite> spriteList;

    sf::Vector2f pinnedOffset;
    sf::Vector2f origin,
                createdOrigin;

    uint8_t state;

public:
    view* View;
    sf::RenderWindow* drawTarget;
    CVViewPanel* viewPanel;

    APP* mainApp();

    inline const bool& isVisible() const{ return visible; }
    inline void setVisible(bool visibleState){ visible = visibleState; }

    inline uint8_t getState(){ return state; };
    inline void setState(uint8_t newState){ state = newState; }

    inline bool clickable(){ return canClick; }
    inline const bool& highlightable() const{ return canHighlight; }
    inline const bool& isActive() const{ return active; }
    inline bool draggable(){ return canDrag; }
    inline bool stationary(){ return canMove; }

    inline void setClickableStatus(bool state){ canClick = state; }
    inline void setHighlightableStatus(bool state){ canHighlight = state; }
    inline void setMoveableStatus(bool state){
        canMove = state;
        if(!state && canDrag) canDrag = false; }
    inline void setDraggableStatus(bool state){
        if(state && !canMove) canMove = true;
        if(state && !canClick) canClick = true;
        canDrag = state; }

    inline bool isHighlighted(){ return highlighted; }
    inline bool isClickHeld(){ return clickHeld; }
    inline bool hasMouseHover(){ return mouseHovering; }

    virtual void setClickState(bool state){ }
    virtual void setHighlightState(bool state){ }
    virtual void setMoveState(bool state){ }
    virtual void setActiveState(bool state){ }

    // Interactivity

    inline std::string tag(){ return IDtag; }
    inline void setTag(std::string newTag){ IDtag = newTag; }

    // Positioning

    virtual void move(const sf::Vector2f& offset) = 0;
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x,y));
    }

    virtual void setPosition(const sf::Vector2f& position) = 0;
    inline void setPosition(const float& posX, const float& posY){
        setPosition(sf::Vector2f(posX, posY));
    }


    virtual sf::Vector2f getPosition() const = 0;
    inline const sf::Vector2f& getOrigin() const{ return origin; }
    inline void setOrigin(const sf::Vector2f& newOrigin){ origin = newOrigin; }

    virtual sf::Vector2f getSize() = 0;
    virtual const sf::FloatRect& getBounds() const = 0;

    virtual void rotate(const float& angle, const sf::Vector2f& origin) = 0;
    virtual void setRotation(const float& angle, const sf::Vector2f& origin) = 0;
    virtual float getRotation() const = 0;

    // Subelements and texturing

    void addSprite(const sf::Texture* texture, const sf::Vector2f& position, const sf::Vector2f& size = sf::Vector2f(0,0));
    virtual void setFillTexture(sf::Texture* texture) = 0;

    virtual bool update(CVEvent& event, const sf::Vector2f& mousePos) = 0; // Core update function for time-dependent activities (ie. Animations)
    virtual bool draw(){
        if(drawTarget == nullptr) return false;
        if(!visible) return false;

        for(auto& sprite : spriteList){
            drawTarget->draw(sprite);
        }

        return true;
    }

    visElement():
        visible(false),
        canMove(false),
        canHighlight(false),
        canClick(false),
        canDrag(false),
        active(true),
        mouseHovering(false),
        clickHeld(false),
        highlighted(false),
        origin(0.0f,0.0f),
        state(STATE_NONE),
        View(nullptr){ }

    visElement(view* View,
               bool canMove = false,
               bool canHighlight = false,
               bool canClick = false,
               bool canDrag = false,
               bool active = true);


    virtual ~visElement(){ }
};

class view{
protected:
    bool forceClose, render,
        bClosed,
        bElasticSelect;

    AIALG::StringVector saveRequestFiles;

    std::vector<visElement*> popUpElements;
    std::vector<visElement*> waitingPopUpElements;

    std::vector<CVViewPanel*> viewPanels;
    std::vector<CVViewPanel*> waitingViewPanels;
    std::vector<std::string> panelTags;

    std::vector<visElement*> splashElements;

    std::vector<CVAnim> pendingAnims;
    unsigned int numPendingAnims;

    std::vector<view*> subViews;
    std::vector<std::string> subViewTags;
    view* parentView;
    std::thread* appThread;

    unsigned int frameRateLimit;
    float frameRate, width, height;

    sf::Vector2i resizeTarget,
            resizeSpeed,
            moveTarget,
            moveSpeed;

    // Render states

    uint8_t viewState;

    // Event handling

    std::chrono::high_resolution_clock::time_point timeLeftClickPressed;
    std::chrono::high_resolution_clock::time_point timeLastLeftClick;
    std::chrono::high_resolution_clock::time_point timeRightClickPressed;
    std::chrono::high_resolution_clock::time_point timeLastRightClick;

    void handleTriggerEvent(const BYTE& eventID);

public:

    sf::RenderWindow* viewPort;
    GUI* overlay;
    APP* mainApp;

    sf::Vector2f mousePos;

    inline bool requestFocus(){ viewPort->requestFocus(); }

    inline const std::thread* mainAppThreadID() const{ return appThread; }

    inline bool closed() const{ return viewState == VIEW_STATE_CLOSED; }

    inline void setElasticSelectState(bool newState){ bElasticSelect = newState; }
    inline bool canElasticSelect() const{ return bElasticSelect; }

    inline void setState(uint8_t newState){ viewState = newState; }
    inline uint8_t getState(){ return viewState; }

    void setVisible(const unsigned int index, bool newVisibleState);
    void isolateVisible(const unsigned int index);

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

    inline void anim_passive(sf::Shape* object, const float& rate, const uint8_t animType = CV_OBJ_ANIM_FADE_OUT){
        sf::Transformable* obj_coerce = (sf::Transformable*) object;
        for(auto& anim : pendingAnims){
            if(anim.object() == obj_coerce){
                anim.setPassiveAnim(animType, rate);
                return;
            }
        }

        pendingAnims.emplace_back(object,
                                  std::vector<CVAnimCheckpoint>({
                                        CVAnimCheckpoint(object->getPosition(), 0.0f, CV_OBJ_ANIM_NONE) }));
        ++numPendingAnims;
    }
    inline void resetPassiveAnim(sf::Shape* object){
        sf::Transformable* obj_coerce = (sf::Transformable*) object;
        for(auto& anim : pendingAnims){
            if(anim.object() == obj_coerce){
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

    inline const float& getWidth() const{ return width; }
    inline const float& getHeight() const{ return height; }
    inline sf::Vector2f getSize(){ return sf::Vector2f(viewPort->getSize()); }

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
        moveSpeed = speed;

        if(moveTarget.x >= getPosition().x) moveSpeed.x = abs(moveSpeed.x);
        else moveSpeed.x = -abs(moveSpeed.x);

        if(moveTarget.y >= getPosition().y) moveSpeed.y = abs(moveSpeed.y);
        else moveSpeed.y = -abs(moveSpeed.y);
    }

    template<typename T1, typename T2> void move_to(const sf::Vector2<T1>& coords, const sf::Vector2<T2>& speed = sf::Vector2<T2>(1,1)){
        moveTarget = sf::Vector2i(coords);
        moveSpeed = speed;

        if(moveTarget.x >= getPosition().x) moveSpeed.x = abs(moveSpeed.x);
        else moveSpeed.x = -abs(moveSpeed.x);

        if(moveTarget.y >= getPosition().y) moveSpeed.y = abs(moveSpeed.y);
        else moveSpeed.y = -abs(moveSpeed.y);
    }

    template<typename T1, typename T2> void cascade(const sf::Vector2<T1>& newSizeDims,
                                 const T2& speed = sf::Vector2<T2>(1,1)){ // Resize while anchoring bottom-right
        resize_to(newSizeDims, speed);
        move_to(getPosition() + getSize() - newSizeDims, speed);
    }

    inline void addPopUp(visElement* popUpElement){ waitingPopUpElements.push_back(popUpElement); }
    inline unsigned int numPopUpElements(){ return popUpElements.size(); }

    inline void deletePopUp(visElement* popUpElement){
        for(std::vector<visElement*>::iterator it = popUpElements.begin();
            it != popUpElements.end();){
                if(*it == popUpElement){
                    delete(*it);
                    popUpElements.erase(it);
                }
                else ++it;
        }
    }

    inline const unsigned int numSubViews(){ return subViews.size(); }
    inline view* getParentView(){ return parentView; }
    inline void setParentView(view* newParentView){ parentView = newParentView; }
    void addSubView(view* newSubView, std::string tag = "");
    view* taggedSubView(std::string tag);

    inline CVViewPanel* getPanel(const unsigned int i){ return viewPanels[i]; }

    CVViewPanel* getTaggedPanel(const std::string s);
    CVViewPanel* operator[](const std::string s);

    void addPanel(CVViewPanel* newPanel, std::string tag = "");
    void setVisiblePanel(const unsigned int panelIndex);
    void setVisiblePanel(const std::string tag);
    void setVisiblePanel(CVViewPanel* panel);

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

    void sync(); // Sync draw events from external threads to the cycle of the view thread

    void draw();
    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    bool handleViewEvents(CVEvent& event);

    view(unsigned int x, unsigned int y, std::string winName,
          uint32_t style, APP* mainApp, const sf::Vector2f& screenPosition = sf::Vector2f(NAN,NAN),
          GUI* overlay = nullptr);

    ~view();
};

class APP{
protected:
    bool running;
    std::thread* mainUpdateThread;
    std::string defaultFont;
    std::mutex appSyncLock;

    inline void lock(){ appSyncLock.lock(); }
    inline void unlock(){ appSyncLock.unlock(); }

public:
    std::vector<view*> viewList;
    std::vector<std::string> viewTags;
    std::vector<GUI*> GUIlist;
    std::vector<std::string> GUITags;

    std::vector<std::thread> viewThreads;

    unsigned int frameRate;
    float frameTime;

    float leftClickLatency;
    float rightClickLatency;

    inline void setRunning(bool runState){ running = runState; }
    inline const bool getRunState(){ return running; }

    FontManager fonts;
    ImageManager bitmaps;

    inline const std::string& getDefaultFont() const{ return defaultFont; }

    // App virtuals

    virtual uint8_t loadPackages() = 0; // Load data and media required for this app to run
    virtual uint8_t initMainLoop(uint8_t runMode) = 0; // Initiate a thread consisting of the main data update loop for this app
    virtual bool initUI(unsigned int videoX, unsigned int videoY) = 0; // Initiate the UI required for this app before main loop begins

    inline view* getView(const std::string& tag){
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
        viewList.push_back(new view(x, y, winName, style, this));
        viewTags.push_back(winName);
    }

    inline bool addView(view* View, const std::string& viewTag){
        if(!AIALG::anyEqual(viewTag, viewTags)){
            viewList.push_back(View);
            viewTags.push_back(viewTag);
            return true;
        }
        else return false;
    }

    inline void setViewGUI(view* View, GUI* viewGUI){
        View->overlay = viewGUI;
    }
    void setViewGUI(std::string viewTag, std::string GUItag);

    void closeView(const unsigned int viewIndex){
        viewList[viewIndex]->close();
        unsigned int thrIndex = 0;
        for(size_t i = 0; i < viewThreads.size(); ++i, ++thrIndex){
            if(&viewThreads[i] == viewList[viewIndex]->mainAppThreadID()){
                viewThreads[i].join();
                viewThreads.erase(viewThreads.begin() + i);
            }
        }
        viewList.erase(viewList.begin() + viewIndex);
        viewTags.erase(viewTags.begin() + viewIndex);
    }

    void closeAll(){
        for(auto& View : viewList){
            View->close();
        }
        for(auto& thread : viewThreads){
            if(thread.joinable()) thread.join();
        }
        viewList.clear();
        viewTags.clear();
    }

    void mainUpdate();

    explicit APP():
        running(false),
        mainUpdateThread(nullptr),
        defaultFont(),
        frameRate(60),
        frameTime(1.0f/60),
        leftClickLatency(0.5f),
        rightClickLatency(0.5f){ }
    explicit APP(unsigned int frameRate,
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

    ~APP(){
        closeAll();
        running = false;
        if(mainUpdateThread != nullptr) mainUpdateThread->join();
    }
};

// Derived classes

class CVShape: public visElement{
protected:
    ColorTheme colorTheme;
    sf::Rect<float> bounds;
public:

    virtual sf::Vector2f getPosition() const = 0;
    void move(const sf::Vector2f& offset);

    void setPosition(const sf::Vector2f& newPosition);
    inline void move(const float& x, const float& y){ move(sf::Vector2f(x, y)); }

    virtual void rotate(const float& angle, const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) = 0;
    virtual void setRotation(const float& angle, const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) = 0;
    virtual float getRotation() const = 0;
    virtual void setSize(const sf::Vector2f& newSize){
        bounds.width = newSize.x;
        bounds.height = newSize.y;
    }

    inline const sf::FloatRect& getBounds() const{ return bounds; }

    CVShape(view* View):visElement(View){ }
    virtual ~CVShape(){ }

};

class CVBox: public CVShape{
protected:

    std::vector<sf::RectangleShape> panel;
    std::vector<sf::RectangleShape> mask;

public:

    void updateTriggers(){ }

    bool draw();

    inline sf::Vector2f getPosition() const{ return panel.front().getPosition(); }
    void setPosition(const sf::Vector2f& position);
    void move(const sf::Vector2f& offset);

    inline void move(const float& x, const float& y){ move(sf::Vector2f(x, y)); }

    void rotate(const float& angle, const sf::Vector2f& origin = sf::Vector2f(NAN, NAN));
    void setRotation(const float& angle, const sf::Vector2f& origin = sf::Vector2f(NAN, NAN));
    void setSize(const sf::Vector2f& newSize);
    inline float getRotation() const{ return panel.front().getRotation(); }

    inline const sf::RectangleShape& getPanelObject(const unsigned int order){ return panel[order]; }
    inline const sf::RectangleShape& getTopPanel(){ return panel.back(); }
    inline const sf::RectangleShape& getBottomPanel(){ return panel.front(); }
    inline unsigned int numPanelObjects() const{ return panel.size(); }

    inline const sf::Color& baseFillColor() const{ return colorTheme.front(); }
    inline const sf::Color& baseOutlineColor() const{ return colorTheme[1]; }

    void setFillTexture(sf::Texture* texture){ }
    inline sf::Vector2f getSize(){ return sf::Vector2f(bounds.width, bounds.height); }

    bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVBox(view* View, sf::Vector2f position, float width, float height,
            sf::Color fillColor, sf::Color borderColor,
            float borderWidth);

};

class CVTextBox:public CVBox{
protected:
    uint8_t textAlignment;
    textEntry textInfo;

    std::vector<sf::Text> displayText;

public:

    bool draw();
    void setPosition(const sf::Vector2f& position);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x,y));
    }
    void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y){ move(sf::Vector2f(x, y)); }

    void setTextTemplate(const textEntry& textInfo){ this->textInfo = textInfo; }
    void setText(const unsigned int textIndex, std::string newText);

    void addTextEntry(textEntry newText, float padding = 12.0f, bool regular = true);
    void addTextEntry(const textEntry& newText, const sf::Vector2f& position);

    inline const textEntry& getTextInfo(){ return textInfo; }

    inline void removeTextEntry(const unsigned int textIndex){ displayText.erase(displayText.begin() + textIndex); }

    inline const sf::Color& baseTextColor() const{ return colorTheme[2]; }

    CVTextBox(view* View, sf::Vector2f position, float width, float height,
                textEntry textInfo, sf::Color fillColor, sf::Color borderColor,
                float borderWidth);
    CVTextBox(view* View, sf::Vector2f position, float width, float height,
               sf::Color fillColor, sf::Color borderColor,
                float borderWidth);

};

class CVViewPanel: public CVTextBox{
protected:

    std::vector<visElement*> viewPanelElements;
    AIALG::StringVector viewPanelTags;
    bool bFitWindow;

public:

    void addPanelElement(visElement* newElement, std::string newTag = "");
    void removePanelElement(visElement* element);
    void removePanelElement(const std::string& tag);
    visElement* getTaggedViewPanel(const std::string& tag);

    inline const std::vector<visElement*>& getPanels() const{ return viewPanelElements; }

    inline unsigned int numPanels() const{ return viewPanelElements.size(); }

    virtual bool draw() = 0;
    virtual bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVViewPanel(view* parentView, std::string panelTag = "",
                sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                bool fitToWindow = true);
    ~CVViewPanel(){
        for(auto& item : viewPanelElements){
            delete(item);
        }
    }
};

class CVBasicViewPanel: public CVViewPanel{
public:

    bool update(CVEvent& event, const sf::Vector2f& mousePos);
    bool draw();

    CVBasicViewPanel(view* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent):
        CVViewPanel(parentView, panelTag, backgroundColor){ }
};

}

#endif // CVIS_BASE
