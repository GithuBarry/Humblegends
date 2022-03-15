//
//  MPTrapModel.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#include "MPTrapModel.hpp"

using namespace cugl;
using namespace cugl::physics2;

#pragma mark -
#pragma mark Constructors


/**
 * Initializes a trap with the given characteristics in a given location in Room Space.
 *
 * The geometry corresponding to the room type given by the room ID is
 * taken from the JSON file of rooms.
 *
 * Rooms are automatically initialized to have the bounds given by
 * the default room width/height.
 *
 * @param x         The x-coordinate of the trap in room space
 * @param y         The y-coordinate of the room in room space
 *
 * @return     Returns True if the space is initialized properly.
 */
bool TrapModel::init(float x, float y) {
    
    _boxObstacle.init();
    _boxObstacle.Obstacle::setPosition(x, y);
    _polyNode.init();
    _polyNode.setPosition(x, y);
    createFixtures();
    return true;
}


/**
 * Creates the physics Body(s) for this object, adding them to the world.
 *
 * This method overrides the base method to keep your ship from spinning.
 *
 * @param world Box2D world to store body
 *
 * @return true if object allocation succeeded
 */
void TrapModel::createFixtures() {
    _boxObstacle.createFixtures();
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void TrapModel::releaseFixtures() {
    _boxObstacle.releaseFixtures();
}

