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

#include "KYC/interface/dashboard.hpp"
#include "KYC/interface/action.hpp"
#include "KYC/interface/menu.hpp"
#include "KYC/interface/app.hpp"

#include "cvision/view.hpp"

#include <chrono>
#include <iomanip>

using namespace cvis;

namespace KYC
{

KYCDashboard::KYCDashboard(CVView* parentView, std::string panelTag, sf::Color backgroundColor,
                     const sf::Vector2f& size, bool bFitToWindow,
                     const sf::Vector2f& position):
                         CVBasicViewPanel(parentView, panelTag, backgroundColor, size, bFitToWindow, position),
                         header(nullptr),
                         footer(nullptr),
                         time(nullptr),
                         summaryPanel(nullptr),
                         actionPanel(nullptr),
                         lastSearchPanel(nullptr),
                         jobsPanel(nullptr),
                         updatesPanel(nullptr),
                         menuBtn(nullptr),
                         menuPanel(nullptr)
{

    header = new CVTextBox(View,
                         sf::Vector2f(position.x,
                                      position.y + View->getTitleBarHeight()),
                        size.x, size.y*0.12,
                        textEntry("Welcome!",
                                  "ArialNova",
                                  32*View->getViewScale(),
                                  ALIGN_LEFT_MIDLINE,
                                  sf::Color(153,0,0)),
                        sf::Color(255,255,255),
                        sf::Color(255,255,255), 0.0f);

    header->setTextPadding(size.x/12);
    header->addSprite(appTexture("logo_emblem"),
                      sf::Vector2f(header->getSize().y/2,
                                   header->getSize().y/2),
                      sf::Vector2f(header->getSize().y*0.7,
                                   header->getSize().y*0.7));

    menuBtn = new CVButton(View,
                            sf::Vector2f(position.x + size.x*0.95,
                                          header->getPosition().y + header->getSize().y/2),
                            header->getSize().y*0.5,
                            header->getSize().y*0.5,
                            "menu", 2);

    menuBtn->setSpriteColor(sf::Color(153,0,0));
    menuPanel = (KYCMenu*)getElementById("Menu Panel");

    footer = new CVTextBox(View,
                         sf::Vector2f(position.x,
                                      position.y + size.y*0.96),
                        size.x, size.y*0.04,
                        textEntry("Keystone ID",
                                  mainApp()->getDefaultFont(),
                                  16*View->getViewScale(),
                                  ALIGN_LEFT_MIDLINE,
                                  sf::Color(190,190,190)),
                        sf::Color(255,255,255),
                        sf::Color(255,255,255), 0.0f);

    footer->setTextPadding(12.0f*View->getViewScale());


    time = new CVTextBox(View,
                         sf::Vector2f(position.x + size.x*0.5,
                                      position.y + size.y*0.96),
                        size.x * 0.5, size.y*0.04,
                        textEntry("",
                                  mainApp()->getDefaultFont(),
                                  15*View->getViewScale(),
                                  ALIGN_RIGHT_MIDLINE,
                                  sf::Color(140,140,140)),
                        sf::Color(255,255,255,0),
                        sf::Color(255,255,255), 0.0f);

    time->setTextPadding(12.0f*View->getViewScale());

    summaryPanel = new CVSwitchPanel(View, "Summary Panel",
                                  sf::Color(255,255,255,58), sf::Vector2f(size.x*0.3, size.y*0.68),
                                  false, sf::Vector2f(position.x + size.x * 0.03,
                                                      header->getPosition().y + header->getSize().y + size.y*0.08));
    summaryPanel->setDrawClipping(true);
    summaryPanel->disablePanning();
    summaryPanel->setRounding(12.0*View->getViewScale());
    summaryPanel->setCenterOnNew(false);
    summaryPanel->setClosablePanels(false);
    summaryPanel->setPanRate(90.0f);
    summaryPanel->setElementPadding(size.x*0.05);

    lastSearchPanel = new CVListPanel(View, "Last Searches",
                                  sf::Color(255,255,255,78),
                                      sf::Vector2f(summaryPanel->getSize().x * 0.94,
                                                   summaryPanel->getSize().y - summaryPanel->getSize().x*0.06),
                                  false, sf::Vector2f(summaryPanel->getPosition().x + summaryPanel->getSize().x*0.03,
                                                      summaryPanel->getPosition().y + summaryPanel->getSize().x*0.03));
    lastSearchPanel->setRounding(7.0*View->getViewScale());

    jobsPanel = new CVListPanel(View, "Jobs",
                                  sf::Color(255,255,255,78),
                                      sf::Vector2f(summaryPanel->getSize().x * 0.94,
                                                   summaryPanel->getSize().y - summaryPanel->getSize().x*0.06),
                                  false, sf::Vector2f(summaryPanel->getPosition().x + summaryPanel->getSize().x*0.03,
                                                      summaryPanel->getPosition().y + summaryPanel->getSize().x*0.03));
    jobsPanel->setRounding(7.0*View->getViewScale());

    updatesPanel = new CVListPanel(View, "Updates",
                                  sf::Color(255,255,255,78),
                                      sf::Vector2f(summaryPanel->getSize().x * 0.94,
                                                   summaryPanel->getSize().y - summaryPanel->getSize().x*0.06),
                                  false, sf::Vector2f(summaryPanel->getPosition().x + summaryPanel->getSize().x*0.03,
                                                      summaryPanel->getPosition().y + summaryPanel->getSize().x*0.03));
    updatesPanel->setRounding(7.0*View->getViewScale());

    summaryPanel->addPanelElement(lastSearchPanel,    "Last Searches");
    summaryPanel->addPanelElement(jobsPanel,          "Jobs");
    summaryPanel->addPanelElement(updatesPanel,       "Updates");

    lastTab = new CVButton(View,
                             sf::Vector2f(summaryPanel->getPosition().x,
                                          header->getPosition().y + header->getSize().y + size.y*0.03),
                             size.x*0.095, size.y*0.04,
                             textEntry("Last Searches",
                                       mainApp()->getDefaultFont(),
                                       14*View->getViewScale(),
                                       ALIGN_LEFT_MIDLINE,
                                       sf::Color(60,60,60)),
                             "", sf::Color(200,200,200,160),
                             sf::Color(128,128,128,0), 0.0f);
    lastTab->setRounding(6.0*View->getViewScale());
    lastTab->addTriggerTarget(summaryPanel, "SwitchTo Last Searches");
    lastTab->setTextPadding(12.0*View->getViewScale());
    lastTab->highlight(true);

    jobsTab = new CVButton(View,
                             sf::Vector2f(summaryPanel->getPosition().x + size.x*0.1025,
                                          header->getPosition().y + header->getSize().y + size.y*0.03),
                             size.x*0.095, size.y*0.04,
                             textEntry("Running Jobs",
                                       mainApp()->getDefaultFont(),
                                       14*View->getViewScale(),
                                       ALIGN_LEFT_MIDLINE,
                                       sf::Color(60,60,60)),
                             "", sf::Color(200,200,200,160),
                             sf::Color(128,128,128,0), 0.0f);
    jobsTab->setRounding(6.0*View->getViewScale());
    jobsTab->addTriggerTarget(summaryPanel, "SwitchTo Jobs");
    jobsTab->setTextPadding(12.0*View->getViewScale());

    updatesTab = new CVButton(View,
                             sf::Vector2f(summaryPanel->getPosition().x + size.x*0.205,
                                          header->getPosition().y + header->getSize().y + size.y*0.03),
                             size.x*0.095, size.y*0.04,
                             textEntry("Updates",
                                       mainApp()->getDefaultFont(),
                                       14*View->getViewScale(),
                                       ALIGN_LEFT_MIDLINE,
                                       sf::Color(60,60,60)),
                             "", sf::Color(200,200,200,160),
                             sf::Color(128,128,128,0), 0.0f);
    updatesTab->setRounding(6.0*View->getViewScale());
    updatesTab->addTriggerTarget(summaryPanel, "SwitchTo Updates");
    updatesTab->setTextPadding(12.0*View->getViewScale());

    actionPanel = new KYCActionPanel(View, "Action Panel",
                                  sf::Color(255,255,255,158), sf::Vector2f(size.x*0.6, size.y*0.73),
                                  false, sf::Vector2f(summaryPanel->getPosition().x + summaryPanel->getSize().x + size.x * 0.04f,
                                                      header->getPosition().y + header->getSize().y + size.y*0.03));
    actionPanel->setRounding(24.0*View->getViewScale());

    addPanelElement(summaryPanel,       "Summary Panel");
    addPanelElement(actionPanel,        "Action Panel");

    addPanelElement(lastTab,            "Last Tab");
    addPanelElement(jobsTab,            "Jobs Tab");
    addPanelElement(updatesTab,         "Updates Tab");

    addPanelElement(header,             "Header");
    addPanelElement(menuBtn,            "Menu");
    addPanelElement(footer,             "Footer");
    addPanelElement(time,               "Time");

}

bool KYCDashboard::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    const client& user = ((KYCApp*)mainApp())->getLoggedUser();
    if(!user.last_name().empty())
    {

        std::stringstream ss;
        ss << "Welcome, ";
        if(user.user_title().empty())
        {
            if(user.user_gender() == Gender::male)
            {
                ss << "Mr. " << user.last_name();
            }
            else if(user.user_gender() == Gender::female)
            {
                ss << "Ms. " << user.last_name();
            }
            else
            {
                ss << user.first_name();
            }
        }
        else
        {
            ss << user.user_title() << ". " << user.last_name();
        }

        header->setText(0, ss.str());

    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::stringstream tss;
    tss << std::put_time(&tm, "%A, %b %d %Y - %H:%M:%S %p");
    time->setText(0, tss.str());

    if(menuBtn->getTrigger())
    {
        switch(menuBtn->getCurrentState())
        {
        case 1:
            {
                menuPanel->move_to(sf::Vector2f(bounds.left + bounds.width - menuPanel->getSize().x,
                                   menuPanel->getPosition().y), 1220.0f);
                break;
            }
        default:
            {
                menuPanel->move_to(sf::Vector2f(bounds.left + bounds.width,
                                   menuPanel->getPosition().y), 1220.0f);
                break;
            }
        }
    }

    if(event.LMBreleased &&
       (event.LMBreleaseFrames == 1))
    {
        if((menuBtn->getCurrentState() == 1))
        {
            if(!menuBtn->getBounds().contains(event.LMBreleasePosition) &&
               !menuPanel->getBounds().contains(event.LMBreleasePosition))
            {
                menuBtn->setState(0);
                menuPanel->move_to(sf::Vector2f(bounds.left + bounds.width,
                                       menuPanel->getPosition().y), 1220.0f);
            }
        }

        if(summaryPanel->getActiveElement() == lastSearchPanel)
        {
            lastTab->highlight(true);
            jobsTab->highlight(false);
            updatesTab->highlight(false);
        }
        else if(summaryPanel->getActiveElement() == jobsPanel)
        {
            lastTab->highlight(false);
            jobsTab->highlight(true);
            updatesTab->highlight(false);
        }
        else if(summaryPanel->getActiveElement() == updatesPanel)
        {
            lastTab->highlight(false);
            jobsTab->highlight(false);
            updatesTab->highlight(true);
        }
    }

    return true;
}

bool KYCDashboard::draw(sf::RenderTarget* target)
{
    if(!CVBasicViewPanel::draw(target))
    {
        return false;
    }

    return true;
}

}
