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

#ifndef KYC_APP
#define KYC_APP

#include <string>
#include <vector>

#include "cvision/app.hpp"

#include "KYC/network/user.hpp"

#define IMPORT_SUCCESS          0b11
#define IMPORT_FONT_SUCCESS     0b1
#define IMPORT_IMAGE_SUCCESS    0b10
#define IMPORT_FAIL             0b0

#define EXIT_CODE_SUCCESS       0
#define EXIT_CODE_FAIL          -1
#define EXIT_CODE_TERMINATE     1

namespace KYC
{

class KYCView;
class KYCLoginPanel;
class KYCJoinForm;

class KYCApp : public cvis::CVApp
{
public:

    uint8_t initMainLoop(uint8_t runMode = 0) override;
    uint8_t loadPackages() override;
    bool initUI(unsigned int videoX, unsigned int videoY) override;

    void mainLoop();
    void update();

    void setUser(const client& newUser);
    const client& getLoggedUser() const;

    KYCApp();
    ~KYCApp();

protected:

    bool bTerminate,
        bRunning;

    KYCView* viewPort;

    friend class KYCLoginPanel;
    friend class KYCJoinForm;

    client_base users;
    client logged_user;

};

}

#endif // KYC_APP
