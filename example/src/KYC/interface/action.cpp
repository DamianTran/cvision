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

#include "KYC/interface/action.hpp"

#include "cvision/view.hpp"

using namespace cvis;

namespace KYC
{

KYCActionPanel::KYCActionPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         searchBtn(nullptr),
                         networkBtn(nullptr),
                         insightsBtn(nullptr),
                         shareBtn(nullptr),
                         chatBtn(nullptr)
{

    searchBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.025,
                                          position.y + size.y*0.025),
                             size.x*0.46, size.y*0.46,
                             textEntry("Start a search",
                                       "ArialNova",
                                       20*View->getViewScale(),
                                       ALIGN_CENTER_BOTTOM,
                                       sf::Color(140,140,140,0)),
                             "search", sf::Color(255,255,255,0),
                             sf::Color(128,128,128,0), 0.0f);
    searchBtn->setRounding(size.y/20);
    searchBtn->setFade(220,2);
    searchBtn->setHighlightColor(sf::Color(255,255,255));
    searchBtn->setSpriteColor(sf::Color(193,0,0,0));
    searchBtn->setSpriteScale(0.70f);
    searchBtn->setTextPadding(size.y/25);

    searchBtn->addTriggerTarget(getElementById("AppPanel"), "SwitchTo Search Panel");

    networkBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.515,
                                          position.y + size.y*0.025),
                             size.x*0.46, size.y*0.46,
                             textEntry("Network analysis",
                                       "ArialNova",
                                       20*View->getViewScale(),
                                        ALIGN_CENTER_BOTTOM,
                                       sf::Color(140,140,140,0)),
                            "network",
                             sf::Color(255,255,255,0),
                             sf::Color(128,128,128,0), 0.0f);

    networkBtn->setRounding(size.y/25);
    networkBtn->setFade(220,2);
    networkBtn->setHighlightColor(sf::Color(255,255,255));
    networkBtn->setSpriteColor(sf::Color(193,0,0,0));
    networkBtn->setSpriteScale(0.625f);
    networkBtn->setTextPadding(size.y/25);

    insightsBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.025,
                                          position.y + size.y*0.515),
                             size.x*0.3, size.y*0.46,
                             textEntry("Insights",
                                       "ArialNova",
                                       20*View->getViewScale(),
                                        ALIGN_CENTER_BOTTOM,
                                       sf::Color(140,140,140,0)),
                            "insights",
                             sf::Color(255,255,255,0),
                             sf::Color(128,128,128,0), 0.0f);

    insightsBtn->setRounding(size.y/25);
    insightsBtn->setFade(220,2);
    insightsBtn->setHighlightColor(sf::Color(255,255,255));
    insightsBtn->setSpriteColor(sf::Color(193,0,0,0));
    insightsBtn->setSpriteScale(0.625f);
    insightsBtn->setTextPadding(size.y/25);

    shareBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.35,
                                          position.y + size.y*0.515),
                             size.x*0.3, size.y*0.46,
                             textEntry("Share",
                                       "ArialNova",
                                       20*View->getViewScale(),
                                        ALIGN_CENTER_BOTTOM,
                                       sf::Color(140,140,140,0)),
                            "share",
                             sf::Color(255,255,255,0),
                             sf::Color(128,128,128,0), 0.0f);

    shareBtn->setRounding(size.y/25);
    shareBtn->setFade(220,2);
    shareBtn->setHighlightColor(sf::Color(255,255,255));
    shareBtn->setSpriteColor(sf::Color(193,0,0,0));
    shareBtn->setSpriteScale(0.625f);
    shareBtn->setTextPadding(size.y/25);

    chatBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.675,
                                          position.y + size.y*0.515),
                             size.x*0.3, size.y*0.46,
                             textEntry("Support",
                                       "ArialNova",
                                       20*View->getViewScale(),
                                        ALIGN_CENTER_BOTTOM,
                                       sf::Color(140,140,140,0)),
                            "support",
                             sf::Color(255,255,255,0),
                             sf::Color(128,128,128,0), 0.0f);

    chatBtn->setRounding(size.y/25);
    chatBtn->setFade(220,2);
    chatBtn->setHighlightColor(sf::Color(255,255,255));
    chatBtn->setSpriteColor(sf::Color(193,0,0,0));
    chatBtn->setSpriteScale(0.625f);
    chatBtn->setTextPadding(size.y/25);

    addPanelElement(searchBtn,      "Search");
    addPanelElement(networkBtn,     "Network");
    addPanelElement(insightsBtn,    "Insights");
    addPanelElement(shareBtn,       "Share");
    addPanelElement(chatBtn,        "Support");

}

bool KYCActionPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    return true;
}

}
