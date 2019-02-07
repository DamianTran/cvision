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

#ifndef KYC_VIEW
#define KYC_VIEW

#include "cvision/view.hpp"
#include "cvision/widgets.hpp"
#include "cvision/panel.hpp"
#include "cvision/panel/toggle.hpp"

namespace KYC
{

class KYCApp;
class KYCLoginPanel;
class KYCDashboard;
class KYCJoinPanel;
class KYCSearchPanel;
class KYCMenu;

class KYCView : public cvis::CVView
{
public:

    cvis::CVTitleBar*               titleBar;

    cvis::CVBasicViewPanel*         fgPanel;

    KYCLoginPanel*                  loginPanel;
    KYCDashboard*                   dashboard;
    KYCJoinPanel*                   joinPanel;
    KYCSearchPanel*                 searchPanel;
    KYCMenu *                       menuPanel;

    cvis::CVSwitchPanel*            bgPanel;

    KYCView(KYCApp* app);

};

}

#endif //KYC_VIEW
