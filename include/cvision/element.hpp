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

#ifndef CVIS_ELEMENT
#define CVIS_ELEMENT

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "cvision/lib.hpp"

#include "cvision/trigger.hpp"
#include "cvision/color.hpp"

// Action flags

#define VIS_MOVABLE                     true
#define VIS_IMMOVABLE                   false

#define VIS_HIGHLIGHT                   true
#define VIS_NONHIGHLIGHT                false

#define VIS_ACTIVE                      true
#define VIS_INACTIVE                    false

#define VIS_CLICKABLE                   true
#define VIS_NONCLICKABLE                false

// ================================================

#define ELEMENT_CENTER_TOP_LEFT         0
#define ELEMENT_CENTERED                1
#define ELEMENT_CENTER_TOP_RIGHT        2
#define ELEMENT_CENTER_BOTTOM_LEFT      3
#define ELEMENT_CENTER_BOTTOM_RIGHT     4
#define ELEMENT_CENTER_LEFT             5
#define ELEMENT_CENTER_RIGHT            6
#define ELEMENT_CENTER_TOP              7
#define ELEMENT_CENTER_BOTTOM           8

// Layer stratification

#define CV_LAYER_ALL                    0b01111111

#define CV_LAYER_SPRITES                0b1
#define CV_LAYER_FILL                   0b10
#define CV_LAYER_OUTLINE                0b100
#define CV_LAYER_SHAPES                 0b1000
#define CV_LAYER_HIGHLIGHT              0b10000
#define CV_LAYER_TEXT                   0b100000
#define CV_LAYER_MASK                   0b1000000
#define CV_LAYER_MEMBERS                0b10000000

// Development shortcuts

#define CV_UPDATE_OVERRIDE  \
    bool update(cvis::CVEvent& event, const sf::Vector2f& mousePos) override

#define CV_DEFINE_UPDATE(class) \
    bool class::update(CVEvent& event, const sf::Vector2f& mousePos)

#define CV_DRAW_OVERRIDE    \
    bool draw(sf::RenderTarget* target) override

#define CV_DEFINE_DRAW(class)   \
    bool class::draw(sf::RenderTarget* target)

namespace cvis
{

typedef std::map<std::string, int> KeyMapping; /**< @brief General function-key mapping by index */

class CVView;
class CVViewPanel;
class CVApp;
class CVEvent;

/** @brief Common CVElement virtual base class.
  *
  * All CVision elements inherit from this class.
  * Contains methods for drawing, updating, physics,
  * sprite addition, animation, and many others.
  *
  * Create custom classes by inheriting from this class
  * and then defining the pure virtual draw() and update()
  * methods.
  */

class CVISION_API CVElement
{
public:

    CVISION_API CVElement();
    CVISION_API CVElement(CVView* View,
              bool canHighlight = false,
              bool canClick = false,
              bool canDrag = false,
              bool active = true);


    virtual ~CVElement();

    /** ========================================================================

        Cascade interaction

    // ===================================================================== **/

    CVView* View;
    sf::RenderWindow* drawTarget;
    CVViewPanel* viewPanel;

    CVISION_API const CVApp* mainApp() const;
    CVISION_API CVElement* getElementById(const std::string& tag);

    CVISION_API const sf::Font* appFont(const std::string& font) const;
    CVISION_API const sf::Texture* appTexture(const std::string& tag) const;
    CVISION_API const sf::Image* appImage(const std::string& tag) const;
    CVISION_API const sf::Color& appColor(const std::string& tag) const;

    CVISION_API void setViewCursor(const sf::Texture* texture,
                                   const sf::Vector2f& size = sf::Vector2f(24.0f, 24.0f),
                                   const sf::Color& color = sf::Color::White,
                                   const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f));
    CVISION_API void setViewCursor(const std::string& texture,
                                   const sf::Vector2f& size = sf::Vector2f(24.0f, 24.0f),
                                   const sf::Color& color = sf::Color::White,
                                   const sf::Vector2f& origin = sf::Vector2f(0.0f, 0.0f));

    CVISION_API std::string getDefaultFont() const;

    template<typename T> bool isType()
    {
        return dynamic_cast<T*>(this) != nullptr;
    }

    inline const bool& isVisible() const
    {
        return visible;
    }
    inline void setVisible(bool visibleState)
    {
        visible = visibleState;
    }

