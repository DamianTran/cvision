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

#include "KYC/interface/view.hpp"
#include "KYC/interface/app.hpp"
#include "KYC/interface/login.hpp"
#include "KYC/interface/dashboard.hpp"
#include "KYC/interface/join.hpp"
#include "KYC/interface/search.hpp"
#include "KYC/interface/menu.hpp"

using namespace cvis;

namespace KYC
{

KYCView::KYCView(KYCApp* app):
    CVView(sf::VideoMode::getDesktopMode().width*3/4,
           sf::VideoMode::getDesktopMode().height*7/8,
           "Keystone ID View", sf::Style::None, app,
           sf::Vector2f(sf::VideoMode::getDesktopMode().width/20,
                        sf::VideoMode::getDesktopMode().height/100)),
    titleBar(nullptr),
    fgPanel(nullptr),
    loginPanel(nullptr),
    dashboard(nullptr),
    joinPanel(nullptr),
    searchPanel(nullptr),
    bgPanel(nullptr)
{

    #ifdef __APPLE__
    setTopMargin(height/24);
    #else
    setTopMargin(height/22);
    #endif

    sf::Color borderColor = sf::Color(25,25,25);
    float borderThickness = 1.0f;

    bgPanel = new CVSwitchPanel(this, "AppPanel", sf::Color::White, sf::Vector2f(width - 2*borderThickness,
                                   height - borderThickness), false,
                                   sf::Vector2f(borderThickness, 0.0f));

    bgPanel->setCenterOnNew(false);
    bgPanel->setClosablePanels(false);
    bgPanel->disablePanning();
    bgPanel->setTexture("dashboard_bkg");
    bgPanel->setPanRate(40.0);
    bgPanel->setElementPadding(0);

    addPanel(bgPanel, "AppPanel");

    fgPanel = new CVBasicViewPanel(this, "DashboardFg", sf::Color::Transparent, sf::Vector2f(width - 2*borderThickness,
                                   height - borderThickness), false,
                                   sf::Vector2f(borderThickness, 0.0f));
    fgPanel->setOutlineColor(borderColor);
    fgPanel->setOutlineThickness(1.0f);

    CVTitleBar* titleBar = new CVTitleBar(this, ALIGN_LEFT, width, titleBarHeight,
                              textEntry("Keystone ID", "ArialNova", 17*getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color::White),
                              "logo_small", borderColor, borderColor, 0.0f);

    fgPanel->addPanelElement(titleBar, "Title Bar");

    menuPanel = new KYCMenu(this, "Menu Panel",
                              sf::Color(255,255,255), sf::Vector2f(getWidth()*0.25,
                                                                       height*0.84 -
                                                                       titleBar->getSize().y),
                              false, sf::Vector2f(getWidth(),
                                                  titleBar->getSize().y + height*0.12));

    addPanel(menuPanel, "Menu Panel");
    addPanel(fgPanel, "Foreground");

    loginPanel = new KYCLoginPanel(this, "Login", sf::Color::White, sf::Vector2f(width - 2*borderThickness,
                                   height - borderThickness), false,
                                   sf::Vector2f(borderThickness, 0.0f));
    loginPanel->addTriggerTarget(bgPanel, "SwitchTo Dashboard");

    bgPanel->addPanelElement(loginPanel, "Login");

    dashboard = new KYCDashboard(this, "Dashboard", sf::Color(0,0,0,50), sf::Vector2f(width - 2*borderThickness,
                                   height - borderThickness), false,
                                   sf::Vector2f(borderThickness, 0.0f));
    bgPanel->addPanelElement(dashboard, "Dashboard");

    joinPanel = new KYCJoinPanel(this, "Join", sf::Color::White, sf::Vector2f(width - 2*borderThickness,
                                   height - borderThickness), false,
                                   sf::Vector2f(borderThickness, 0.0f));
    bgPanel->addPanelElement(joinPanel, "Join", 0);

    searchPanel = new KYCSearchPanel(this, "Search Panel", sf::Color(0,0,0,50), sf::Vector2f(width - 2*borderThickness,
                                   height - borderThickness), false,
                                   sf::Vector2f(borderThickness, 0.0f));
    bgPanel->addPanelElement(searchPanel, "Search Panel");

    setState(VIEW_STATE_MAIN);

}

}
