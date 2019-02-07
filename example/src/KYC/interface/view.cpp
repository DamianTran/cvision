/**================================================================

>	Keystone ID Source Code
>	Copyright (C) 2019 Keystone ID, Incorporated

>	DESCRIPTION:

>	Keystone ID is a web-searching service for enhanced
>	due diligence on individuals with web, political, or
>	government presence.  Keystone ID makes use of several
>	database APIs including Twitter, Facebook, and the
>	News API to help clients verify the credentials of, or
>	discover affiliations to, target individuals.

>	LEGAL:

>	The source code herein is PROPRIETARY and its
>	use, modification, and distribution are to be
>	performed solely with the permission of the
>	owning Company ("Keystone ID, Incorporated").
>	Use, modification, and/or distribution of the
>	enclosed material is ILLEGAL and parties involved
>	in such acts are liable for damages incurred as
>	a result of such acts to the owning Company.

================================================================**/

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