    CVISION_API float viewScale() const;
    CVISION_API void setSpriteScale(const float& newScale);

    /** ========================================================================

        Trigger event handling

    // ===================================================================== **/

    inline void receive_trigger(const std::string& newTrigger)
    {
        incoming_triggers.emplace_back(newTrigger);
    }
    CVISION_API std::string take_trigger(const std::string& newTrigger); // Attempt to get a trigger and extract its information
    inline bool trigger_waiting() const{ return !incoming_triggers.empty(); }

    inline void addTriggerTarget(CVElement* target,
                                 const std::string& signal,
                                 const unsigned int& state = UINT_MAX)
    {
        trigger_targets.emplace_back(target, signal, state);
    }

    inline void addTriggerTarget(const std::string& ID,
                                 const std::string& signal,
                                 const unsigned int& state = UINT_MAX)
    {
        trigger_targets.emplace_back(getElementById(ID), signal, state);
        trigger_targets.back().tag = ID;
    }

    CVISION_API void sendTriggers();
    CVISION_API void sendTrigger(CVElement* target, const std::string& signal);

    inline bool getTrigger(){
        if(bTriggered){
            bTriggered = false;
            return true;
        }
        return false;
    }

    CVISION_API void setClosable(const bool& status = true,         // Enable closable signal
                                 CVElement* element = nullptr);     // Default signal is an "x" sprite CVButton

    inline bool is_closable() const
    {
        return closeButton;
    }

    /** ========================================================================

        States

    // ===================================================================== **/

    inline const bool& shouldDelete() const{ return bDelete; }

    inline virtual const unsigned int& getCurrentState() const noexcept
    {
        return stateNum;
    }
    inline const unsigned int& getNumStates() const
    {
        return numStates;
    }
    inline void setNumStates(const unsigned int& N)
    {
        numStates = N;
    }

    inline uint8_t getState()
    {
        return state;
    };

    CVISION_API virtual void setState(const uint8_t& newState); // Toggle to a new state
    CVISION_API void setFade(const uint8_t& alpha, const int& rate, const unsigned char& flags = CV_LAYER_ALL);
    CVISION_API virtual bool fadeComplete() const noexcept;
    inline bool isFading() const noexcept{ return !fadeComplete(); }

    inline void setFadeStatus(const bool& state, const unsigned char& flags = CV_LAYER_ALL)
    {
        bFade = state;
    }
    CVISION_API void setDrawClipping(const bool& state);

    inline bool clickable()
    {
        return canClick;
    }
    inline const bool& highlightable() const
    {
        return canHighlight;
    }
    inline const bool& isActive() const
    {
        return active;
    }
    inline bool draggable()
    {
        return canDrag;
    }
    inline bool stationary()
    {
        return bStatic;
    }

    CVISION_API void highlight(const bool& state);
    virtual inline void setHighlightableStatus(const bool& state)
    {
        if(state) bNoInteract = false;
        canHighlight = state;
    }
    inline void setDraggableStatus(bool state)
    {
        if(state)
        {
            bStatic = false;
            canClick = true;
            bNoInteract = false;
        }
        canDrag = state;
    }
    virtual inline void setDragAndDrop(const bool& state)
    {
        if(state) bNoInteract = false;
        bDragAndDrop = state;
    }

    virtual inline void setHighlightColor(const sf::Color& color)
    {
        highlightColor = color;
    }

    inline const bool& isHighlighted() const
    {
        return highlighted;
    }
    inline const bool& isClickHeld() const
    {
        return clickHeld;
    }
    inline const bool& hasMouseHover() const
    {
        return mouseHovering;
    }
    inline const bool& hasFocus() const
    {
        return bHasFocus;
    }

    virtual void setClickState(bool state) { }
    virtual void setHighlightState(bool state) { }
    virtual void setMoveState(bool state) { }

    inline void setStatic(const bool& state = true)
    {
        bStatic = state;
    }
    inline void setInteractable(const bool& state = true)
    {
        bNoInteract = !state;
    }

    /** Set the active status of the element */
    virtual inline void setActiveState(const bool& state)
    {
        active = state;
    }

    inline void setSpriteOnly(const bool& state = true)
    {
        bSpriteOnly = state;
    }

