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
	// Get Reynard's position and add it to his trail
	_trail->push_front(_character->getPosition());
	// Trim trail back down to size if necessary
	if (_trail->size() > TRAIL_LENGTH) _trail->pop_back();

	// Call parent method at the end
	CharacterController::update(delta);
}
