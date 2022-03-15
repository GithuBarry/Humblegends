//
//  MPEnemyController.cpp
//  Malperdy
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 2/25/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#define VELOCITY    10
#define DUDE_JUMP   5.5f

#include "MPEnemyController.h"

/**
 * This method handles anything about the character that needs to change over time.
 *
 * @param delta The amount of time that has passed since the last frame
 */
void EnemyController::update(float delta) {
	// Handle what the enemy does depending on their current behavior state
	switch (_character->getBehaveState()) {
	case (EnemyModel::BehaviorState::PATROLLING):
		break;
	case (EnemyModel::BehaviorState::REALIZING):
		// If enough time has passed that enemy realizes Reynard's there
		if (_detectTime > DETECTION_TIME) {
			// Do a little jump
			jump();
			// Reset detection time
			_detectTime = 0;
			// Start chasing
			_character->setBehaveState(EnemyModel::BehaviorState::CHASING);
		}
		// If not, tick detection time
		else {
			_detectTime += delta;
		}
		break;
	case (EnemyModel::BehaviorState::CHASING):
		// If Reynard is within attack range, attack
		// Otherwise, raycast to the point on Reynard's trail closest to Reynard that the enemy can see
		// If no such point, move enemy to Searching state
		// Otherwise, set enemy to be moving in Reynard's direction
		break;
	case (EnemyModel::BehaviorState::SEARCHING):
		break;
	case (EnemyModel::BehaviorState::RETURNING):
		break;
	}

	// Call parent method at the end
	CharacterController::update(delta);
}

#pragma mark -
#pragma mark Behavior Methods

/**
 * Called when a target has entered this enemy's detection radius, so in beginContact
 * when the target begins contact with this enemy's detection sensor. Takes in a pointer
 * to the controller for the target.
 *
 * Because characters other than Reynard can be pursued, this is generalized to just
 * take in a character controller.
 *
 * @param target    Controller for the target, who's just entered the enemy's detection radius
 */
//void EnemyController::detectTarget(shared_ptr<CharacterController> target) {
//	_target = target;
//	// Start realizing
//	_character->setBehaveState(EnemyModel::BehaviorState::REALIZING);
//}

/**
 * Called when Reynard has entered this enemy's detection radius, so in beginContact
 * when Reynard begins contact with this enemy's detection sensor. Takes in a pointer
 * to the controller for Reynard.
 *
 * This overloads the general version of this function for Reynard specifically.
 *
 * @param target    Controller for Reynard, who's just entered the enemy's detection radius
 */
void EnemyController::detectTarget(shared_ptr<CharacterController> target) {
	_target = target;
	// Start realizing
	_character->setBehaveState(EnemyModel::BehaviorState::REALIZING);
}

/**
 * Called when a target has left this enemy's detection radius, so in endContact when the
 * target ends contact with this enemy's detection sensor. Takes in a pointer to the
 * controller for the target that was lost.
 *
 * Because characters other than Reynard can be pursued, this is generalized to just
 * take in a character controller.
 *
 * @param target    Controller for the target, who's just exited the enemy's detection radius
 */
//void EnemyController::loseTarget(shared_ptr<CharacterController> target) {
//	_target = nullptr;
//	// Stop realizing
//	_character->setBehaveState(EnemyModel::BehaviorState::PATROLLING);
//	// Reset detect time
//	_detectTime = 0;
//};