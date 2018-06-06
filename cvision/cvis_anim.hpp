#pragma once

#ifndef CVIS_ANIM
#define CVIS_ANIM

#include "algorithm.hpp"
#include <SFML/Graphics.hpp>

#include "cvision/cvis_algorithm.hpp"

// Animations =====================================

// Generic

#define CV_OBJ_ANIM_NONE                0_BIT
#define CV_OBJ_ANIM_SLIDE               1_BIT
#define CV_OBJ_ANIM_ACCEL               2_BIT
#define CV_OBJ_ANIM_DECEL               3_BIT

#define CV_OBJ_ANIM_FADE_IN             4_BIT
#define CV_OBJ_ANIM_FADE_OUT            5_BIT

namespace cvis{

struct CVAnimCheckpoint{

    sf::Vector2f destination;
    float moveSpeed;
    uint8_t animType;

    CVAnimCheckpoint(const sf::Vector2f& destination, const float& speed, const uint8_t animType):
        destination(destination),
        moveSpeed(abs(speed)),
        animType(animType){ }
    ~CVAnimCheckpoint() = default;

};

class CVAnim{ // Runtime movement animator
protected:
    sf::Transformable* animObject;
    sf::Vector2f checkpointStart;
    float timeLastCheckpointBegin,
        moveSpeed,
        moveAngle,
        moveDist,
        passiveAnimRate;
    std::vector<CVAnimCheckpoint> checkpoints;
    unsigned int currentAnimCheckpoint;
    uint8_t passiveAnimType;

public:

    inline CVAnimCheckpoint& currentCheckpoint() { return checkpoints[currentAnimCheckpoint]; }
    inline CVAnimCheckpoint& operator[](const unsigned int& index){ return checkpoints[index]; }

    void nextCheckpoint();
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
        return timeLastCheckpointBegin < dist/moveSpeed;
    }

    bool checkPassiveAnim();
    bool checkpointDistanceValid(const float& frameTime);
    void resetPassiveAnim();

    bool animFrame(const float& frameTime);
    inline bool finished(){ return !checkPassiveAnim() && (currentAnimCheckpoint >= checkpoints.size()); }
    inline const sf::Transformable* object() const{ return animObject; }

    void changePath(const std::vector<CVAnimCheckpoint>& newPath);
    void addCheckpoints(const std::vector<CVAnimCheckpoint>& newCheckpoints, bool cumulative = false);

    CVAnim(sf::Transformable* animObject, const std::vector<CVAnimCheckpoint>& checkpoints);
    ~CVAnim() = default;
};

}

#endif // CVIS_ANIM
