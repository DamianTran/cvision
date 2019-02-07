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

#ifndef KYC_SEARCH_PANEL
#define KYC_SEARCH_PANEL

#include <cvision/panel.hpp>
#include <cvision/word.hpp>

namespace KYC
{

class KYCMenu;

class KYCPersonSearchForm : public cvis::CVBasicViewPanel
{
public:

    CV_PANEL_CONSTRUCTOR(KYCPersonSearchForm);

    cvis::CVTypeBox *   firstNameEntry;
    cvis::CVTypeBox *   lastNameEntry;
    cvis::CVTypeBox *   birthYearEntry;
    cvis::CVTypeBox *   birthMonthEntry;
    cvis::CVTypeBox *   birthDayEntry;
    cvis::CVTypeBox *   countryEntry;
    cvis::CVTypeBox *   stateEntry;
    cvis::CVTypeBox *   cityEntry;
    cvis::CVTypeBox *   occupationEntry;
    cvis::CVTypeBox *   organizationEntry;

    cvis::CVTypePanel * notesEntry;

    cvis::CVButton *    uploadBtn;
    cvis::CVButton *    searchBtn;

    CV_UPDATE_OVERRIDE;

};

class KYCOrganizationSearchForm : public cvis::CVBasicViewPanel
{
public:

    CV_PANEL_CONSTRUCTOR(KYCOrganizationSearchForm);

    cvis::CVTypeBox *   nameEntry;
    cvis::CVTypeBox *   fieldEntry;
    cvis::CVTypeBox *   countryEntry;
    cvis::CVTypeBox *   stateEntry;
    cvis::CVTypeBox *   cityEntry;
    cvis::CVTypeBox *   createdYearEntry;
    cvis::CVTypeBox *   createdMonthEntry;
    cvis::CVTypeBox *   createdDayEntry;

    cvis::CVTypePanel * notesEntry;

    cvis::CVButton *    uploadBtn;
    cvis::CVButton *    searchBtn;

    CV_UPDATE_OVERRIDE;
};

class KYCSearchPanel : public cvis::CVBasicViewPanel
{
public:

    CV_PANEL_CONSTRUCTOR(KYCSearchPanel);

    cvis::CVTextBox *               header,
                    *               footer,
                    *               time;

    cvis::CVButton  *               menuBtn,
                    *               backBtn,
                    *               personTab,
                    *               organizationTab;

    KYCMenu *                       menuPanel;

    cvis::CVSwitchPanel *           searchFormPanel;
    KYCPersonSearchForm *           personSearchForm;
    KYCOrganizationSearchForm *     organizationSearchForm;

    CV_UPDATE_OVERRIDE;

};

}

#endif // KYC_SEARCH_PANEL
