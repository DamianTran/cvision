#include "cvision/cvis_anim.hpp"

using namespace AIALG;

namespace cvis{

CVAnim::CVAnim(sf::Transformable* animObject, const std::vector<CVAnimCheckpoint>& checkpoints):
        animObject(animObject),
        checkpointStart(animObject->getPosition()),
        timeLastCheckpointBegin(0.0f),
        moveSpeed(0.0f),
        moveAngle(0.0f),
        moveDist(0.0f),
        passiveAnimRate(0.0f),
        checkpoints(checkpoints),
        currentAnimCheckpoint(0),
        passiveAnimType(CV_OBJ_ANIM_NONE){
            moveAngle = get_angle(animObject->getPosition(), currentCheckpoint().destination);
            if(checkpoints.size() > 0) moveSpeed = checkpoints.front().moveSpeed;
        }

void CVAnim::nextCheckpoint(){
    checkpointStart = currentCheckpoint().destination;
    ++currentAnimCheckpoint;
    animObject->setPosition(checkpointStart);

    if(currentAnimCheckpoint < checkpoints.size()){
        moveAngle = get_angle(checkpointStart, currentCheckpoint().destination);
        moveSpeed = currentCheckpoint().moveSpeed;
    }

    timeLastCheckpointBegin = 0.0f;
    moveDist = 0.0f;
}

void CVAnim::changePath(const std::vector<CVAnimCheckpoint>& newPath){
    currentAnimCheckpoint = 0;
    checkpoints = newPath;

    moveAngle = get_angle(animObject->getPosition(), currentCheckpoint().destination);
    moveDist = 0.0f;
    moveSpeed = currentCheckpoint().moveSpeed;
    checkpointStart = animObject->getPosition();
    timeLastCheckpointBegin = 0.0f;
}

void CVAnim::addCheckpoints(const std::vector<CVAnimCheckpoint>& newCheckpoints, bool cumulative){
    if(!cumulative){
        for(auto& checkpoint : newCheckpoints){
            checkpoints.emplace_back(checkpoint);
        }
    }
    else{
        sf::Vector2f cDist = checkpoints.back().destination - animObject->getPosition();
        for(auto& checkpoint : newCheckpoints){
            checkpoints.emplace_back(checkpoint);
            checkpoints.back().destination += cDist;
        }
    }
}

bool CVAnim::checkPassiveAnim(){
    sf::Shape* obj_coerce = (sf::Shape*)animObject;
    switch(passiveAnimType){
        case CV_OBJ_ANIM_FADE_IN:{
            if(obj_coerce->getFillColor().a == 255_BIT) return false;
            else return true;
        }
        case CV_OBJ_ANIM_FADE_OUT:{
            if(obj_coerce->getFillColor().a == 0_BIT) return false;
            else return true;
        }
        default: return false;
    }
}

void CVAnim::resetPassiveAnim(){
    sf::Shape* obj_coerce = (sf::Shape*)animObject;
    switch(passiveAnimType){
        case CV_OBJ_ANIM_FADE_IN:{
            sf::Color cColor = obj_coerce->getFillColor();
            cColor.a = 0;
            obj_coerce->setFillColor(cColor);
            break;
        }
        case CV_OBJ_ANIM_FADE_OUT:{
            sf::Color cColor = obj_coerce->getFillColor();
            cColor.a = 255;
            obj_coerce->setFillColor(cColor);
            break;
        }
        default: break;
    }
}

bool CVAnim::animFrame(const float& frameTime){

    if(checkpointDistanceValid(frameTime)){ // Move
        switch(currentCheckpoint().animType){
            case CV_OBJ_ANIM_SLIDE:{
                animObject->move(sf::Vector2f(cos(moveAngle),sin(moveAngle))*frameTime*moveSpeed);
                moveDist += frameTime*moveSpeed;
                break;
            }
            case CV_OBJ_ANIM_ACCEL:{
                float accelFactor = abs(1.0f - (getDistance(animObject->getPosition(), checkpointStart) + frameTime*moveSpeed) /
                    getDistance(currentCheckpoint().destination, checkpointStart));
                animObject->move(sf::Vector2f(cos(moveAngle),sin(moveAngle))*frameTime*moveSpeed*accelFactor);
                moveDist += frameTime*moveSpeed*accelFactor;
                break;
            }
            case CV_OBJ_ANIM_DECEL:{
                float accelFactor = abs((getDistance(animObject->getPosition(), checkpointStart) + frameTime*moveSpeed) /
                    getDistance(currentCheckpoint().destination, checkpointStart));
                animObject->move(sf::Vector2f(cos(moveAngle),sin(moveAngle))*frameTime*moveSpeed*accelFactor);
                moveDist += frameTime*moveSpeed*accelFactor;
                break;
            }
            default: animObject->setPosition(currentCheckpoint().destination);
        }
        timeLastCheckpointBegin += frameTime;
    }
    else if(currentAnimCheckpoint < checkpoints.size()){
        nextCheckpoint();
    }

    if(checkPassiveAnim()){ // Apply passive animations
        switch(passiveAnimType){
            case CV_OBJ_ANIM_FADE_IN:{
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
                break;
            }
            case CV_OBJ_ANIM_FADE_OUT:{
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
                break;
            }
            default: break;
        }
    }
}

bool CVAnim::checkpointDistanceValid(const float& frameTime){
    switch(currentCheckpoint().animType){
        case CV_OBJ_ANIM_FADE_IN: return false;
        case CV_OBJ_ANIM_FADE_OUT: return false;
        default: return moveDist + frameTime*moveSpeed < getDistance(checkpointStart, currentCheckpoint().destination);
    }
}

}
