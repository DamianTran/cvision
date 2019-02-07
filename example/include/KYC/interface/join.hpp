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

#ifndef KYC_JOIN
#define KYC_JOIN

#include "cvision/panel.hpp"

namespace KYC
{

class KYCJoinForm : public cvis::CVBasicViewPanel
{
public:

    KYCJoinForm(cvis::CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color(255,255,255,0),
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    cvis::CVTypeBox*          titleEntry;
    cvis::CVTypeBox*          firstNameEntry;
    cvis::CVTypeBox*          lastNameEntry;
    cvis::CVTypeBox*          userNameEntry;
    cvis::CVTypeBox*          emailEntry;
    cvis::CVTypeBox*          passwordEntry;
    cvis::CVTypeBox*          confirmPassEntry;

    cvis::CVButton*             submitButton;

    bool update(cvis::CVEvent& event, const sf::Vector2f& mousePos) override;

    bool checkFields();

};

class KYCJoinPanel : public cvis::CVBasicViewPanel
{
public:

    KYCJoinPanel(cvis::CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color(255,255,255,0),
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    KYCJoinForm*                form;

    cvis::CVSwitchPanel*        adDisplay;

    cvis::CVButton*             backButton;

};

}

#endif // KYC_JOIN
