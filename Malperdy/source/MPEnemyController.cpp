//
//  MPEnemyController.cpp
//  Malperdy
// 
//  This class handles the behavior of a single enemy. Before any enemies are created to populate the
//  GameScene, the static references to Reynard and the ObstacleWorld must be set; otherwise, the enemies
//  will be inert and not do anything.
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

shared_ptr<physics2::ObstacleWorld> EnemyController::_obstacleWorld = nullptr;
shared_ptr<ReynardController> EnemyController::_reynard = nullptr;

/**
 * This method handles anything about the character that needs to change over time.
 *
 * @param delta The amount of time that has passed since the last frame
 */
void EnemyController::update(float delta) {
	// Call parent method first
	CharacterController::update(delta);

	// Don't update any behavior if the enemy doesn't have a reference to Reynard or the ObstacleWorld yet
	if (_reynard == nullptr || _obstacleWorld == nullptr) {
		CULog("ERROR: set Reynard and ObstacleWorld for EnemyController before creating enemies");
		return;
	}

	// Raycast to Reynard
	reyCast();

	// Handle what the enemy does depending on their current behavior state
	switch (_character->getBehaveState()) {
	case (EnemyModel::BehaviorState::PATROLLING):
		// TODO: handle transition to REALIZING based on target status
		break;
	case (EnemyModel::BehaviorState::REALIZING):
	{
		// TODO: replace detection with something without sensors

		CULog("Detection time: %f", _detectTime);
		// If enough time has passed that enemy realizes Reynard's there
		if (_detectTime > DETECTION_TIME) {
			// Do a little jump
			jump();
			// Reset detection time
			_detectTime = 0;
			// Add target's current location to the enemy's next location queue
			_character->_futureMoveLocations->push_back(_target->getPosition());
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
		// TODO: make this proper chase behavior

		// CULog("CHASING");
		// Turn to face the direction of the target
		Vec2 dir = _target->getPosition() - _character->getPosition();
		if ((dir.x < 0 && _character->isFacingRight()) ||
			(dir.x > 0 && !_character->isFacingRight())) _character->flipDirection();
		// TODO: if target is within attack range, attack

		// For now, if target is too close, stop
		if (abs(dir.x) <= 2.0f) _character->setMoveState(CharacterModel::MovementState::STOPPED);
		// Otherwise, chase them
		else _character->setMoveState(CharacterModel::MovementState::RUNNING);

		// If no such point, so enemy lost the target, move enemy to Searching state
		//if (!onTheTrail) _character->setBehaveState(EnemyModel::BehaviorState::SEARCHING);
	}
		break;
	case (EnemyModel::BehaviorState::SEARCHING):
		// TODO: proper searching behavior
		_character->setMoveState(CharacterModel::MovementState::STOPPED);
		break;
	case (EnemyModel::BehaviorState::RETURNING):
		// TODO: does the enemy go home? Or does it just give up and start patrolling where it is?

		break;
	}
}

/**
 * Helper debug function that updates a debug line drawn to help with raycasting.
 * 
 * It creates the line between the two given points and attaches it to the given
 * parent node with the given name. If there is an existing line with that name
 * already attached to the parent, it will be removed first.
 * 
 * Both points should be given in world space, and will be transformed accordingly when
 * the resulting line is attached to the given node.
 * 
 * @param p1		The first point to create the line with in world space
 * @param p2		The second point to create the line with in world space
 * @param parent	The node to attach the created line to
 * @param name		The name to give to the line, to track what to remove
 * @param color		The color of the resulting line (green by default)
 * @param width		The width of the resulting line (5 by default)
 * @return linePoly	The line between the two given points as a PolygonNode
 */
void updateLine(Vec2 p1, Vec2 p2, shared_ptr<scene2::SceneNode> parent, string name, Color4 color = Color4::GREEN, float width = 5.0f) {
	// Create line
	Path2 line;
	vector<Vec2> points;
	points.push_back(parent->worldToNodeCoords(p1));
	points.push_back(parent->worldToNodeCoords(p2));
	line.set(points);
	SimpleExtruder se = SimpleExtruder(line);
	se.calculate(width);
	shared_ptr<scene2::PolygonNode> linePoly = scene2::PolygonNode::alloc();
	linePoly->setPolygon(se.getPolygon());
	linePoly->setColor(color);
	linePoly->setAbsolute(true);

	// Remove old line if any
	parent->removeChildByName(name);
	// Now add updated line to parent
	parent->addChildWithName(linePoly, name);
}

/**
 * Performs a raycast from this enemy to Reynard. If the raycast succeeds, then the enemy's
 * target and/or target location will be updated accordingly. The new values of these variables
 * will then drive changing between behavior states in the update() method of this class.
 * 
 * This method is called and performed at the start of each frame in the update() method
 * above, but the actual raycast is performed asynchronously. Therefore, there may be a
 * slight delay in a raycast succeeding and the enemy's behavior changing, although that's
 * probably fine because it simulates slow brain.
 */
void EnemyController::reyCast() {

	// Draw line from enemy to Reynard
	updateLine(getPosition(), _reynard->getPosition(), _character->_node, "toReynard");

	// Raycast to Reynard's location
	// Note that the raycast will detect all fixtures in its path, and it will not necessarily
	// do so in any specific order. We will need to identify what the first fixture in the
	// raycast's path is, as that's the one that actually matters.
	_obstacleWorld->rayCast(
		// This callback controls how the ray cast proceeds by returning a float.
		// If -1, it ignores this fixture and continues. If 0, it terminates the
		// ray cast. If 1, it does not clip the ray and continues. Finally, for
		// any fraction, it clips the ray at that point.
		[this](b2Fixture* fixture, const Vec2 point, const Vec2 normal, float fraction)->float{
			// Draw line from enemy to point of impact
			updateLine(getPosition(), point * _character->_drawScale, _character->_node, "cast", Color4::RED, 5.0f);

			// If Reynard is a fixture in the path, then set the point to start going to
			// Otherwise, set the point as a "null" value
			_raycastCache = _reynard->isMyBody(fixture->GetBody()) ? point : Vec2(-1, -1);

			return fraction;
		},
		_character->getPosition(), _reynard->getCharacter()->getPosition());

	// If the closest hit fixture really was Reynard, then set this enemy's target accordingly
	if (_raycastCache != Vec2(-1, -1)) {
		_target = _reynard->getCharacter();
		_targetLoc = _raycastCache;
		_raycastCache = Vec2(-1, -1);
	}

	//onTheTrail = _target->getBody()->GetFixtureList()->RayCast(output, input, 1);

	//CULog("Fraction: %f", output->fraction);

	// If raycast made it to its destination
	//if (output->fraction == 1.0f) {
	//	// Start running towards target and break
	//	/*_character->setMoveState(CharacterModel::MovementState::RUNNING);
	//	break;*/
	//	CULog("TARGET SPOTTED");
	//}
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