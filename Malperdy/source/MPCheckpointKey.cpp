//
//  MPKey.cpp
//  Malperdy
//
//  Created by Abu Qader on 5/13/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//
#ifndef MPKey_hpp
#define MPKey_hpp
#include "MPCheckpointKey.h"
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include <stdio.h>

#pragma mark -
#pragma mark Initializers

using namespace cugl;

// ALWAYS EVEN
int CheckpointKey::_idCounter = 0;

bool CheckpointKey::init(const Vec2 pos, const Size size) {
    if (physics2::BoxObstacle::init(pos,size)) {
        // ABU: Below gives physics object weight
        setDensity(0.0f);
        setFriction(0.0f);
        setFixedRotation(true);
        setSensor(true);

        // Set ID number and increment for next key
        _keyID = _idCounter;
        _idCounter += 2;

        _isPathFinding = true;
        return true;
    };
    return false;
}

void CheckpointKey::setIsPathFinding(bool isPathFinding) {
    _isPathFinding = isPathFinding;
    return;
}

bool CheckpointKey::isPathFinding() {
    return _isPathFinding;
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void CheckpointKey::update(float dt) {
    
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
void CheckpointKey::dispose() {
    _node = nullptr;
}

void CheckpointKey::createFixtures() {
    BoxObstacle::createFixtures();
}

void CheckpointKey::releaseFixtures() {
    BoxObstacle::releaseFixtures();
}


#endif
