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

#include "KYC/interface/join.hpp"
#include "KYC/interface/app.hpp"
#include "KYC/interface/dashboard.hpp"

#include "KYC/network/user.hpp"

#include "cvision/view.hpp"

using namespace cvis;

namespace KYC
{

KYCJoinPanel::KYCJoinPanel(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         form(nullptr),
                         adDisplay(nullptr),
                         backButton(nullptr)
{

    CVTextBox* join_slogan = new CVTextBox(View,
                                         sf::Vector2f(position.x,
                                                      position.y + View->getTitleBarHeight()),
                                        size.x, size.y*0.15,
                                        textEntry("Experience ease, speed, and efficiency in automated due diligence powered by network AI",
                                                  mainApp()->getDefaultFont(),
                                                  25*View->getViewScale(),
                                                  ALIGN_LEFT_MIDLINE,
                                                  sf::Color(230,230,230)),
                                                    sf::Color(35,35,35),
                                                    sf::Color(255,255,255), 0.0f);

    join_slogan->setTextPadding(size.x/20);

    backButton = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.93,
                                          join_slogan->getPosition().y +
                                          join_slogan->getSize().y/2 - size.y/30),
                             size.y/15, size.y/15,
                             textInfo, "rounded_arrow", sf::Color(190,190,190,0),
                             sf::Color(128,128,128,0), 0.0f);
    backButton->setRounding(size.y/15);
    backButton->setFade(255,1);
    backButton->setSpriteColor(sf::Color(153,0,0,0));
    backButton->setSpriteScale(0.65f);

    backButton->addTriggerTarget(getElementById("AppPanel"), "SwitchTo Login");

    adDisplay = new CVSwitchPanel(View, "Ad Panel",
                                  sf::Color::White, sf::Vector2f(size.x, size.y*0.45),
                                  false, sf::Vector2f(position.x,
                                                      join_slogan->getPosition().y +
                                                      join_slogan->getSize().y - 2.0));


    CVTextBox* AD1 = new CVTextBox(View, sf::Vector2f(position.x,
                                              adDisplay->getPosition().y),
                           adDisplay->getSize().x, adDisplay->getSize().y,
                           textEntry("We provide both convenience and power\nin performing extensive background checks.\n"
                                     "You can have the world at your fingertips when\nour technology is on your device\n",
                                                  mainApp()->getDefaultFont(),
                                                  26*View->getViewScale(),
                                                  ALIGN_LEFT_BOTTOM,
                                                  sf::Color(255,255,255)),
                           sf::Color::White, sf::Color::Transparent, 0.0f);
    AD1->setTexture("desk");
    AD1->setTextPadding(size.x/20);

    CVTextBox* AD2 = new CVTextBox(View, sf::Vector2f(position.x,
                                              adDisplay->getPosition().y),
                           adDisplay->getSize().x, adDisplay->getSize().y,
                           textEntry("Our platform provides a data-secure,\nprivate platform for internal workplace use.\n"
                                     "When security and privacy are important to you,\ntrust our program to keep your searches\n"
                                     "anonymous.",
                                                  mainApp()->getDefaultFont(),
                                                  26*View->getViewScale(),
                                                  ALIGN_LEFT_BOTTOM,
                                                  sf::Color(255,255,255)),
                           sf::Color::White, sf::Color::Transparent, 0.0f);
    AD2->setTexture("office_cpu");
    AD2->setTextPadding(size.x/20);

    CVTextBox* AD3 = new CVTextBox(View, sf::Vector2f(position.x,
                                              adDisplay->getPosition().y),
                           adDisplay->getSize().x, adDisplay->getSize().y,
                           textEntry("Communicate and collaborate more effectively\nusing condensed reports provided by our platform.\n"
                                     "Bring the expanse of the web down to a workable\nscale and save valuable time and resources\n",
                                                  mainApp()->getDefaultFont(),
                                                  26*View->getViewScale(),
                                                  ALIGN_LEFT_BOTTOM,
                                                  sf::Color(255,255,255)),
                           sf::Color::White, sf::Color::Transparent, 0.0f);
    AD3->setTexture("training");
    AD3->setTextPadding(size.x/20);

