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
