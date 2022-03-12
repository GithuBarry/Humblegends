//
//  MPEnemyModel.h
//  Malperdy
//  
//  This file is based on the CS 4152 PlatformDemo by Walker White, 2021
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
// 
//  Placeholder rabbit image from: https://www.clipartbay.com/cliparts/orange-rabbit-clipart-glhoc79.png
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 3/10/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#ifndef __MP_ENEMY_MODEL_H__
#define __MP_ENEMY_MODEL_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPCharacterModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Enemy Model

/**
* Enemy avatar for the platform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class EnemyModel : public CharacterModel {

public:
    /** Enum representing the current AI state that this enemy is in */
    enum class AIState : int {
        PATROLLING, // Enemy has not seen Reynard and is patrolling normally
        REALIZING,  // Enemy has line of sight and is noticing Reynard's presence
        CHASING,    // Enemy is chasing after Reynard
        SEARCHING,  // Enemy has lost Reynard and is looking for him
        RETURNING   // Enemy has given up on Reynard and is going home
    };

protected:
    // CONSTANTS
    /** How far the /

    /** How long until the enemy drops off a wall */
    float _wallSlideDuration;
    /** How long until the enemy can jump again */
    int _jumpCooldown;

public:

#pragma mark Hidden Constructors
    /**
     * Initializes the enemy at the given position.
     *
     * The enemy is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos       Initial position in world coordinates
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for the enemy's appearance
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Texture> image);

};

#endif //__MP_ENEMY_MODEL_H__