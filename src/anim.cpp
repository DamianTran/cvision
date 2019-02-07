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

#include "cvision/anim.hpp"

using namespace EZC;

namespace cvis
{

CVAnim::CVAnim(sf::Transformable* animObject, const std::vector<CVAnimCheckpoint>& checkpoints):
    animObject(animObject),
    checkpointStart(animObject->getPosition()),
    timeLastCheckpointBegin(0.0f),
    velocity(0.0f),
    moveAngle(0.0f),
    moveDist(0.0f),
    passiveAnimRate(0.0f),
    checkpoints(checkpoints),
    currentAnimCheckpoint(0),
    passiveAnimType(CV_OBJ_ANIM_NONE)
{
    moveAngle = get_angle(animObject->getPosition(), currentCheckpoint().destination);
    if(checkpoints.size() > 0) velocity = checkpoints.front().velocity;
}

void CVAnim::nextCheckpoint()
{
    checkpointStart = currentCheckpoint().destination;
    ++currentAnimCheckpoint;
    animObject->setPosition(checkpointStart);

    if(currentAnimCheckpoint < checkpoints.size())
    {
        moveAngle = get_angle(checkpointStart, currentCheckpoint().destination);
        velocity = currentCheckpoint().velocity;
    }

    timeLastCheckpointBegin = 0.0f;
    moveDist = 0.0f;
}

void CVAnim::changePath(const std::vector<CVAnimCheckpoint>& newPath)
{
    currentAnimCheckpoint = 0;
    checkpoints = newPath;

    moveAngle = get_angle(animObject->getPosition(), currentCheckpoint().destination);
    moveDist = 0.0f;
    velocity = currentCheckpoint().velocity;
    checkpointStart = animObject->getPosition();
    timeLastCheckpointBegin = 0.0f;
}

void CVAnim::addCheckpoints(const std::vector<CVAnimCheckpoint>& newCheckpoints, bool cumulative)
{
    if(!cumulative)
    {
        for(auto& checkpoint : newCheckpoints)
        {
            checkpoints.emplace_back(checkpoint);
        }
    }
    else
    {
        sf::Vector2f cDist = checkpoints.back().destination - animObject->getPosition();
        for(auto& checkpoint : newCheckpoints)
        {
            checkpoints.emplace_back(checkpoint);
            checkpoints.back().destination += cDist;
        }
    }
}

bool CVAnim::checkPassiveAnim()
{
    switch(passiveAnimType)
    {
    case CV_OBJ_ANIM_FADE_IN:
    {
        if(dynamic_cast<sf::Shape*>(animObject))
        {
            sf::Shape* obj_coerce = (sf::Shape*)animObject;
            if(obj_coerce->getFillColor().a == 255_BIT) return false;
            else return true;
        }
        else if(dynamic_cast<sf::Sprite*>(animObject))
        {
            sf::Sprite* obj_coerce = (sf::Sprite*)animObject;
            if(obj_coerce->getColor().a == 255_BIT) return false;
            else return true;
        }
        else if(dynamic_cast<sf::Text*>(animObject))
        {
            sf::Text* obj_coerce = (sf::Text*)animObject;
            if(obj_coerce->getFillColor().a == 255_BIT) return false;
            else return true;
        }
        break;
    }
    case CV_OBJ_ANIM_FADE_OUT:
    {
        if(dynamic_cast<sf::Shape*>(animObject))
        {
            sf::Shape* obj_coerce = (sf::Shape*)animObject;
            if(obj_coerce->getFillColor().a == 0_BIT) return false;
            else return true;
        }
        else if(dynamic_cast<sf::Sprite*>(animObject))
        {
            sf::Sprite* obj_coerce = (sf::Sprite*)animObject;
            if(obj_coerce->getColor().a == 0_BIT) return false;
            else return true;
        }
        else if(dynamic_cast<sf::Text*>(animObject))
        {
            sf::Text* obj_coerce = (sf::Text*)animObject;
            if(obj_coerce->getFillColor().a == 0_BIT) return false;
            else return true;
        }
        break;
    }
    case CV_OBJ_ANIM_ROTATE:  // Rotate until anim stopped
    {
        return true;
    }
    default:
        return false;
    }

    return false;
}

void CVAnim::resetPassiveAnim()
{
    switch(passiveAnimType)
    {
    case CV_OBJ_ANIM_FADE_IN:
    {
        if(dynamic_cast<sf::Shape*>(animObject))
        {
            sf::Shape* obj_coerce = (sf::Shape*)animObject;
            sf::Color cColor = obj_coerce->getFillColor();
            cColor.a = 0;
            obj_coerce->setFillColor(cColor);
        }
        else if(dynamic_cast<sf::Sprite*>(animObject))
        {
            sf::Sprite* obj_coerce = (sf::Sprite*)animObject;
            sf::Color cColor = obj_coerce->getColor();
            cColor.a = 0;
            obj_coerce->setColor(cColor);
        }
        else if(dynamic_cast<sf::Text*>(animObject))
        {
            sf::Text* obj_coerce = (sf::Text*)animObject;
            sf::Color cColor = obj_coerce->getFillColor();
            cColor.a = 0;
            obj_coerce->setFillColor(cColor);
        }
        break;
    }
    case CV_OBJ_ANIM_FADE_OUT:
    {
        if(dynamic_cast<sf::Shape*>(animObject))
        {
            sf::Shape* obj_coerce = (sf::Shape*)animObject;
            sf::Color cColor = obj_coerce->getFillColor();
            cColor.a = 255;
            obj_coerce->setFillColor(cColor);
        }
        else if(dynamic_cast<sf::Sprite*>(animObject))
        {
            sf::Sprite* obj_coerce = (sf::Sprite*)animObject;
            sf::Color cColor = obj_coerce->getColor();
            cColor.a = 255;
            obj_coerce->setColor(cColor);
        }
        else if(dynamic_cast<sf::Text*>(animObject))
        {
            sf::Text* obj_coerce = (sf::Text*)animObject;
            sf::Color cColor = obj_coerce->getFillColor();
            cColor.a = 255;
            obj_coerce->setFillColor(cColor);
        }
        break;
    }
    case CV_OBJ_ANIM_ROTATE:
    {
        animObject->setRotation(0.0f);
        break;

    }
    default:
        break;
    }
}

bool CVAnim::animFrame(const float& frameTime)
{

    if(checkpointDistanceValid(frameTime))  // Move
    {
        switch(currentCheckpoint().animType)
        {
        case CV_OBJ_ANIM_SLIDE:
        {
            animObject->move(sf::Vector2f(cos(moveAngle),sin(moveAngle))*frameTime*velocity);
            moveDist += frameTime*velocity;
            break;
        }
        case CV_OBJ_ANIM_ACCEL:
        {
            float accelFactor = abs(1.0f - (getDistance(animObject->getPosition(), checkpointStart) + frameTime*velocity) /
                                    getDistance(currentCheckpoint().destination, checkpointStart));
            animObject->move(sf::Vector2f(cos(moveAngle),sin(moveAngle))*frameTime*velocity*accelFactor);
            moveDist += frameTime*velocity*accelFactor;
            break;
        }
        case CV_OBJ_ANIM_DECEL:
        {
            float accelFactor = abs((getDistance(animObject->getPosition(), checkpointStart) + frameTime*velocity) /
                                    getDistance(currentCheckpoint().destination, checkpointStart));
            animObject->move(sf::Vector2f(cos(moveAngle),sin(moveAngle))*frameTime*velocity*accelFactor);
            moveDist += frameTime*velocity*accelFactor;
            break;
        }
        default:
            animObject->setPosition(currentCheckpoint().destination);
        }
        timeLastCheckpointBegin += frameTime;
    }
    else if(currentAnimCheckpoint < checkpoints.size())
    {
        nextCheckpoint();
    }

    if(checkPassiveAnim())  // Apply passive animations
    {
        switch(passiveAnimType)
        {
        case CV_OBJ_ANIM_FADE_IN:
        {
            if(dynamic_cast<sf::Shape*>(animObject))
            {
                sf::Shape* obj_coerce = (sf::Shape*)animObject;
                sf::Color cColor = obj_coerce->getFillColor();
                uint8_t colorInc;
                float fInc = passiveAnimRate*frameTime;

                if(uint8_t(fInc) < 1_BIT) colorInc = 1_BIT;
                else if((fInc) > 255.0f) colorInc = 255_BIT;
                else colorInc = uint8_t(fInc);

                if((uint16_t)cColor.a + colorInc >= 255_BIT) cColor.a = 255_BIT;
                else cColor.a += colorInc;
                obj_coerce->setFillColor(cColor);
            }
            else if(dynamic_cast<sf::Sprite*>(animObject))
            {
                sf::Sprite* obj_coerce = (sf::Sprite*)animObject;
                sf::Color cColor = obj_coerce->getColor();
                uint8_t colorInc;
                float fInc = passiveAnimRate*frameTime;

                if(uint8_t(fInc) < 1_BIT) colorInc = 1_BIT;
                else if((fInc) > 255.0f) colorInc = 255_BIT;
                else colorInc = uint8_t(fInc);

                if((uint16_t)cColor.a + colorInc >= 255_BIT) cColor.a = 255_BIT;
                else cColor.a += colorInc;
                obj_coerce->setColor(cColor);
            }
            else if(dynamic_cast<sf::Text*>(animObject))
            {
                sf::Text* obj_coerce = (sf::Text*)animObject;
                sf::Color cColor = obj_coerce->getFillColor();
                uint8_t colorInc;
                float fInc = passiveAnimRate*frameTime;

                if(uint8_t(fInc) < 1_BIT) colorInc = 1_BIT;
                else if((fInc) > 255.0f) colorInc = 255_BIT;
                else colorInc = uint8_t(fInc);

                if((uint16_t)cColor.a + colorInc >= 255_BIT) cColor.a = 255_BIT;
                else cColor.a += colorInc;
                obj_coerce->setFillColor(cColor);
            }
            break;
        }
        case CV_OBJ_ANIM_FADE_OUT:
        {
            if(dynamic_cast<sf::Shape*>(animObject))
            {
                sf::Shape* obj_coerce = (sf::Shape*)animObject;
                sf::Color cColor = obj_coerce->getFillColor();
                uint8_t colorInc;
                float fInc = passiveAnimRate*frameTime;

                if(uint8_t(fInc) < 1_BIT) colorInc = 1_BIT;
                else if((fInc) > 255.0f) colorInc = 255_BIT;
                else colorInc = uint8_t(fInc);

                if(cColor.a <= colorInc) cColor.a = 0_BIT;
                else cColor.a -= colorInc;
                obj_coerce->setFillColor(cColor);
            }
            else if(dynamic_cast<sf::Sprite*>(animObject))
            {
                sf::Sprite* obj_coerce = (sf::Sprite*)animObject;
                sf::Color cColor = obj_coerce->getColor();
                uint8_t colorInc;
                float fInc = passiveAnimRate*frameTime;

                if(uint8_t(fInc) < 1_BIT) colorInc = 1_BIT;
                else if((fInc) > 255.0f) colorInc = 255_BIT;
                else colorInc = uint8_t(fInc);

                if(cColor.a <= colorInc) cColor.a = 0_BIT;
                else cColor.a -= colorInc;
                obj_coerce->setColor(cColor);
            }
            else if(dynamic_cast<sf::Text*>(animObject))
            {
                sf::Text* obj_coerce = (sf::Text*)animObject;
                sf::Color cColor = obj_coerce->getFillColor();
                uint8_t colorInc;
                float fInc = passiveAnimRate*frameTime;

                if(uint8_t(fInc) < 1_BIT) colorInc = 1_BIT;
                else if((fInc) > 255.0f) colorInc = 255_BIT;
                else colorInc = uint8_t(fInc);

                if(cColor.a <= colorInc) cColor.a = 0_BIT;
                else cColor.a -= colorInc;
                obj_coerce->setFillColor(cColor);
            }
            break;
        }
        case CV_OBJ_ANIM_ROTATE:
        {
            animObject->setRotation(animObject->getRotation() + passiveAnimRate);
            break;
        }
        default:
            break;
        }
    }

    return true;
}

bool CVAnim::checkpointDistanceValid(const float& frameTime)
{
    switch(currentCheckpoint().animType)
    {
    case CV_OBJ_ANIM_FADE_IN:
        return false;
    case CV_OBJ_ANIM_FADE_OUT:
        return false;
    default:
        return moveDist + frameTime*velocity < getDistance(checkpointStart, currentCheckpoint().destination);
    }
}

}
