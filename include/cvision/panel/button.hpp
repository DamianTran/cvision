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

#ifndef CVIS_BUTTON_PANEL
#define CVIS_BUTTON_PANEL

#include "cvision/panel.hpp"

namespace cvis
{

class CVButton;

/** Button panel which acts as a placer and manager for several
    coordinated button class entities.  Buttons are automatically
    constructed using the arguments in addButton(...).

    Button groups allow for further automation of complex tasks
    such as restricting button highlighting to buttons of a certain
    type (ie. play, pause, stop vs low, medium, high) */

class CVISION_API CVButtonPanel: public CVBasicViewPanel{
public:

    CVISION_API CVButtonPanel(CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color::Transparent,
                             const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                             const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    CVISION_API std::string getInteraction();
    CVISION_API std::string getSelectedFromGroup(const std::string& group) const noexcept;
    CVISION_API void clearSelectedFromGroup(const std::string& group) noexcept;
    CVISION_API bool checkInteraction() const;

    inline sf::Color& buttonFillColor() noexcept{ return colorTheme[buttonFillColorIndex]; }
    inline sf::Color& buttonOutlineColor() noexcept{ return colorTheme[buttonOutlineColorIndex]; }

    CVISION_API void setHighlight(const sf::Vector2i& coords);
    CVISION_API void clearHighlight();

    CVISION_API void addButtonToGroup(const sf::Vector2i& coords,
                                      const std::string& group) noexcept;
    CVISION_API void removeButtonFromGroup(const sf::Vector2i& coords,
                                           const std::string& group) noexcept;
    CVISION_API void removeButtonGroup(const std::string& group) noexcept;
    CVISION_API void createButtonGroup(const std::string& group) noexcept;

    CVISION_API void setGrid(const sf::Vector2i& size);
    CVISION_API void setGrid(const unsigned int& x, const unsigned int& y);

    CVISION_API void setOuterPadding(const float& value);
    CVISION_API void setInnerPadding(const float& value);

    CVISION_API void setSpriteColor(const sf::Color& newColor);
    CVISION_API void setButtonFillColor(const sf::Color& newColor);
    CVISION_API void setButtonOutlineColor(const sf::Color& newColor);

    CVISION_API void setButtonSpriteScale(const float& newScale);
    CVISION_API void setButtonRounding(const float& rounding_radius,
                                       const size_t& pointCount = 12,
                                       const std::vector<bool>& corner_states =
                                       std::vector<bool>(4, true));
    CVISION_API void setButtonOutlineThickness(const float& newThickness);

    CVISION_API void addButton(const sf::Vector2i& coord,
                               const std::string& tag = "",
                               const std::string& icon = "",
                               const std::string& text = "",
                               const unsigned int& numStates = 2,
                               const unsigned int& initialState = 0);
    CVISION_API CVButton* getButton(const sf::Vector2i& coord);
    CVISION_API const CVButton* getButton(const sf::Vector2i& coord) const;

    CVISION_API bool update(CVEvent& event, const sf::Vector2f& mousePos);
    CVISION_API bool draw(sf::RenderTarget* target);

protected:

    std::string interactTag;
    sf::Vector2i gridSize;

    CVISION_API void organizeGrid();
    CVISION_API sf::Vector2f getGridPosition(const sf::Vector2i& coord) const;

    std::vector<std::vector<CVButton*>> buttonGrid;

    std::map<std::string, std::vector<CVButton*>> buttonGroups;

    float       outerPadding;
    float       innerPadding;
    float       fButtonRoundingRadius;
    float       fButtonOutlineThickness;

    size_t      buttonFillColorIndex;
    size_t      buttonOutlineColorIndex;
    size_t      buttonRoundPoints;

    std::vector<bool>   buttonRoundStates;

    CVISION_API bool buttonsShareGroup(const CVButton* btn1, const CVButton* btn2) const noexcept;
    inline bool buttonsShareGroup(const sf::Vector2i& btn1Coords,
                                  const sf::Vector2i& btn2Coords) const noexcept
    {
        return buttonsShareGroup(getButton(btn1Coords), getButton(btn2Coords));
    }

    CVISION_API bool buttonBelongsToGroup(const sf::Vector2i& coords) const noexcept;

};

}

#endif // CVIS_BUTTON_PANEL
