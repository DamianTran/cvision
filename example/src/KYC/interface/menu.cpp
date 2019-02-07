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
