//
//  MPCheckpointKeyCrazy.cpp
//  Rocket Demo (iOS)
//
//  Created by Abu Qader on 5/16/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//
#ifndef MPKeyCrazy_hpp
#define MPKeyCrazy_hpp
#include "MPCheckpointKeyCrazy.hpp"
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include <stdio.h>

#pragma mark -
#pragma mark Initializers

using namespace cugl;


bool CheckpointKeyCrazy::init(const Vec2 pos, const Size size) {
    if (physics2::BoxObstacle::init(pos,size)) {
        // ABU: Below gives physics object weight
        setDensity(10.0f);
        setFriction(0.0f);
        setFixedRotation(true);
        setSensor(false);
        return true;
    };
    return false;
}


/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void CheckpointKeyCrazy::update(float dt) {
    
    BoxObstacle::update(dt);
    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
    }
}

/**
 * Disposes all resources and assets of this Bullet
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a Bullet may not be used until it is initialized again.
 */
void CheckpointKeyCrazy::dispose() {
    _node = nullptr;
}

void CheckpointKeyCrazy::createFixtures() {
    BoxObstacle::createFixtures();
}

void CheckpointKeyCrazy::releaseFixtures() {
    BoxObstacle::releaseFixtures();
}


#endif
