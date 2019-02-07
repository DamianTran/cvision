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

#include "KYC/interface/login.hpp"
#include "KYC/interface/app.hpp"
#include "KYC/interface/dashboard.hpp"

#include "cvision/view.hpp"

using namespace cvis;

namespace KYC
{

KYCLoginPanel::KYCLoginPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         userNameEntry(nullptr),
                         passwordEntry(nullptr),
                         forgotPasswordBtn(nullptr),
                         signUpBtn(nullptr),
                         submitBtn(nullptr),
                         bSubmit(false)
{

    setFade(255,1);

    addSprite(mainApp()->bitmaps.taggedTexture("logo_full"),
                  sf::Vector2f(position.x + size.x/2, position.y + size.y/3),
                  sf::Vector2f(size.y/3, size.y/3),
                  sf::Color(255,255,255,0));

    userNameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.285,
                                position.y + size.y*0.68),
                                size.x/5, size.y/25,
                                textEntry("Username", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    userNameEntry->setRounding(14.0f);
    userNameEntry->setFade(255,1);

    passwordEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.515,
                                position.y + size.y*0.68),
                                size.x/5, size.y/25,
                                textEntry("Password", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    passwordEntry->setRounding(14.0f);
    passwordEntry->setFade(255,1);
    passwordEntry->setEncode(true);

    submitBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.74,
                                          position.y + size.y*0.68),
                             size.x/25, size.y/25,
                             textInfo, "rounded_arrow", sf::Color(230,230,230,0),
                             sf::Color(200,200,200,0), 2.0f);
    submitBtn->setRounding(14.0f);
    submitBtn->setFade(255,1);
    submitBtn->setSpriteColor(sf::Color(153,0,0,0));

    signUpBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.31833,
                                          position.y + size.y*0.755),
                             size.x/6, size.y/20,
                             textEntry("Join Keystone ID",
                                       "ArialNova",
                                       17*View->getViewScale(),
                                       ALIGN_CENTER_MIDLINE,
                                        sf::Color(255,255,255,0)),
                             "", sf::Color(153,0,0,0),
                             sf::Color(200,200,200,0), 2.0f);
    signUpBtn->setRounding(18.0f);
    signUpBtn->setFade(255,1);

    signUpBtn->addTriggerTarget(getElementById("AppPanel"),
                                "SwitchTo Join");

    forgotPasswordBtn = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.515,
                                          position.y + size.y*0.755),
                             size.x/6, size.y/20,
                             textEntry("Forgot password?",
                                       "ArialNova",
                                       17*View->getViewScale(),
                                       ALIGN_CENTER_MIDLINE,
                                        sf::Color(255,255,255,0)),
                             "", sf::Color(153,0,0,0),
                             sf::Color(200,200,200,0), 2.0f);
    forgotPasswordBtn->setRounding(18.0f);
    forgotPasswordBtn->setFade(255,1);

    CVTextBox* cpyNotice = new CVTextBox(View,
                                         sf::Vector2f(position.x + size.x * 0.25,
                                                      position.y + size.y * 0.9),
                                        size.x/2, size.y*0.05,
                                        textEntry("Copyright \xa9 2019 Keystone ID Incorporated",
                                                  mainApp()->getDefaultFont(),
                                                  18*View->getViewScale(),
                                                  ALIGN_CENTER_MIDLINE,
                                                  sf::Color(200,200,200,0)),
                                        sf::Color(255,255,255,0),
                                        sf::Color(255,255,255,0), 0.0f);
    cpyNotice->setFade(255, 1);

    CVTextBox* slogan = new CVTextBox(View,
                                         sf::Vector2f(position.x + size.x * 0.25,
                                                      position.y + size.y * 0.59),
                                        size.x/2, size.y*0.05,
                                        textEntry("An AI web sleuth for times when you need the nose of a bloodhound.",
                                                  mainApp()->getDefaultFont(),
                                                  21*View->getViewScale(),
                                                  ALIGN_CENTER_MIDLINE,
                                                  sf::Color(140,140,140,0)),
                                        sf::Color(255,255,255,0),
                                        sf::Color(255,255,255,0), 0.0f);
    slogan->setFade(255, 1);

    addPanelElement(userNameEntry,          "Username");
    addPanelElement(passwordEntry,          "Password");
    addPanelElement(submitBtn,              "Submit");
    addPanelElement(signUpBtn,              "SignUp");
    addPanelElement(forgotPasswordBtn,      "ForgotPass");
    addPanelElement(cpyNotice,              "Copyright");
    addPanelElement(slogan,                 "Slogan");

}

bool KYCLoginPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    if(submitBtn->getTrigger())
    {
        if(checkSubmit())
        {

            client usrCheck = ((KYCApp*)mainApp())->users.getUser(userNameEntry->getTypeString());

            if(!usrCheck.last_name().empty())
            {

                ((KYCApp*)View->mainApp)->setUser(usrCheck);
                sendTriggers();

            }

        }
        else
        {
            userNameEntry->setFillColor(sf::Color(255,153,153));
            passwordEntry->setFillColor(sf::Color(255,153,153));
        }

        userNameEntry->setTypeString("");
        passwordEntry->setTypeString("");
    }
    else if(userNameEntry->hasFocus() || passwordEntry->hasFocus())
    {
       userNameEntry->setFillColor(sf::Color(230,230,230));
       passwordEntry->setFillColor(sf::Color(230,230,230));
    }

    return true;
}

bool KYCLoginPanel::checkSubmit()
{
    if(((KYCApp*)mainApp())->users.checkLogin(userNameEntry->getTypeString(),
                                        passwordEntry->getTypeString()))
    {
        return true;
    }

    return false;
}

}
