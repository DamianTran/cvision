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

#ifndef CVIS_VIEW_PANEL
#define CVIS_VIEW_PANEL

#include <SFML/Graphics.hpp>

#include "cvision/textbox.hpp"

namespace cvis
{

class CVEvent;

class CVISION_API CVViewPanel: public CVTextBox
{
public:

    CVISION_API CVViewPanel(CVView* parentView, std::string panelTag = "",
                sf::Color backgroundColor = sf::Color::Transparent,
                const sf::Vector2f& size = sf::Vector2f(NAN, NAN),
                bool fitToWindow = true,
                const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    inline const std::vector<CVElement*>& getElements() const
    {
        return viewPanelElements;
    }

    virtual void addPanelElement(CVElement* newElement, std::string newTag = "", const unsigned int& index = UINT_MAX);
    virtual void removePanelElement(CVElement* element);
    virtual void removePanelElement(const std::string& tag);
    virtual void removePanelElement(const unsigned int& index);

    virtual void detachPanelElement(const CVElement* element) noexcept;

    virtual void clear();

    CVISION_API CVElement* getOwnedElementByID(const std::string& ID);
    CVISION_API CVElement* getTaggedElement(const std::string& tag);
    CVISION_API bool elementExists(const std::string& tag);

    inline void setFadeMembersOnly(const bool& status = true){ bFadeMembersOnly = status; }

    inline const std::vector<CVElement*>& getPanels() const
    {
        return viewPanelElements;
    }
    virtual void setHighlightableStatus(const bool& status) override;

    inline void setOutOfBoundsUpdate(const bool& state = true)
    {
        bOutOfBoundsUpdate = state;
    }
    inline void setOutOfBoundsDraw(const bool& state = true)
    {
        bOutOfBoundsDraw = state;
    }

    CVISION_API void setFocus(const bool& state) override;

    inline unsigned int numPanels() const
    {
        return viewPanelElements.size();
    }

    CVISION_API void move(const sf::Vector2f& distance) override;
    inline void move(const float& x, const float& y)
    {
        move(sf::Vector2f(x, y));
    }

    CVISION_API void setPosition(const sf::Vector2f& position) override;
    inline void setPosition(const float& x, const float& y)
    {
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void setSize(const sf::Vector2f& newSize) override;
    inline void setSize(const float& x, const float& y)
    {
        setSize(sf::Vector2f(x, y));
    }

    CVISION_API void fitElements(const bool& fitX = true,
                                 const bool& fitY = true); // Fit the panel boundaries to the attached elements

    CVISION_API void setHighlightColor(const sf::Color& color) override;
    inline void setHighlightColor(const int& r, const int& g, const int& b)
    {
        setHighlightColor(sf::Color(r, g, b));
    }

    CVISION_API void setExpand(const bool& state) override;

    virtual bool draw(sf::RenderTarget* target) override = 0;
    virtual bool update(CVEvent& event, const sf::Vector2f& mousePos) override;

    ~CVViewPanel();

protected:

    std::vector<CVElement*>     viewPanelElements;
    std::vector<std::string>    viewPanelTags;

    bool                        bFitWindow;            // Fit to the window upon creation and upon resize
    bool                        bTransduceFade;        // Transduce fade events to panel elements
    bool                        bTransduceFocus;
    bool                        bDragAndDrop;          // Transfer focus to panel elements
    bool                        bOutOfBoundsDraw;      // Draw items outside of the panel boundary
    bool                        bOutOfBoundsUpdate;    // Update items outside of the panel boundary
    bool                        bFadeMembersOnly;

    sf::Sprite*                 dragShadow;     // If drag-and-drop, a sprite for the item shadow

    CVISION_API virtual void updateBounds() override;
    CVISION_API void updatePanels(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API bool fadeComplete() const noexcept override;

private:

    CVViewPanel(const CVViewPanel& other) = delete;
    CVViewPanel& operator=(const CVViewPanel& other) = delete;

};

}

#endif // CVIS_VIEW_PANEL
