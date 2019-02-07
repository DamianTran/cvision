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
