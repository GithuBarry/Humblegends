//
//  MPReynardController.h
//  Malperdy
//
//  Owner: Abu Qader
//  Contributors: Spencer Hurst, Kristina Gu
//  Version: 2/25/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef __MP_REYNARD_CONTROLLER_H__
#define __MP_REYNARD_CONTROLLER_H__

#include <cugl/cugl.h>

#include "MPCharacterController.h"
#include "MPReynardModel.h"

class ReynardController : public CharacterController<ReynardModel, ReynardController> {
    
protected:
    
    /** The length of time in miliseconds that Reynard is invincible for after being damaged  */
    Uint64 _damageBufferLength = 1000;

    Vec2 lastCheckPointPosition = Vec2(-1,-1);
    /** The moment in time that Reynard was last hit */
    Timestamp _lastHit = Timestamp();

public:

    /**
     * Initializes a new controller for the character at the given position.
     *
     * The character is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos       Initial position in world coordinates
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for the character's appearance
     *
     * @return  true if the character is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2& pos, float drawScale, shared_ptr<Animation> animation);
    
    /**  This method checks if the the difference between _lastHit and the current time exceeds
     *      the damageBufferLength. If it does, then the _lastHit is set to be the current time, and the function
     *      returns true. Otherwise, the function returns false.
     */
    bool canBeHit(){
        Timestamp now = Timestamp();
        
        if(now.ellapsedMillis(_lastHit) > _damageBufferLength){
            _lastHit = now;
            return true;
        }
        return false;
    }

    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);

    /**
     * Applies a knockback force to the character in the direction of the
     * given vector.
     *
     * @param dir   Direction to apply knockback force in
     */
    void knockback(b2Vec2 dir);
    
    /**
     Record all the information needed to reset reynard to some checkpoint
     */
    void checkPoint(){
        lastCheckPointPosition = _character->getPosition();
    }
    
    void revert(){
        _character->setHearts(2); //TODO change to better implementation
        _character->setPosition(lastCheckPointPosition);
    }
};


#endif //__MP_REYNARD_CONTROLLER_H__
