//
//  MPEnemyController.h
//  Malperdy
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 3/10/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef __MP_ENEMY_CONTROLLER_H__
#define __MP_ENEMY_CONTROLLER_H__

#include <cugl/cugl.h>
#include "MPCharacterController.h"
#include "MPEnemyModel.h"
#include "MPReynardController.h"

/** How many seconds Reynard must be within an enemy's detection radius before the enemy realizes that he is there */
#define DETECTION_TIME 1.0f

class EnemyController : public CharacterController<EnemyModel, EnemyController> {

private:

    /** The target that this enemy is currently pursuing, or null if there is none */
    shared_ptr<CharacterController> _target = nullptr;

    // COOLDOWNS
    /** How long Reynard has been in the enemy's detection radius so far */
    float _detectTime = 0.0f;

public:
    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);

#pragma mark -
#pragma mark Behavior Methods

    /**
     * Called when a target has entered this enemy's detection radius, so in beginContact
     * when the target begins contact with this enemy's detection sensor. Takes in a pointer
     * to the controller for the target.
     * 
     * Because characters other than Reynard can be pursued, this is generalized to just
     * take in a character controller.
     * 
     * TODO: handle issues with multiple possible targets
     * 
     * @param target    Controller for the target, who's just entered the enemy's detection radius
     */
    //void detectTarget(shared_ptr<CharacterController> target);

    /**
     * Called when Reynard has entered this enemy's detection radius, so in beginContact
     * when Reynard begins contact with this enemy's detection sensor. Takes in a pointer
     * to the controller for Reynard.
     *
     * This overloads the general version of this function for Reynard specifically.
     *
     * @param target    Controller for Reynard, who's just entered the enemy's detection radius
     */
    void detectTarget(shared_ptr<CharacterController> target);

    /**
     * Called when a target has left this enemy's detection radius, so in endContact when the
     * target ends contact with this enemy's detection sensor. Takes in a pointer to the
     * controller for the target that was lost.
     *
     * Because characters other than Reynard can be pursued, this is generalized to just
     * take in a character controller.
     * 
     * TODO: handle issues with multiple possible targets
     *
     * @param target    Controller for the target, who's just exited the enemy's detection radius
     */
    //void loseTarget(shared_ptr<CharacterController> target);

};


#endif //__MP_ENEMY_CONTROLLER_H__