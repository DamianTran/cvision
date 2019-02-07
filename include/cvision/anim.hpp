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

#ifndef CVIS_ANIM
#define CVIS_ANIM

#include "EZC/algorithm.hpp"
#include <SFML/Graphics.hpp>

#include "cvision/algorithm.hpp"

// Animations =====================================

// Generic

#ifndef CVIS_ANIM_MACROS
#define CVIS_ANIM_MACROS

#define CV_OBJ_ANIM_NONE                0
#define CV_OBJ_ANIM_SLIDE               1
#define CV_OBJ_ANIM_ACCEL               2
#define CV_OBJ_ANIM_DECEL               3

#define CV_OBJ_ANIM_FADE_IN             4
#define CV_OBJ_ANIM_FADE_OUT            5
#define CV_OBJ_ANIM_ROTATE              6
#endif

namespace cvis{

struct CVISION_API CVAnimCheckpoint{

    sf::Vector2f destination;
    float velocity;
    uint8_t animType;

    CVAnimCheckpoint(const sf::Vector2f& destination, const float& speed, const uint8_t animType):
        destination(destination),
        velocity(abs(speed)),
        animType(animType){ }
    CVISION_API ~CVAnimCheckpoint() = default;

};

class CVISION_API CVAnim{ // Runtime movement animator
protected:
    sf::Transformable* animObject;
    sf::Vector2f checkpointStart;
    float timeLastCheckpointBegin,
        velocity,
        moveAngle,
        moveDist,
        passiveAnimRate;
    std::vector<CVAnimCheckpoint> checkpoints;
    unsigned int currentAnimCheckpoint;
    uint8_t passiveAnimType;

public:

    inline CVAnimCheckpoint& currentCheckpoint() { return checkpoints[currentAnimCheckpoint]; }
    inline CVAnimCheckpoint& operator[](const unsigned int& index){ return checkpoints[index]; }

    CVISION_API void nextCheckpoint();
    inline void setPassiveAnim(const uint8_t& newPassiveType, const float& rate){
        passiveAnimType = newPassiveType;
        passiveAnimRate = rate;
    }

    inline bool checkpointTimeValid() const{
        float dist;
        if(sin(moveAngle) == 0){
            dist = checkpoints[currentAnimCheckpoint].destination.x - checkpointStart.x;
        }
        else{
            dist = (checkpoints[currentAnimCheckpoint].destination.y - checkpointStart.y)/sin(moveAngle);
        }
        return timeLastCheckpointBegin < dist/velocity;
    }

    CVISION_API bool checkPassiveAnim();
    CVISION_API bool checkpointDistanceValid(const float& frameTime);
    CVISION_API void resetPassiveAnim();

    CVISION_API bool animFrame(const float& frameTime);
    inline bool finished(){ return !checkPassiveAnim() && (currentAnimCheckpoint >= checkpoints.size()); }
    inline const sf::Transformable* object() const{ return animObject; }

    CVISION_API void changePath(const std::vector<CVAnimCheckpoint>& newPath);
    CVISION_API void addCheckpoints(const std::vector<CVAnimCheckpoint>& newCheckpoints, bool cumulative = false);

    CVISION_API CVAnim(sf::Transformable* animObject, const std::vector<CVAnimCheckpoint>& checkpoints);
    CVISION_API ~CVAnim() = default;
};

}

#endif // CVIS_ANIM