    virtual inline void setFocus(const bool& state)
    {
        bHasFocus = state;
    }
    inline void setMouseFollow(const bool& followX, const bool& followY)
    {
        if(followX || followY)
        {
            bNoInteract = false;
        }
        bFollowMouseX = followX;
        bFollowMouseY = followY;
    }

    CVISION_API void setDropShadow(const bool& state = true,
                                   const int& radius = 6,
                                   const uint8_t& alpha = 100,
                                   const sf::Vector2f& offset = sf::Vector2f(4.0f, 4.0f),
                                   const sf::Vector2f& scale = sf::Vector2f(1.0f, 1.0f));

    /** ========================================================================

        Interactivity

    // ===================================================================== **/

    inline const std::string& tag() const noexcept{ return IDtag; } /**< @brief Get the unique ID assigned to this element. */
    inline void setTag(const std::string& newTag) noexcept{ IDtag = newTag; } /**< @brief Set a unique ID for this element. */

    /** ========================================================================

        Position

    // ===================================================================== **/

    CVISION_API virtual void move(const sf::Vector2f& offset);
    inline void move(const float& x, const float& y){ move(sf::Vector2f(x,y)); }

    inline virtual void setPosition(const sf::Vector2f& position){ move(position - getPosition()); }
    inline void setPosition(const float& posX, const float& posY) noexcept{ setPosition(sf::Vector2f(posX, posY)); }

    CVISION_API virtual void setExpand(const bool& state);
    inline const sf::FloatRect& getBounds() const noexcept{ return bounds; }
    inline const sf::FloatRect& getGlobalBounds() const { return getBounds(); } /**< @brief Interface with templated SFML functions. */

    CVISION_API virtual sf::Vector2f getPosition() const;
    inline const sf::Vector2f& getOrigin() const noexcept{ return origin; }
    inline void setOrigin(const sf::Vector2f& newOrigin){ origin = newOrigin; }

    inline virtual sf::Vector2f getSize(){ return fTrueSize; }
    CVISION_API virtual void setSize(const sf::Vector2f& newSize);
    inline void setSize(const float& x, const float& y) noexcept{ setSize(sf::Vector2f(x, y)); }

    virtual float getOutlineThickness() const = 0;

    virtual void rotate(const float& angle,
                        const unsigned char& flags = CV_LAYER_ALL,
                        const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) = 0;
    virtual void setRotation(const float& angle,
                             const unsigned char& flags = CV_LAYER_ALL,
                             const sf::Vector2f& origin = sf::Vector2f(NAN, NAN)) = 0;
    virtual float getRotation(const unsigned char& flags = CV_LAYER_ALL) const = 0;

    /** ========================================================================

        Movement and physics

    // ===================================================================== **/

    inline void setBounce(const bool& state){ bBounce = state; }
    inline void forceOnScreen(const bool& state = true){ bForceOnScreen = state; }

    CVISION_API void setElasticity(const float& newElasticity);
    inline void setFriction(const float& newCoefficient) noexcept{ fFriction = newCoefficient; }
    inline void setVelocity(const sf::Vector2f& newVelocity) noexcept{ velocity = newVelocity; }
    inline void setAcceleration(const sf::Vector2f& newAcceleration) noexcept{ acceleration = newAcceleration; }

    CVISION_API float speed();

    CVISION_API void stop();    /**< @brief Stop all motion for this element. */
    CVISION_API void move_to(const sf::Vector2f& position,
                 const float& velocity,
                 const float& drag = 0.0f);    /**< @brief Push toward destination. */
    CVISION_API void anim_move(const sf::Vector2f& distance,
                 const float& velocity,
                 const float& drag = 0.0f);    /**< @brief Animated move by a certain distance. */

    /** @brief Push the element in the direction of [angle] (in radians) and impart
      * components of scalar [velocity] instantaneously.
      *
      * Optionally, add
      * a drag coefficient to the motion that will eventually slow down the
      * moving element.
      *
      * @param angle    angle, in radians, of the push
      * @param velocity velocity, in pixels per second, to be imparted
      * @param drag     drag, in -pixels/second, to be imparted
      */
    CVISION_API void push(const float& angle,
              const float& velocity,
              const float& drag = 0.0f);        // Push in a direction

    /** @brief Amplify/dampen velocity by a scaling factor */
    CVISION_API void scale_velocity(const float& proportion);

    /** ========================================================================

        Subelements and texturing

    // ===================================================================== **/

