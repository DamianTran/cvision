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

#ifndef KYC_VIEW
#define KYC_VIEW

#include "cvision/view.hpp"
#include "cvision/widgets.hpp"
#include "cvision/panel.hpp"
#include "cvision/panel/toggle.hpp"

namespace KYC
{

class KYCApp;
class KYCLoginPanel;
class KYCDashboard;
class KYCJoinPanel;
class KYCSearchPanel;
class KYCMenu;

class KYCView : public cvis::CVView
{
public:

    cvis::CVTitleBar*               titleBar;

    cvis::CVBasicViewPanel*         fgPanel;

    KYCLoginPanel*                  loginPanel;
    KYCDashboard*                   dashboard;
    KYCJoinPanel*                   joinPanel;
    KYCSearchPanel*                 searchPanel;
    KYCMenu *                       menuPanel;

    cvis::CVSwitchPanel*            bgPanel;

    KYCView(KYCApp* app);

};

}

#endif //KYC_VIEW
