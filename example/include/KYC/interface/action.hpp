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

#pragma once

#ifndef KYC_ACTION
#define KYC_ACTION

#include "cvision/panel.hpp"

namespace KYC
{

class KYCActionPanel : public cvis::CVBasicViewPanel
{
public:

    KYCActionPanel(cvis::CVView* parentView, std::string panelTag = "", sf::Color backgroundColor = sf::Color(255,255,255,0),
                     const sf::Vector2f& size = sf::Vector2f(NAN, NAN), bool bFitToWindow = true,
                     const sf::Vector2f& position = sf::Vector2f(0.0f,0.0f));

    bool update(cvis::CVEvent& event, const sf::Vector2f& mousePos);

    cvis::CVButton*     searchBtn;
    cvis::CVButton*     networkBtn;
    cvis::CVButton*     insightsBtn;
    cvis::CVButton*     shareBtn;
    cvis::CVButton*     chatBtn;

};

}

#endif // KYC_ACTION
