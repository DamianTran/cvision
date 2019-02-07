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

#include "KYC/interface/search.hpp"
#include "KYC/interface/menu.hpp"

#include "cvision/view.hpp"
#include "cvision/app.hpp"

using namespace cvis;

namespace KYC
{

CV_DEFINE_PANEL_CONSTRUCT(KYCPersonSearchForm),
    firstNameEntry(nullptr),
    lastNameEntry(nullptr),
    birthYearEntry(nullptr),
    birthMonthEntry(nullptr),
    birthDayEntry(nullptr),
    countryEntry(nullptr),
    stateEntry(nullptr),
    cityEntry(nullptr),
    occupationEntry(nullptr),
    organizationEntry(nullptr)
{
    CVTextBox* title = new CVTextBox(View,
                                     position,
                                    size.x, size.y*0.15,
                                    textEntry("Perform a search for an individual",
                                              mainApp()->getDefaultFont(),
                                              25*View->getViewScale(),
                                              ALIGN_LEFT_MIDLINE,
                                              sf::Color(60,60,60)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);


    addPanelElement(title, "Person title");

    firstNameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.05,
                                position.y + size.y*0.17),
                                size.x*0.22, size.y/18,
                                textEntry("First name", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    firstNameEntry->setRounding(14.0f * View->getViewScale());

    lastNameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.3,
                                position.y + size.y*0.17),
                                size.x*0.22, size.y/18,
                                textEntry("Last name", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    lastNameEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* nameHeader = new CVTextBox(View,
                                     firstNameEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y*0.17,
                                    textEntry("Name",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    nameHeader->setTextPadding(0);

    birthYearEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.63,
                                position.y + size.y*0.17),
                                size.x/20, size.y/18,
                                textEntry("Year", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_CENTER_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    birthYearEntry->setRounding(14.0f * View->getViewScale());

    birthMonthEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.7,
                                position.y + size.y*0.17),
                                size.x/20, size.y/18,
                                textEntry("Month", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_CENTER_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    birthMonthEntry->setRounding(14.0f * View->getViewScale());

    birthDayEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.77,
                                position.y + size.y*0.17),
                                size.x/20, size.y/18,
                                textEntry("Day", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_CENTER_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    birthDayEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* dateHeader = new CVTextBox(View,
                                     birthYearEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Date of birth",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    dateHeader->setTextPadding(0);

    cityEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.05,
                                position.y + size.y*0.37),
                                size.x*0.22, size.y/18,
                                textEntry("City", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    cityEntry->setRounding(14.0f * View->getViewScale());

    stateEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.3,
                                position.y + size.y*0.37),
                                size.x*0.22, size.y/18,
                                textEntry("State/Province", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    stateEntry->setRounding(14.0f * View->getViewScale());

    countryEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.55,
                                position.y + size.y*0.37),
                                size.x*0.22, size.y/18,
                                textEntry("Country", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    countryEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* locationHeader = new CVTextBox(View,
                                     cityEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Location",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    locationHeader->setTextPadding(0);

    occupationEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.05,
                                position.y + size.y*0.57),
                                size.x*0.22, size.y/18,
                                textEntry("Occupation", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    occupationEntry->setRounding(14.0f * View->getViewScale());

    organizationEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.3,
                                position.y + size.y*0.57),
                                size.x*0.22, size.y/18,
                                textEntry("Organization", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    organizationEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* affiliationHeader = new CVTextBox(View,
                                     occupationEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Affiliations",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    affiliationHeader->setTextPadding(0);

    notesEntry = new CVTypePanel(View,
                                textEntry("Any other information?",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                "Other Notes", sf::Color(230,230,230,200),
                                sf::Vector2f(size.x*0.5, size.y*0.18),
                                std::vector<float>(4, 14.0f*View->getViewScale()),
                                sf::Vector2f(position.x + size.x * 0.05,
                                             position.y + size.y * 0.77));
    notesEntry->setRounding(14.0f * View->getViewScale());
    notesEntry->setDrawClipping(true);
    notesEntry->hide_margins();
    notesEntry->setOutlineThickness(2.0f);
    notesEntry->setOutlineColor(sf::Color(200,200,200,200));

    CVTextBox* notesHeader = new CVTextBox(View,
                                     notesEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Other notes",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    notesHeader->setTextPadding(0);

    uploadBtn = new CVButton(View,
                             sf::Vector2f(notesEntry->getPosition().x + notesEntry->getSize().x + size.x*0.05,
                                          notesEntry->getPosition().y),
                             size.y*0.18, size.y*0.18,
                             textInfo,
                            "upload",
                             sf::Color(230,230,230,220),
                             sf::Color(128,128,128,0), 0.0f);

    uploadBtn->setRounding(size.y/30);
    uploadBtn->setSpriteColor(sf::Color(173,0,0));
    uploadBtn->setSpriteScale(0.625f);
    uploadBtn->setOutlineThickness(2.0f);
    uploadBtn->setOutlineColor(sf::Color(200,200,200,200));

    searchBtn = new CVButton(View,
                             sf::Vector2f(uploadBtn->getPosition().x + uploadBtn->getSize().x + size.x*0.03,
                                          notesEntry->getPosition().y),
                             size.y*0.18, size.y*0.18,
                             textInfo,
                            "search_person",
                             sf::Color(230,230,230,220),
                             sf::Color(128,128,128,0), 0.0f);

    searchBtn->setRounding(size.y/30);
    searchBtn->setSpriteColor(sf::Color(173,0,0));
    searchBtn->setSpriteScale(0.625f);
    searchBtn->setOutlineThickness(2.0f);
    searchBtn->setOutlineColor(sf::Color(200,200,200,200));

    CVTextBox* div1 = new CVTextBox(View,
                                     birthMonthEntry->getPosition() - sf::Vector2f(size.x * 0.012, -size.y*0.005),
                                    size.x, size.y*0.17,
                                    textEntry("/",
                                              mainApp()->getDefaultFont(),
                                              22*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    div1->setTextPadding(0);

    CVTextBox* div2 = new CVTextBox(View,
                                     birthDayEntry->getPosition() - sf::Vector2f(size.x * 0.012, -size.y*0.005),
                                    size.x, size.y*0.17,
                                    textEntry("/",
                                              mainApp()->getDefaultFont(),
                                              22*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    div2->setTextPadding(0);

    addPanelElement(firstNameEntry,     "Person First Name");
    addPanelElement(lastNameEntry,      "Person Last Name");
    addPanelElement(birthYearEntry,     "Person Birth Year");
    addPanelElement(birthMonthEntry,    "Person Birth Month");
    addPanelElement(birthDayEntry,      "Person Birth Day");
    addPanelElement(cityEntry,          "Person City");
    addPanelElement(stateEntry,         "Person State");
    addPanelElement(countryEntry,       "Person Country");
    addPanelElement(occupationEntry,    "Person Occupation");
    addPanelElement(organizationEntry,  "Person Organization");
    addPanelElement(notesEntry,         "Person Notes");

    addPanelElement(uploadBtn,          "Person Upload");
    addPanelElement(searchBtn,          "Person Search");

    addPanelElement(nameHeader,         "Person name");
    addPanelElement(dateHeader,         "Person DOB");
    addPanelElement(locationHeader,     "Person location");
    addPanelElement(affiliationHeader,  "Person affiliation");
    addPanelElement(notesHeader,        "Person notes");
    addPanelElement(div1,               "div1");
    addPanelElement(div2,               "div2");
}

bool KYCPersonSearchForm::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    return true;
}

CV_DEFINE_PANEL_CONSTRUCT(KYCOrganizationSearchForm)
{
    CVTextBox* title = new CVTextBox(View,
                                     position,
                                    size.x, size.y*0.15,
                                    textEntry("Perform a search for an organization",
                                              mainApp()->getDefaultFont(),
                                              25*View->getViewScale(),
                                              ALIGN_LEFT_MIDLINE,
                                              sf::Color(60,60,60)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);


    addPanelElement(title, "Organization title");

    nameEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.05,
                                position.y + size.y*0.17),
                                size.x*0.22, size.y/18,
                                textEntry("Name", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    nameEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* nameHeader = new CVTextBox(View,
                                     nameEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y*0.17,
                                    textEntry("Organization name",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    nameHeader->setTextPadding(0);

    createdYearEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.63,
                                position.y + size.y*0.17),
                                size.x/20, size.y/18,
                                textEntry("Year", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_CENTER_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    createdYearEntry->setRounding(14.0f * View->getViewScale());

    createdMonthEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.7,
                                position.y + size.y*0.17),
                                size.x/20, size.y/18,
                                textEntry("Month", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_CENTER_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    createdMonthEntry->setRounding(14.0f * View->getViewScale());

    createdDayEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.77,
                                position.y + size.y*0.17),
                                size.x/20, size.y/18,
                                textEntry("Day", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_CENTER_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    createdDayEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* dateHeader = new CVTextBox(View,
                                     createdYearEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Year of incorporation/first active",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    dateHeader->setTextPadding(0);

    cityEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.05,
                                position.y + size.y*0.37),
                                size.x*0.22, size.y/18,
                                textEntry("City", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    cityEntry->setRounding(14.0f * View->getViewScale());

    stateEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.3,
                                position.y + size.y*0.37),
                                size.x*0.22, size.y/18,
                                textEntry("State/Province", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    stateEntry->setRounding(14.0f * View->getViewScale());

    countryEntry = new CVTypeBox(View, sf::Vector2f(position.x + size.x*0.55,
                                position.y + size.y*0.37),
                                size.x*0.22, size.y/18,
                                textEntry("Country", mainApp()->getDefaultFont(),
                                          16U*View->getViewScale(), ALIGN_LEFT_MIDLINE, sf::Color(60,60,60)),
                                sf::Color(230,230,230,200), sf::Color(200,200,200,200),
                                2.0f, CV_OBJ_ANIM_SLIDE, CV_TEXT_FIT_LATERAL);
    countryEntry->setRounding(14.0f * View->getViewScale());

    CVTextBox* locationHeader = new CVTextBox(View,
                                     cityEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Location",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    locationHeader->setTextPadding(0);

    notesEntry = new CVTypePanel(View,
                                textEntry("Any other information?",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                "Other Notes", sf::Color(230,230,230,200),
                                sf::Vector2f(size.x*0.5, size.y*0.18),
                                std::vector<float>(4, 14.0f*View->getViewScale()),
                                sf::Vector2f(position.x + size.x * 0.05,
                                             position.y + size.y * 0.57));
    notesEntry->setRounding(14.0f * View->getViewScale());
    notesEntry->setDrawClipping(true);
    notesEntry->hide_margins();
    notesEntry->setOutlineThickness(2.0f);
    notesEntry->setOutlineColor(sf::Color(200,200,200,200));

    CVTextBox* notesHeader = new CVTextBox(View,
                                     notesEntry->getPosition() - sf::Vector2f(0.0f, size.y*0.05),
                                    size.x, size.y * 0.15,
                                    textEntry("Other notes",
                                              mainApp()->getDefaultFont(),
                                              17*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    notesHeader->setTextPadding(0);

    CVTextBox* div1 = new CVTextBox(View,
                                    createdMonthEntry->getPosition() - sf::Vector2f(size.x * 0.012, -size.y*0.005),
                                    size.x, size.y*0.17,
                                    textEntry("/",
                                              mainApp()->getDefaultFont(),
                                              22*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    div1->setTextPadding(0);

    uploadBtn = new CVButton(View,
                             sf::Vector2f(notesEntry->getPosition().x + notesEntry->getSize().x + size.x*0.05,
                                          notesEntry->getPosition().y),
                             size.y*0.18, size.y*0.18,
                             textInfo,
                            "upload",
                             sf::Color(230,230,230,220),
                             sf::Color(128,128,128,0), 0.0f);

    uploadBtn->setRounding(size.y/30);
    uploadBtn->setSpriteColor(sf::Color(173,0,0));
    uploadBtn->setSpriteScale(0.625f);
    uploadBtn->setOutlineThickness(2.0f);
    uploadBtn->setOutlineColor(sf::Color(200,200,200,200));

    searchBtn = new CVButton(View,
                             sf::Vector2f(uploadBtn->getPosition().x + uploadBtn->getSize().x + size.x*0.03,
                                          notesEntry->getPosition().y),
                             size.y*0.18, size.y*0.18,
                             textInfo,
                            "search_org",
                             sf::Color(230,230,230,220),
                             sf::Color(128,128,128,0), 0.0f);

    searchBtn->setRounding(size.y/30);
    searchBtn->setSpriteColor(sf::Color(173,0,0));
    searchBtn->setSpriteScale(0.625f);
    searchBtn->setOutlineThickness(2.0f);
    searchBtn->setOutlineColor(sf::Color(200,200,200,200));

    CVTextBox* div2 = new CVTextBox(View,
                                     createdDayEntry->getPosition() - sf::Vector2f(size.x * 0.012, -size.y*0.005),
                                    size.x, size.y*0.17,
                                    textEntry("/",
                                              mainApp()->getDefaultFont(),
                                              22*View->getViewScale(),
                                              ALIGN_LEFT,
                                              sf::Color(80,80,80)),
                                    sf::Color(255,255,255,0),
                                    sf::Color(255,255,255), 0.0f);
    div2->setTextPadding(0);

    addPanelElement(nameEntry,          "Organization Name");
    addPanelElement(createdYearEntry,   "Organization Creation Year");
    addPanelElement(createdMonthEntry,  "Organization Creation Month");
    addPanelElement(createdDayEntry,    "Organization Creation Day");
    addPanelElement(cityEntry,          "Organization City");
    addPanelElement(stateEntry,         "Organization State");
    addPanelElement(countryEntry,       "Organization Country");
    addPanelElement(notesEntry,         "Organization Notes");

    addPanelElement(uploadBtn,          "Organization Upload");
    addPanelElement(searchBtn,          "Organization Search");

    addPanelElement(nameHeader,         "Organization name");
    addPanelElement(dateHeader,         "Organization creation date");
    addPanelElement(locationHeader,     "Organization location");
    addPanelElement(notesHeader,        "Organization notes");
    addPanelElement(div1,               "div1");
    addPanelElement(div2,               "div2");
}

bool KYCOrganizationSearchForm::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
    }

    return true;
}

CV_DEFINE_PANEL_CONSTRUCT(KYCSearchPanel),
    header(nullptr),
    footer(nullptr),
    time(nullptr),
    menuBtn(nullptr),
    backBtn(nullptr),
    menuPanel((KYCMenu*)getElementById("Menu Panel"))
{

    header = new CVTextBox(View,
                         sf::Vector2f(position.x,
                                      position.y + View->getTitleBarHeight()),
                        size.x, size.y*0.12,
                        textEntry("Web Search",
                                  "ArialNova",
                                  32*View->getViewScale(),
                                  ALIGN_LEFT_MIDLINE,
                                  sf::Color(153,0,0)),
                        sf::Color(255,255,255),
                        sf::Color(255,255,255), 0.0f);

    header->setTextPadding(size.x/12);

    menuBtn = new CVButton(View,
                            sf::Vector2f(position.x + size.x*0.95,
                                          header->getPosition().y + header->getSize().y/2),
                            header->getSize().y*0.5,
                            header->getSize().y*0.5,
                            "menu", 2);

    menuBtn->setSpriteColor(sf::Color(153,0,0));

    backBtn = new CVButton(View,
                             sf::Vector2f(header->getPosition().x +
                                          header->getSize().y/2 - size.y/30,
                                          header->getPosition().y +
                                          header->getSize().y/2 - size.y/30),
                             size.y/15, size.y/15,
                             textInfo, "rounded_arrow", sf::Color(170,170,170),
                             sf::Color(128,128,128,0), 0.0f);
    backBtn->setRounding(size.y/15);
    backBtn->setSpriteColor(sf::Color(153,0,0));
    backBtn->setSpriteScale(0.65f);
    backBtn->getSprite(0).rotate(180);

    backBtn->addTriggerTarget(getElementById("AppPanel"), "SwitchTo Dashboard");

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

    searchFormPanel = new CVSwitchPanel(View, "Search Form Panel",
                                  sf::Color(255,255,255,58), sf::Vector2f(size.x*0.9, size.y*0.68),
                                  false, sf::Vector2f(position.x + size.x * 0.05,
                                                      header->getPosition().y + header->getSize().y + size.y*0.08));
    searchFormPanel->setDrawClipping(true);
    searchFormPanel->disablePanning();
    searchFormPanel->setRounding(18.0*View->getViewScale());
    searchFormPanel->setCenterOnNew(false);
    searchFormPanel->setClosablePanels(false);
    searchFormPanel->setElementPadding(searchFormPanel->getSize().x*0.05);

    personSearchForm = new KYCPersonSearchForm(View, "Person Search Form",
                                  sf::Color(255,255,255,158), sf::Vector2f(size.x*0.88, size.y*0.68 - size.x*0.02),
                                  false, searchFormPanel->getPosition());
    personSearchForm->setRounding(18.0*View->getViewScale());
    personSearchForm->highlight(true);

    organizationSearchForm = new KYCOrganizationSearchForm(View, "Organization Search Form",
                                  sf::Color(255,255,255,158), sf::Vector2f(size.x*0.88, size.y*0.68 - size.x*0.02),
                                  false, searchFormPanel->getPosition());
    organizationSearchForm->setRounding(18.0*View->getViewScale());

    personTab = new CVButton(View,
                             sf::Vector2f(searchFormPanel->getPosition().x,
                                          header->getPosition().y + header->getSize().y + size.y*0.03),
                             size.x*0.125, size.y*0.04,
                             textEntry("Individuals",
                                       mainApp()->getDefaultFont(),
                                       14*View->getViewScale(),
                                       ALIGN_LEFT_MIDLINE,
                                       sf::Color(60,60,60)),
                             "", sf::Color(200,200,200,160),
                             sf::Color(128,128,128,0), 0.0f);
    personTab->setRounding(6.0*View->getViewScale());
    personTab->addTriggerTarget(searchFormPanel, "SwitchTo Person Search Form");
    personTab->setTextPadding(12.0*View->getViewScale());

    organizationTab = new CVButton(View,
                             sf::Vector2f(searchFormPanel->getPosition().x + size.x*0.1325,
                                          header->getPosition().y + header->getSize().y + size.y*0.03),
                             size.x*0.125, size.y*0.04,
                             textEntry("Organizations",
                                       mainApp()->getDefaultFont(),
                                       14*View->getViewScale(),
                                       ALIGN_LEFT_MIDLINE,
                                       sf::Color(60,60,60)),
                             "", sf::Color(200,200,200,160),
                             sf::Color(128,128,128,0), 0.0f);
    organizationTab->setRounding(6.0*View->getViewScale());
    organizationTab->addTriggerTarget(searchFormPanel, "SwitchTo Organization Search Form");
    organizationTab->setTextPadding(12.0*View->getViewScale());

    searchFormPanel->addPanelElement(personSearchForm,          "Person Search Form");
    searchFormPanel->addPanelElement(organizationSearchForm,    "Organization Search Form");

    addPanelElement(searchFormPanel,    "Search Form Panel");

    addPanelElement(header,             "Search Header");
    addPanelElement(footer,             "Search Footer");
    addPanelElement(time,               "Search Time");

    addPanelElement(personTab,          "Person Tab");
    addPanelElement(organizationTab,    "Organization Tab");

    addPanelElement(menuBtn,            "Search Menu Button");
    addPanelElement(backBtn,            "Search Return");
}

bool KYCSearchPanel::update(CVEvent& event, const sf::Vector2f& mousePos)
{
    if(!CVBasicViewPanel::update(event, mousePos))
    {
        return false;
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
        if(searchFormPanel->getActiveElement() == personSearchForm)
        {
            personTab->highlight(true);
            organizationTab->highlight(false);
        }
        else if(searchFormPanel->getActiveElement() == organizationSearchForm)
        {
            personTab->highlight(false);
            organizationTab->highlight(true);
        }
    }

    return true;
}

}
