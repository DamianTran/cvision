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

#include "KYC/interface/menu.hpp"
#include "KYC/interface/app.hpp"

#include "cvision/view.hpp"

#include <map>

using namespace cvis;

namespace KYC
{

KYCMenu::KYCMenu(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         signOutBtn(nullptr)
{

    setDrawClipping(true);

    struct menuButtonInfo
    {

        menuButtonInfo(const std::string& newLabel,
                       const std::string& newTexture,
                       const std::string& newTarget,
                       const std::string& newSignal):
                            label(newLabel),
                            texture(newTexture),
                            target(newTarget),
                            signal(newSignal){ }

        std::string label;
        std::string texture;
        std::string target;
        std::string signal;

    };

    std::vector<menuButtonInfo> menuItinerary;
    menuItinerary.emplace_back("Options", "options", "OptionsPanel", "Appear");
    menuItinerary.emplace_back("Profile", "profile", "ProfilePanel", "Appear");

    for(size_t i = 0; i < menuItinerary.size(); ++i)
    {
        CVButton* newBtn = new CVButton(View,
                                         sf::Vector2f(position.x - 1.0f,
                                                      position.y + 1.0f + i*size.y*0.15),
                                         size.x, size.y*0.15,
                                         textEntry(menuItinerary[i].label,
                                                   "ArialNova",
                                                   19*View->getViewScale(),
                                                   ALIGN_LEFT_MIDLINE,
                                                   sf::Color(140,140,140)),
                                         menuItinerary[i].texture, sf::Color(255,255,255),
                                         sf::Color(220,220,220), 1.0f);
        newBtn->getSprite(0).setPosition(newBtn->getPosition().x + newBtn->getSize().y/2,
                                          newBtn->getPosition().y + newBtn->getSize().y/2);
        newBtn->setTextPadding(newBtn->getSize().y);
        newBtn->setSpriteColor(sf::Color(153,0,0));
        newBtn->setSpriteScale(0.50f);

        newBtn->addTriggerTarget(getElementById(menuItinerary[i].target),
                                                menuItinerary[i].signal);
        addPanelElement(newBtn, menuItinerary[i].label + "MenuBtn");

    }

    signOutBtn = new CVButton(View,
                                     sf::Vector2f(position.x - 1.0f,
                                                  position.y + size.y*0.85 - 1.0f),
                                     size.x, size.y*0.15,
                                     textEntry("Log Out",
                                               "ArialNova",
                                               19*View->getViewScale(),
                                               ALIGN_LEFT_MIDLINE,
                                               sf::Color(140,140,140)),
                                     "logout", sf::Color(255,255,255),
                                     sf::Color(220,220,220), 1.0f);
    signOutBtn->getSprite(0).setPosition(signOutBtn->getPosition().x + signOutBtn->getSize().y/2,
                                      signOutBtn->getPosition().y + signOutBtn->getSize().y/2);
    signOutBtn->setTextPadding(signOutBtn->getSize().y);
    signOutBtn->setSpriteColor(sf::Color(153,0,0));
    signOutBtn->setSpriteScale(0.40f);

    addPanelElement(signOutBtn, "LogOutMenuBtn");

}

bool KYCMenu::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    if(signOutBtn->getTrigger())
    {
        move_to(sf::Vector2f(View->getWidth(), getPosition().y), 2220.0f);
        ((KYCApp*)View->mainApp)->setUser(client());
        getElementById("AppPanel")->receive_trigger("SwitchTo Login");
    }

    return true;

}

}
