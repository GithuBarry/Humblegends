//
//  MPReynardModel.cpp
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
#include "MPReynardModel.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#define SIGNUM(x)  ((x > 0) - (x < 0))

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for Dashing */
#define DASH_COOLDOWN   20
/** Amount of time (in seconds) for wall slide duration */
#define WALL_SLIDE_DURATION 1.5f
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.95f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.7f
/** The amount to shrink the sensor fixture (horizontally) relative to the image */
#define DUDE_SSHRINK  0.6f
/** Height of the sensor attached to the player's feet */
#define SENSOR_HEIGHT   0.1f
/** The density of the character */
#define DUDE_DENSITY    1.0f
/** The impulse for the character jump */
#define DUDE_JUMP       500.0f
/** The impulse for the character dash */
#define DUDE_DASH       10.0f
/** Debug color for the sensor */
#define DEBUG_COLOR     Color4::RED


using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
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
bool ReynardModel::init(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> image) {
    // If initialization of parent class failed, return immediately
    if (!(CharacterModel::init(pos, drawScale, image))) return false;

    // Have Reynard be running by default
    _moveState = MovementState::RUNNING;

    return true;
}
