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

#ifndef KYC_APP
#define KYC_APP

#include <string>
#include <vector>

#include "cvision/app.hpp"

#include "KYC/network/user.hpp"

#define IMPORT_SUCCESS          0b11
#define IMPORT_FONT_SUCCESS     0b1
#define IMPORT_IMAGE_SUCCESS    0b10
#define IMPORT_FAIL             0b0

#define EXIT_CODE_SUCCESS       0
#define EXIT_CODE_FAIL          -1
#define EXIT_CODE_TERMINATE     1

namespace KYC
{

class KYCView;
class KYCLoginPanel;
class KYCJoinForm;

class KYCApp : public cvis::CVApp
{
public:

    uint8_t initMainLoop(uint8_t runMode = 0) override;
    uint8_t loadPackages() override;
    bool initUI(unsigned int videoX, unsigned int videoY) override;

    void mainLoop();
    void update();

    void setUser(const client& newUser);
    const client& getLoggedUser() const;

    KYCApp();
    ~KYCApp();

protected:

    bool bTerminate,
        bRunning;

    KYCView* viewPort;

    friend class KYCLoginPanel;
    friend class KYCJoinForm;

    client_base users;
    client logged_user;

};

}

#endif // KYC_APP
