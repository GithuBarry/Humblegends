//
//  MPReynardController.cpp
//  Malperdy
//
//  Owner: Abu Qader
//  Contributors: Spencer Hurst, Kristina Gu
//  Version: 2/25/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#define VELOCITY    .5f
#define DUDE_JUMP   0.55f

#include "MPReynardController.h"

/**
 * This method handles anything about the character that needs to change over time.
 *
 * @param delta The amount of time that has passed since the last frame
 */
void ReynardController::update(float delta) {
	// Call parent method at the end
	CharacterController::update(delta);
}

/**
 * Applies a knockback force to the character in the direction of the
 * given vector.
 *
 * @param dir   Direction to apply knockback force in
 */
void ReynardController::knockback(b2Vec2 dir) {
    // Call parent method
    CharacterController::knockback(dir);

    // Turn Reynard in the direction of the knockback force
    if ((dir.x < 0 && _character->isFacingRight()) ||
        (dir.x > 0 && !_character->isFacingRight())) turn();
}