    CVISION_API void addSprite(const sf::Texture* texture,
                   const sf::Vector2f& position,
                   const sf::Vector2f& size = sf::Vector2f(0,0),
                   const sf::Color& fillColor = sf::Color::White,
                   const sf::IntRect& subRect = sf::IntRect(0, 0, 0, 0));       // Add a new sprite
    CVISION_API void removeSprites(const std::string& tag);                         // Remove all sprites with this tag
    CVISION_API bool has_sprite(const std::string& tag) const;                     // Check if a sprite of this type has been added

    inline const std::vector<sf::Sprite>& sprites() const
    {
        return spriteList;
    }
    CVISION_API sf::Sprite& lastSprite();
    CVISION_API sf::Sprite& firstSprite();
    CVISION_API sf::Sprite& getSprite(const unsigned int& index);

    inline void removeAllSprites()
    {
        spriteList.clear();
    }
    inline void removeLastSprite()
    {
        if(spriteList.size() > 0) spriteList.pop_back();
    }
    inline void removeFirstSprite()
    {
        if(spriteList.size() > 0) spriteList.erase(spriteList.begin());
    }

    CVISION_API void setSpriteSize(const size_t& index,
                                   const sf::Vector2f& newSize,
                                   const bool& maintainRatio = false);

    virtual void setFillTexture(sf::Texture* texture) = 0;
    inline sf::Color baseFillColor() const
    {
        if(colorTheme.size() > 0)
            return colorTheme.front();
        return sf::Color::Transparent;
    }
    inline sf::Color baseOutlineColor() const
    {
        if(colorTheme.size() > 1)
            return colorTheme[1];
        return sf::Color::Transparent;
    }
    inline const sf::Color& baseSpriteColor() const
    {
        return spriteColor;
    }
    inline sf::Color inactiveColor() const
    {
        if(colorTheme.size() > 3)
        {
            return colorTheme[3];
        }
        return sf::Color(128, 128, 128, 170);
    }

    CVISION_API virtual bool update(CVEvent& event, const sf::Vector2f& mousePos); // Core update function for time-dependent activities (ie. Animations)
    CVISION_API virtual bool draw(sf::RenderTarget* target);

    CVISION_API virtual void getTexture(sf::Texture& output,
                                        const sf::Color& canvas_color = sf::Color::Transparent); // Get an image of the current draw state
    CVISION_API bool saveImage(const std::string& save_file);

    CVISION_API void createShadow(const uint8_t& alpha, const float& drawScale = 1.0f); // Create a shadow using a draw texture
    CVISION_API void removeShadow();    // Disable shadow rendering

    virtual void sendData(CVEvent& event) const = 0;

    virtual inline void setColor(const sf::Color& newColor)
    {
        if(colorTheme.empty()) colorTheme.emplace_back(newColor);
        else colorTheme.front() = newColor;

        for(auto& sprite : spriteList)
        {
            sprite.setColor(newColor);
        }

        highlightColor = newColor;
        brighten(highlightColor, 40);

        spriteColor = newColor;
    }
    inline void setColor(const int& r, const int& g, const int& b)
    {
        setColor(sf::Color(r,g,b));
    }

    virtual inline sf::Color getFillColor() const{ return sf::Color::Transparent; }
    virtual inline sf::Color getOutlineColor() const{ return sf::Color::Transparent; }

    virtual inline sf::Color getSpriteColor() const
    {
        if(!spriteList.empty())
        {
            return spriteList.back().getColor();
        }
        else return sf::Color::Transparent;
    }

    inline void setControls(const KeyMapping& newControls)
    {
        controls = newControls;
    }
    inline void setControlKey(const std::string& controlID, const sf::Keyboard::Key& key)
    {
        controls[controlID] = key;
    }
    inline int getKey(const std::string& controlID)
    {
        try
        {
            return controls[controlID];
        }
        catch(...)
        {
            return sf::Keyboard::Unknown;
        }
    }

protected:

    /** ========================================================================

        Flags

    // ===================================================================== **/

    bool visible;            /**< Is this item drawn to the renderer? */

    bool canHighlight;       /**< Can the item be illuminated? */
    bool canClick;           /**< Is the item clickable? */
    bool canDrag;            /**< Can the item be dragged? */
    bool active;             /**< Is the item active? */