    adDisplay->setCenterOnNew(false);
    adDisplay->disablePanning();
    adDisplay->setDrawClipping(true);
    adDisplay->setAutoSwitchTime(8.0f);
    adDisplay->setClosablePanels(false);
    adDisplay->setElementPadding(0.0f);

    adDisplay->addPanelElement(AD1, "AD1");
    adDisplay->addPanelElement(AD2, "AD2");
    adDisplay->addPanelElement(AD3, "AD3");

    addPanelElement(join_slogan,            "Join Slogan");
    addPanelElement(backButton,             "Back To Login");
    addPanelElement(adDisplay,              "Ad Display");

    form = new KYCJoinForm(View, panelTag + "_form", backgroundColor,
                           sf::Vector2f(size.x, size.y*0.45), false,
                           sf::Vector2f(position.x, size.y*0.65));

    addPanelElement(form, "Join Form");

}

KYCJoinForm::KYCJoinForm(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         titleEntry(nullptr),
                         firstNameEntry(nullptr),
                         lastNameEntry(nullptr),
                         userNameEntry(nullptr),
                         emailEntry(nullptr),
                         passwordEntry(nullptr),
                         confirmPassEntry(nullptr),
                         submitButton(nullptr)
{

    CVTextBox* title = new CVTextBox(View,
                                         position,
                                        size.x, size.y*0.15,
                                        textEntry("Sign up for Keystone ID",
                                                  mainApp()->getDefaultFont(),
                                                  25*View->getViewScale(),
                                                  ALIGN_LEFT_MIDLINE,
                                                  sf::Color(180,180,180,0)),
                                        sf::Color(255,255,255,0),
                                        sf::Color(255,255,255,0), 0.0f);

    title->setFade(255,1);
    addPanelElement(title, "Form title");

    titleEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.17,
                                position.y + size.y*0.2),
                                size.x/25, size.y/12,
                                textEntry("Title", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    titleEntry->setRounding(14.0f);
    titleEntry->setFade(255,1);

    firstNameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.24,
                                position.y + size.y*0.2),
                                size.x/4, size.y/12,
                                textEntry("First name", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    firstNameEntry->setRounding(14.0f);
    firstNameEntry->setFade(255,1);

    lastNameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.52,
                                position.y + size.y*0.2),
                                size.x/4, size.y/12,
                                textEntry("Last name", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    lastNameEntry->setRounding(14.0f);
    lastNameEntry->setFade(255,1);

    userNameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.24,
                                position.y + size.y*0.38),
                                size.x/4, size.y/12,
                                textEntry("Choose an account name", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    userNameEntry->setRounding(14.0f);
    userNameEntry->setFade(255,1);

    emailEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.52,
                                position.y + size.y*0.38),
                                size.x/4, size.y/12,
                                textEntry("Email Address", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    emailEntry->setRounding(14.0f);
    emailEntry->setFade(255,1);

    passwordEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.24,
                                position.y + size.y*0.56),
                                size.x/4, size.y/12,
                                textEntry("Choose a password", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    passwordEntry->setRounding(14.0f);
    passwordEntry->setFade(255,1);
    passwordEntry->setEncode(true);

    confirmPassEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.52,
                                position.y + size.y*0.56),
                                size.x/4, size.y/12,
                                textEntry("Confirm password", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(120,120,120,0)),
                                sf::Color(230,230,230,0), sf::Color(200,200,200,0),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    confirmPassEntry->setRounding(14.0f);
    confirmPassEntry->setFade(255,1);
    confirmPassEntry->setEncode(true);

    addPanelElement(titleEntry,          "User Title");
    addPanelElement(firstNameEntry,      "User First");
    addPanelElement(lastNameEntry,       "User Last");
    addPanelElement(userNameEntry,       "Username");
    addPanelElement(emailEntry,          "Email");
    addPanelElement(passwordEntry,       "Password");
    addPanelElement(confirmPassEntry,    "Confirm password");

    submitButton = new CVButton(View,
                             sf::Vector2f(position.x + size.x*0.9,
                                          position.y + size.y*0.3),
                             size.y/5, size.y/5,
                             textInfo, "rounded_arrow", sf::Color(120,120,120,0),
                             sf::Color(128,128,128,0), 0.0f);
    submitButton->setRounding(size.y/5);
    submitButton->setFade(255,1);
    submitButton->setSpriteColor(sf::Color(153,0,0,0));
    submitButton->setSpriteScale(0.65f);
    submitButton->setActiveState(false);

    addPanelElement(submitButton,        "Signup Submit");

}

