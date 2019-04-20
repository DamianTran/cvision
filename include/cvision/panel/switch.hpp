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

#ifndef CVIS_SWITCH_PANEL
#define CVIS_SWITCH_PANEL

#include "cvision/panel.hpp"

namespace cvis
{

class CVISION_API CVSwitchPanel: public CVBasicViewPanel{ // Allows for switching of view focus between panel elements
protected:
    int viewIndex;

    bool bCanPan,
        bAutoPan,
        bCenterOnNew,
        bClosablePanels,
        bLoop,
        bTimed;

    float panRate,
            elementPadding,
            autoSwitchLatency;

    std::chrono::high_resolution_clock::time_point timeLastSwitch;

public:

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);

    CVISION_API CVElement* getActiveElement();
    CVISION_API std::string getFocusTag();

    inline void enableAutoSwitch()
    {
        bTimed = true;
    }
    inline void disableAutoSwitch()
    {
        bTimed = false;
    }

    inline void setAutoSwitchTime(const float& newTime)
    {
        bTimed = newTime > 0.0f;
        autoSwitchLatency = newTime;
    }

    inline void setSwitchLoopable(const bool& status = true)
    {
        bLoop = status;
    }

    void setCenter(const int& index);
    inline void switch_to(const int& index)
    {
        setCenter(index);
    }

    CVISION_API void setCenter(const std::string& tag);
    void switch_to(const std::string& tag)
    {
        setCenter(tag);
    }

    CVISION_API void switch_next();
    CVISION_API void switch_previous();

    CVISION_API void setPanRate(const float& newRate);
    CVISION_API bool checkFocus(const std::string& tag); // Check if the view is focused on a tagged panel

    CVISION_API void setElementPadding(const float& newPadding);

    CVISION_API void addPanelElement(CVElement* newElement, std::string newTag = "", const unsigned int& index = UINT_MAX);
    CVISION_API void removePanelElement(const unsigned int& index);

    CVISION_API void setPosition(const sf::Vector2f& newPosition);
    inline void setPosition(const float& x, const float& y){
        setPosition(sf::Vector2f(x, y));
    }

    CVISION_API void move(const sf::Vector2f& distance);
    inline void move(const float& x, const float& y){
        move(sf::Vector2f(x, y));
    }

    CVISION_API void enablePanning(); // Allow the user to pan the panel around
    CVISION_API void disablePanning();

    CVISION_API void enableAutoPan();
    CVISION_API void disableAutoPan();

    inline void setClosablePanels(const bool& state = true)
    {
        bClosablePanels = state;
    }

    inline void setCenterOnNew(const bool& state = true)
    {
        bCenterOnNew = state;
    }

    CVISION_API CVSwitchPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

};

}

#endif // CVIS_SWITCH_PANEL
