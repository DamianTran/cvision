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
// warranties.  By using CVision, you ackowledge and agree
// to this disclaimer.  Use of CVision in Users's programs
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

#ifndef CVIS_TRIGGER
#define CVIS_TRIGGER

#include <vector>
#include <string>
#include <thread>

#include "cvision/lib.hpp"

#include <SFML/Graphics.hpp>

// Trigger ID toolkit ============================

// Flag IDs

#define CV_TRIGGER_FLAG_MISC            255_BIT
#define CV_TRIGGER_FLAG_NONE            0_BIT
#define CV_TRIGGER_FLAG_TYPE            1_BIT
#define CV_TRIGGER_FLAG_DIRECTION       2_BIT

// Direction flags

#define CV_TRIGGER_DIRECTION_NONE       0b0
#define CV_TRIGGER_DIRECTION_LEFT       0b1
#define CV_TRIGGER_DIRECTION_RIGHT      0b10
#define CV_TRIGGER_DIRECTION_UP         0b100
#define CV_TRIGGER_DIRECTION_DOWN       0b1000

// Type flags

#define CV_TRIGGER_FAIL                 0_BIT
#define CV_TRIGGER_CLICK                1_BIT
#define CV_TRIGGER_HOVER                2_BIT
#define CV_TRIGGER_HOLD                 3_BIT
#define CV_TRIGGER_BUTTON               4_BIT
#define CV_TRIGGER_TIME                 5_BIT
#define CV_TRIGGER_HOLD_RELEASE         6_BIT
#define CV_TRIGGER_MISC                 255_BIT

// Zone flags

#define CV_TRIGGER_ZONE_NONE            0_BIT
#define CV_TRIGGER_ZONE_CENTER          1_BIT
#define CV_TRIGGER_ZONE_BODY            2_BIT
#define CV_TRIGGER_ZONE_BORDER          3_BIT
#define CV_TRIGGER_ZONE_OUTSIDE         4_BIT

namespace cvis
{

class CVElement;

class CVISION_API CVTrigger  // Base common trigger format - instantiate at trigger point and pass reference along
{
protected:
    std::string infoArray,      // Bytes of flag values
        flags,       // Flag codes
        tag;         // A tag associated with the trigger

public:
    inline void addFlag(const char& newFlag, const char& newValue = 0b0)
    {
        infoArray.push_back(newValue);
        flags.push_back(newFlag);
    }
    inline void removeFlag(const char& flag)
    {
        for(size_t i = 0; i < flags.size(); ++i)
        {
            if(flags[i] == flag)
            {
                flags.erase(flags.begin() + i);
                infoArray.erase(infoArray.begin() + i);
                return;
            }
        }
    }
    inline void changeFlagValue(const char& flag, const char& newValue)
    {
        for(size_t i = 0; i < flags.size(); ++i)
        {
            if(flags[i] == flag) infoArray[i] = newValue;
        }
    }
    inline void setFlagValue(const unsigned int& flagIndex, const char& newValue)
    {
        infoArray[flagIndex] = newValue;
    }
    inline bool getFlagValue(const char& flag, char& output) const
    {
        for(size_t i = 0; i < flags.size(); ++i)
        {
            if(flags[i] == flag)
            {
                output = infoArray[i];
                return true;
            }
        }
        return false;
    }
    inline std::string getTag() const
    {
        return tag;
    }
    inline void setTag(const std::string& newTag)
    {
        tag = newTag;
    }

    CVTrigger(const std::string& flags, const std::string& infoArray, const std::string& tag = std::string()):
        infoArray(infoArray),
        flags(flags),
        tag(tag) { }
    ~CVTrigger() = default;

};

struct CVISION_API CVTriggerTarget
{

    CVTriggerTarget():
        target(nullptr),
        state(UINT_MAX) { }
    CVTriggerTarget(CVElement* newTarget,
                    const std::string& newSignal = "",
                    const unsigned int& state = UINT_MAX):
        target(newTarget),
        signal(newSignal),
        state(state) { }

    CVElement* target;

    std::string signal;         // Information sent to the target

    unsigned int state;

};

}

#endif // CVIS_TRIGGER
