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
	// Call parent method first
	CharacterController::update(delta);

	// Handle what the enemy does depending on their current behavior state
	switch (_character->getBehaveState()) {
	case (EnemyModel::BehaviorState::PATROLLING):
		break;
	case (EnemyModel::BehaviorState::REALIZING):
	{
		CULog("Detection time: %f", _detectTime);
		// If enough time has passed that enemy realizes Reynard's there
		if (_detectTime > DETECTION_TIME) {
			// Do a little jump
			jump();
			// Reset detection time
			_detectTime = 0;
			// Add target's current location to the enemy's next location queue
			_futureMoveLocations->push_back(_target->getPosition());
			// Start chasing
			_character->setBehaveState(EnemyModel::BehaviorState::CHASING);
		}
		// If not, tick detection time
		else {
			_detectTime += delta;
		}
	}
		break;
	case (EnemyModel::BehaviorState::CHASING):
	{
		CULog("CHASING");
		// Turn to face the direction of the target
		Vec2 dir = _target->getPosition() - _character->getPosition();
		if ((dir.x < 0 && _character->isFacingRight()) ||
			(dir.x > 0 && !_character->isFacingRight())) _character->flipDirection();
		// TODO: if target is within attack range, attack

		// Otherwise, start chasing target by following their trail
		shared_ptr<deque<Vec2>> currTrail = _target->getTrail();
		// Initialize values for input and output of raycast
		/*b2RayCastInput input;
		input.p1 = b2Vec2(_character->getX() * _character->_drawScale, _character->getY() * _character->_drawScale);
		input.maxFraction = 1;
		b2RayCastOutput *output = new b2RayCastOutput();*/
		// Boolean for whether a raycast succeeded
		bool onTheTrail = false;
		// For each point on the target's trail, starting with the closest point to the target
		for (deque<Vec2>::iterator itr = currTrail->begin(); itr != currTrail->end(); ++itr) {
			// Raycast from self to the point on target's trail closest to the target
			_obstacleWorld->rayCast( // Lambda expression for what happens on a hit
				[this](b2Fixture* fixture, const Vec2 point, const Vec2 normal, float fraction)->float{
					// If there is line of sight to the hit location, add that to future movement queue
					if (fraction >= 1.0f) _futureMoveLocations->push_back(point);
				},
				_character->getPosition(), *itr);

			//input.p2 = b2Vec2((*itr).x * _character->_drawScale, (*itr).y * _character->_drawScale);
			//onTheTrail = _target->getBody()->GetFixtureList()->RayCast(output, input, 1);

			//CULog("Fraction: %f", output->fraction);

			//// draw raycast line (for some reason, always seems to go into the ground?)
			//Path2 line;
			//vector<Vec2> points;
			//points.push_back(_character->getPosition());
			//points.push_back(*itr);
			//line.set(points);
			//SimpleExtruder se = SimpleExtruder(line);
			//se.calculate(5.0f);
			//shared_ptr<scene2::PolygonNode> linePoly = scene2::PolygonNode::alloc();
			//linePoly->setPolygon(se.getPolygon());
			//linePoly->setColor(Color4::GREEN);
			//linePoly->setAbsolute(true);
			//_character->_node->addChild(linePoly);
			//linePoly->setAbsolute(true);

			//// If raycast made it to its destination
			//if (onTheTrail && output->fraction == 1.0f) {
			//	// Start running towards target and break
			//	_character->setMoveState(CharacterModel::MovementState::RUNNING);
			//	break;
			//}
		}

		// If no such point, so enemy lost the target, move enemy to Searching state
		if (!onTheTrail) _character->setBehaveState(EnemyModel::BehaviorState::SEARCHING);
	}
		break;
	case (EnemyModel::BehaviorState::SEARCHING):
		// TODO: proper searching behavior
		_character->setMoveState(CharacterModel::MovementState::STOPPED);
		break;
	case (EnemyModel::BehaviorState::RETURNING):
		break;
	}
}

#pragma mark -
#pragma mark Behavior Methods

/**
 * Called when a target has entered this enemy's detection radius, so in beginContact
 * when the target begins contact with this enemy's detection sensor. Takes in a pointer
 * to the model for the target.
 *
 * Because characters other than Reynard can be pursued, this is generalized to just
 * take in a character model.
 * 
 * TODO: handle issues with multiple possible targets
 *
 * @param target    The target who's just entered the enemy's detection radius
 */
void EnemyController::detectTarget(shared_ptr<CharacterModel> target) {
	// Only continue if the enemy isn't already chasing someone
	if (_character->getBehaveState() == EnemyModel::BehaviorState::CHASING) return;
	_target = target;
	// Start realizing
	_character->setBehaveState(EnemyModel::BehaviorState::REALIZING);
}

/**
 * Called when a target has left this enemy's detection radius, so in endContact when the
 * target ends contact with this enemy's detection sensor. Takes in a pointer to the
 * model for the target that was lost.
 *
 * Because characters other than Reynard can be pursued, this is generalized to just
 * take in a character model.
 * 
 * TODO: handle issues with multiple possible targets
 *
 * @param target	The target who's just exited the enemy's detection radius
 */
void EnemyController::loseTarget(shared_ptr<CharacterModel> target) {
	_target = nullptr;
	// Stop realizing
	_character->setBehaveState(EnemyModel::BehaviorState::PATROLLING);
	// Reset detect time
	_detectTime = 0;
};