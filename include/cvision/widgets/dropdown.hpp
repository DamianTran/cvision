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

#ifndef CVIS_DROPDOWN
#define CVIS_DROPDOWN

#include <cvision/panel.hpp>

namespace cvis
{

class CVTypeBox;
class CVButton;
class CVListPanel;

/** CVision drop-down menu with optional dialog type entry */

class CVISION_API CVDropDownBox : public CVBasicViewPanel
{
public:

    CVISION_API CVDropDownBox(cvis::CVView * View,
                              const std::string& tag = "",
                              const std::vector<std::string>& selection =
                                    std::vector<std::string>(),
                              const sf::Vector2f& position = sf::Vector2f(0.0f, 0.0f),
                              const sf::Vector2f& size = sf::Vector2f(0.0f, 0.0f),
                              const TextEntry& textInfo = TextEntry("", ""),
                              const sf::Color& fillColor = sf::Color::Transparent,
                              const sf::Color& buttonFillColor = sf::Color::Transparent,
                              const sf::Color& outlineColor = sf::Color::Transparent,
                              const float& outlineThickness = 0.0f,
                              const float& rounding = 8.0f,
                              const float& maxDropDownHeight = 256.0f,
                              const std::string& dropDownIcon = "angle_down");

    CVISION_API bool update(cvis::CVEvent& event, const sf::Vector2f& mousePos);

    cvis::CVTypeBox *       dropDownDialog;
    cvis::CVButton *        dropDownBtn;
    cvis::CVListPanel *     dropDownMenu;

    inline bool dropMenuVisible() const noexcept{ return bDropDownVisible; }

    inline const std::vector<std::string>& getMenu() const noexcept{ return selection_list; }
    CVISION_API void addMenuItem(const std::string& newEntry);

    CVISION_API std::string getSelected() const;
    CVISION_API void setSelected(const std::string& value);
    CVISION_API void setSelected(const unsigned int& index);

    CVISION_API void openMenu();
    CVISION_API void closeMenu();
    CVISION_API void toggleMenu();

protected:

    bool                        bDropDownVisible;               //  Is the drop down menu currently visible?
    bool                        bTypeEntry;                     //  Can the user type their own entry into the field box?
    bool                        bAutoFill;                      //  If bTypeEntry, auto-fill the field with potential list item candidates
    bool                        bAutoScroll;                    //  If bTypeEntry and bDropDownVisible, automatically scroll the list to the best match every keystroke

    float                       fMaxDropDownHeight;             //  Maximum height for the drop down menu before scroll bar appears
    float                       fListItemSpacing;               //  Spacing between drop-down menu items

    std::vector<std::string>    selection_list;                 //  Contents of the drop-down menu

    unsigned int                selected_index;                 //  The currently selected menu item

};

}

#endif // CVIS_DROPDOWN
