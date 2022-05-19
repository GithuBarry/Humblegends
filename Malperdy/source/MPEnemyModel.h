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

/** Name of the detection sensor for an enemy */
#define DETECTION_SENSOR_NAME 10
/** How close to a future move location an enemy has to be to be considered arrived */
#define FUTURE_MOVE_ERROR_SQUARED 0.04f
/** Default max number of hearts an enemy will have */
#define DEFAULT_ENEMY_MAX_HEARTS 3

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
    /** Enum representing the current behavior state that this enemy is in */
    enum class BehaviorState : int {
        PATROLLING, // Enemy has not seen Reynard and is patrolling normally
        REALIZING,  // Enemy has line of sight and is noticing Reynard's presence
        CHASING,    // Enemy is chasing after Reynard
        SEARCHING,  // Enemy has lost Reynard and is looking for him
        RETURNING   // Enemy has given up on Reynard and is going home
    };

    /** Queue of locations that the enemy will try to go to next */
    shared_ptr<deque<Vec2>> _futureMoveLocations = make_shared<deque<Vec2>>();

protected:
    // DETECTION
    /** How close to Reynard the enemy must be to start to notice him */
    float _detectionRadius = 4.0f;
    /** The fixture for the enemy's detection radius */
    b2Fixture *_detectFixture;

    /** How long until the enemy drops off a wall */
    float _wallSlideDuration;
    /** How long until the enemy can jump again */
    int _jumpCooldown;

    /** The enemy's current behavior state */
    BehaviorState _behaveState;
    /** The enemy's detection area, which is the circle within which the enemy can detect Reynard */
    b2CircleShape _detectionArea;

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
    bool init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Animation> animation) ;

#pragma mark -
#pragma mark Attribute Methods

    /**
     * Returns this enemy's current behavior state.
     * 
     * @return  This enemy's current behavior state.
     */
    BehaviorState getBehaveState() {
        return _behaveState;
    }

    /**
     * Sets this enemy's current behavior state to the given state.
     *
     * @param state The behavior state to set this enemy to
     */
    void setBehaveState(BehaviorState state) {
        _behaveState = state;
    }

#pragma mark Update

    /**
     * Updates the enemy's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    virtual void update(float dt) override;

    
    virtual float x_scale() override{
        return 1.5;
    }
    
#pragma mark -
#pragma mark Physics Methods

    /**
     * Create new fixtures for this body, defining the shape
     *
     * This is the primary method to override for custom physics objects
     * 
     * In addition to calling the parent method, enemies also have a
     * fixture to act as their detection radius, which is also added here.
     */
    void createFixtures() ;

    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() ;

};

#endif //__MP_ENEMY_MODEL_H__
