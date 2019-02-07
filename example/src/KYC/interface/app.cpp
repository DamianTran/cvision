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

#include "KYC/interface/app.hpp"
#include "KYC/interface/view.hpp"

#include "binINC/font_manifest.hpp"
#include "binINC/img_manifest.hpp"

#include "cvision/time.hpp"

#include "EZC/toolkit/filesystem.hpp"

#include <boost/filesystem.hpp>

using namespace EZC;
using namespace KYC_FONT;

namespace KYC
{

namespace time = std::chrono;

uint8_t KYCApp::initMainLoop(uint8_t runMode)
{

    return CV_INIT_SUCCESS;

}

typedef std::vector<boost::filesystem::path> PathList;

uint8_t KYCApp::loadPackages()
{

    bool imgLoad = false, txtLoad = false;

    for(const auto& ID : IMG_IDS)  // Load from binaries
    {
        if(bitmaps.addTexture(IMG_ID_REG.at(ID), IMG_SIZE_REG.at(ID), ID)) imgLoad = true;
    }

    for(const auto& ID : FONT_IDS)
    {
        if(fonts.addFont(FONT_ID_REG.at(ID), FONT_SIZE_REG.at(ID), ID)) txtLoad = true;
    }

    PathList files;
    getFilesInDir(files, "media", "");

    if(!access("../media/SYS/bin_manifest.txt", F_OK))
    {
        for(size_t i = 0; i < files.size();)  // Filter system binaries during development
        {
            if(cmpStringToList(files[i].string(), {"/SYS/", "\\SYS\\" }, CMP_STR_SIZE_INSENSITIVE))
            {
                files.erase(files.begin() + i);
            }
            else ++i;
        }
    }

    for(auto& path : files)  // Add in user fonts and images
    {
        if(cmpStringToList(path.extension().string(), { ".ttf" }))
        {
            fonts.addFont(path.string(), path.stem().string());
        }
        else if(cmpStringToList(path.extension().string(), { ".png", ".jpg", ".jpeg", ".bmp" }))
        {
            bitmaps.addTexture(path.string(), path.stem().string());
        }
    }

    if(txtLoad & imgLoad) return IMPORT_SUCCESS;
    else if(txtLoad) return IMPORT_FONT_SUCCESS;
    else if(imgLoad) return IMPORT_IMAGE_SUCCESS;
    else return IMPORT_FAIL;
}

bool KYCApp::initUI(unsigned int videoX, unsigned int videoY)
{
    return true;
}

KYCApp::KYCApp():
    CVApp(60,0.25,0.25,"ArialNova_Light"),
    bTerminate(false),
    bRunning(false),
    viewPort(nullptr)
{

    std::cout << "Loading user information...\n";

    users.load("../data/users.bin");

    std::cout << "Loading packages...\n";

    loadPackages();

    std::cout << "Initializing app...\n";

    switch(initMainLoop())
    {

    case CV_INIT_FAIL:
        {
            std::cout << "KYC App init failed... closing\n";
            break;
        }
    default:
        {
            std::cout << "KYC App initialization successful\n";
            break;
        }

    }

    viewPort = new KYCView(this);
    addView(viewPort, "Dashboard");

    while(!viewPort->viewPort->isOpen())
    {
        std::this_thread::sleep_for(std::chrono::duration<float>(1.0f/12));
    }

    mainLoop();

    boost::filesystem::create_directories("../data/");
    users.save("../data/users.bin");

}

KYCApp::~KYCApp()
{

    bRunning = false;

    closeAll();

}

void KYCApp::mainLoop()
{
    auto t0 = TIME_NOW;
    float lastFrameTime;

    bRunning = true;

    while(!bTerminate && bRunning)
    {

        update();

        if(!viewPort->viewPort->isOpen())
        {
            break;
        }

        lastFrameTime = time::duration<float>(TIME_NOW - t0).count();
        t0 = TIME_NOW;

        if(lastFrameTime > frameTime)
        {
            std::this_thread::sleep_for(time::duration<float>(lastFrameTime - frameTime));
        }

    }

    std::cout << "App sequence ended...\n";
}

void KYCApp::update()
{

}

void KYCApp::setUser(const client& newUser)
{
    logged_user = newUser;
}

const client& KYCApp::getLoggedUser() const
{
    return logged_user;
}

}