    bool mouseHovering;      /**< Is the mouse hovering over this element? */
    bool clickHeld;          /**< Is the mouse button being held? */
    bool highlighted;        /**< Is the current highlight state active? */

    bool bHasFocus;          /**< Has the item successfully captured the event cascade? */
    bool bFollowMouseX;      /**< Set the X position to the mouse position while flag is TRUE */
    bool bFollowMouseY;      /**< Set the Y position to the mouse position while flag is TRUE */
    bool bFade;              /**< If true, update the object alpha */
    bool bMove;              /**< If true, update the object position */
    bool bSpriteOnly;        /**< If true, do not render primitives */
    bool bExpand;            /**< If true, expand bounds to encompass member elements */
    bool bStop;              /**< When velocity reaches zero, stop instead of turning */
    bool bBounce;            /**< Bounce off the view boundaries instead of flying off */
    bool bForceOnScreen;     /**< Keep this item on screen */
    bool bHasShadow;         /**< Has stationary texture copy */
    bool bDragAndDrop;       /**< Allow drag-and-drop mechanics */
    bool bClipBounds;        /**< Clip draw region to bounds */
    bool bDelete;            /**< A close call has been ordered on this element */
    bool bTriggered;         /**< Has a generic trigger event been initiated on this element? */
    bool bDropShadow;        /**< Does this item have a drop shadow? */

    bool bStatic;            /**< Cannot be moved */
    bool bNoInteract;        /**< Skip update cycles (enhance overall performance if this element is not being used/drawn) */

    /** ========================================================================

        Trigger elements

    // ===================================================================== **/

    std::string                     IDtag;              /**< ID identifying this element's class */
    std::string                     triggerID;          /**< Specific ID tag for trigger targets */

    std::vector<std::string>        incoming_triggers;  /**< Trigger signals waiting to be processed */
    std::vector<CVTriggerTarget>    trigger_targets;    /**< Trigger targets to send information to */

    CVElement* closeButton;                             /**< An element that can act as a close trigger signal */

    /** ========================================================================

        Info

    // ===================================================================== **/

    std::vector<sf::Sprite>         spriteList;         /**< Sprites under the control of this element, add to this list (ie. pawns) */
    sf::Texture                     clipTexture;        /**< Texture draw target for clipping */
    sf::Texture                     dropShadowTexture;  /**< Texture for drop shadow if applicable */

    sf::Sprite                      shadow;             /**< Shadow of this item: use for drag and drop, etc. */
    sf::Sprite                      drawMask;           /**< Clipping mask if region outside of bounds is to be excluded from the render */
    sf::Sprite                      dropShadow;         /**< Rastered drop shadow based on draw texture */

    ColorTheme                      colorTheme;         /**< Main element colors */
    sf::Color                       highlightColor;     /**< Highlight color if applicable */
    sf::Color                       spriteColor;        /**< Base color of main sprites */

    sf::Vector2f                    pinnedOffset;
    sf::Vector2f                    origin;
    sf::Vector2f                    createdOrigin;

    /** ========================================================================

        Physics

    // ===================================================================== **/

    sf::Vector2f                    destination;
    sf::Vector2f                    velocity;
    sf::Vector2f                    acceleration;   /**< Absolute acceleration (ie. gravity, buoyancy) */

    float                           fMoveAngle;
    float                           fElasticity;    /**< 0 - 1; how much momentum is conserved during bounce interactions */
    float                           fFriction;      /**< coefficient of friction translating to -pixels/s^2 */
    float                           fSpriteScale;

    uint8_t                         state;
    uint8_t                         targetAlpha;

    unsigned char                   fadeLayers;     /**< Layers to apply fade to */

    unsigned int                    stateNum;
    unsigned int                    numStates;

    int                             fadeRate;

    sf::FloatRect                   bounds;
    CVISION_API virtual void updateBounds();

    KeyMapping                      controls;

    /** ========================================================================

        Position/size/scale

    // ===================================================================== **/

    sf::Vector2f                    fTruePos;   /**< @brief True position of the element. */
    sf::Vector2i                    iDrawPos;   /**< @brief Rounded position of the element (for clear draw). */

    sf::Vector2f                    fTrueSize;  /**< @brief True size of the element. */
    sf::Vector2i                    iDrawSize;  /**< @brief Rounded size of the element (for clear draw). */

};

}

#endif // CVIS_ELEMENT