bool KYCJoinForm::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    if(!firstNameEntry->getTypeString().empty() &&
       !lastNameEntry->getTypeString().empty() &&
       !userNameEntry->getTypeString().empty() &&
       !emailEntry->getTypeString().empty() &&
       !passwordEntry->getTypeString().empty() &&
       !confirmPassEntry->getTypeString().empty() &&
       checkFields())
    {
        submitButton->setActiveState(true);
    }
    else
    {
        if(firstNameEntry->hasFocus())
        {
            firstNameEntry->setFillColor(230,230,230);
        }
        if(lastNameEntry->hasFocus())
        {
            lastNameEntry->setFillColor(230,230,230);
        }
        if(userNameEntry->hasFocus())
        {
            userNameEntry->setFillColor(230,230,230);
        }
        if(emailEntry->hasFocus())
        {
            emailEntry->setFillColor(230,230,230);
        }
        if(passwordEntry->hasFocus())
        {
            passwordEntry->setFillColor(230,230,230);
        }
        if(confirmPassEntry->hasFocus())
        {
            confirmPassEntry->setFillColor(230,230,230);
        }
        submitButton->setActiveState(false);
    }

    if(submitButton->getTrigger())
    {
        ((KYCApp*)mainApp())->users.emplace_back(firstNameEntry->getTypeString(),
                                                 lastNameEntry->getTypeString(),
                                                 userNameEntry->getTypeString(),
                                                 passwordEntry->getTypeString(),
                                                 titleEntry->getTypeString(),
                                                 emailEntry->getTypeString(),
                                                 Gender::unknown);

        ((KYCApp*)View->mainApp)->setUser(((KYCApp*)mainApp())->users.back());
        ((CVSwitchPanel*)getElementById("AppPanel"))->switch_to("Dashboard");
    }

    return true;
}

bool KYCJoinForm::checkFields()
{
    client newClient(firstNameEntry->getTypeString(),
                     lastNameEntry->getTypeString(),
                     userNameEntry->getTypeString(),
                     passwordEntry->getTypeString(),
                     titleEntry->getTypeString(),
                     emailEntry->getTypeString(),
                     Gender::unknown);

    bool status = true;

    if(newClient.first_name().empty())
    {
        firstNameEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else if(((KYCApp*)mainApp())->users.check_first_name(newClient))
    {
        firstNameEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else
    {
        firstNameEntry->setFillColor(sf::Color(153,255,153));
    }

    if(newClient.last_name().empty())
    {
        lastNameEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else if(((KYCApp*)mainApp())->users.check_last_name(newClient))
    {
        lastNameEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else
    {
        lastNameEntry->setFillColor(sf::Color(153,255,153));
    }

    if(newClient.user_name().empty())
    {
        userNameEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else if(((KYCApp*)mainApp())->users.check_user_name(newClient))
    {
        userNameEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else
    {
        userNameEntry->setFillColor(sf::Color(153,255,153));
    }

    if(newClient.email_address().empty())
    {
        emailEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else if(((KYCApp*)mainApp())->users.check_email(newClient))
    {
        emailEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else
    {
        emailEntry->setFillColor(sf::Color(153,255,153));
    }

    if(newClient.password().empty())
    {
        passwordEntry->setFillColor(sf::Color(255,153,153));
        confirmPassEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else if(passwordEntry->getTypeString() != confirmPassEntry->getTypeString())
    {
        passwordEntry->setFillColor(sf::Color(255,153,153));
        confirmPassEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else if(check_pass_strength(newClient.password()) < 0.5f)
    {
        passwordEntry->setFillColor(sf::Color(255,153,153));
        confirmPassEntry->setFillColor(sf::Color(255,153,153));
        status = false;
    }
    else
    {
        passwordEntry->setFillColor(sf::Color(153,255,153));
        confirmPassEntry->setFillColor(sf::Color(153,255,153));
    }

    return status;

}

}
