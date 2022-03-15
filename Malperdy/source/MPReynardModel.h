//
//  MPReynardModel.h
//  Malperdy
//  
//  This file is based on the CS 4152 PlatformDemo by Walker White, 2021
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Owner:  TBD
//  Contributors: TBD
//  Version: 2/21/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#ifndef __MP_REYNARD_MODEL_H__
#define __MP_REYNARD_MODEL_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPCharacterModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Reynard Model

/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class ReynardModel : public CharacterModel {

protected:
    /** How long until Reynard drops off a wall */
    float _wallSlideDuration;
    /** How long until we can jump again */
    int _jumpCooldown;
    /** How long until we can dash again */
    int  _dashCooldown;

public:

#pragma mark Hidden Constructors
    /**
     * Initializes Reynard at the given position.
     *
     * Reynard is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos       Initial position in world coordinates
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for Reynard's appearance
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Texture> image);

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    virtual void update(float dt) override;

};

#endif //__MP_REYNARD_MODEL_H__