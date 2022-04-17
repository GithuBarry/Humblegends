//
//  MPCheckpoint.cpp
//  Malperdy
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 3/29/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPCheckpoint.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a checkpoint.
 *
 * @return          true if the checkpoint is initialized properly, false otherwise.
 */
bool Checkpoint::init() {
	// Set the checkpoint to be a sensor, so no collisions happen
	this->setSensor(true);
	return true;
}

/**
 * Called when Reynard collides with this checkpoint.
 */
void Checkpoint::activate() {
	CULog("CHECKPOINT HIT");
}