//
//  MPEnemyController.h
//  Malperdy
// 
//  This class handles the behavior of a single enemy. Before any enemies are created to populate the
//  GameScene, the static references to Reynard and the ObstacleWorld must be set; otherwise, the enemies
//  will be inert and not do anything.
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

protected:

    /** Static reference to the obstacle world for raycasting */
    static shared_ptr<physics2::ObstacleWorld> _obstacleWorld;
    /** Static reference to Reynard */
    static shared_ptr<ReynardController> _reynard;

    /** The target that this enemy is currently pursuing, or null if there is none */
    shared_ptr<CharacterModel> _target = nullptr;
    /** The location in physics space that this enemy will be trying to move to, or (-1, -1) if there is no such location */
    Vec2 _targetLoc = Vec2(-1, -1);
    /** Vector cache to temporarily store values in when handling raycasting, because there may be multiple fixtures */
    Vec2 _raycastCache = Vec2(-1, -1);

    // COOLDOWNS
    /** How long Reynard has been in the enemy's detection radius so far */
    float _detectTime = 0.0f;

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
    bool init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Animation> animation);

    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);

    /**
     * Sets the ObstacleWorld that all enemies will use for raycasting. Should be called
     * upon initialization of GameScene.
     *
     * @param world The ObstacleWorld that the enemies should use for raycasting
     */
    static void setObstacleWorld(shared_ptr<physics2::ObstacleWorld> world) {
        _obstacleWorld = world;
    }

    /**
     * Sets a reference to the controller for Reynard, the player character. Should be called
     * after Reynard's controller is first initialized.
     * 
     * @param reynard   The controller for Reynard, the player character
     */
    static void setReynardController(shared_ptr<ReynardController> reynard) {
        _reynard = reynard;
    }

#pragma mark -
#pragma mark Behavior Methods
private:

    /**
     * Helper function that returns the distance squared between this enemy and Reynard
     * in world units.
     * 
     * @return  The squared distance in world units between this enemy and Reynard
     */
    float distSquaredToReynard() {
        return getScenePosition().distanceSquared(_reynard->getScenePosition());
    }

    /**
     * Performs a raycast from this enemy to Reynard. If the raycast succeeds, then the enemy's
     * target location [_targetLoc] will be updated accordingly. The new value of [_targetLoc]
     * will then drive changing between behavior states in the update() method of this class.
     *
     * This method is called and performed at the start of each frame in the update() method
     * above, but the actual raycast is performed asynchronously. Therefore, there may be a
     * slight delay in a raycast succeeding and the enemy's behavior changing, although that's
     * probably fine because it simulates slow brain.
     */
    void reyCast();

};


#endif //__MP_ENEMY_CONTROLLER_H__
