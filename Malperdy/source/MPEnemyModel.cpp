//
//  MPEnemyModel.cpp
//  Malperdy
//  
//  This file is based on the CS 4152 PlatformDemo by Walker White, 2021
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 3/10/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#include "MPEnemyModel.h"
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
 * Initializes a new enemy at the given position.
 *
 * The enemy is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos       Initial position in world coordinates
 * @param drawScale The drawing scale (world to screen)
 * @param image     The image for the enemy's appearance
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool EnemyModel::init(const cugl::Vec2& pos, float drawScale, shared_ptr<Animation> animation) {
    // If initialization of parent class failed, return immediately
    if (!(CharacterModel::init(pos, drawScale, animation))) return false;

    // Have enemies be stopped by default
    _moveState = MovementState::STOPPED;
	_currAnimation = "run";
    // Also have enemies be patrolling by default
    _behaveState = BehaviorState::PATROLLING;

	// Set this enemy's max health according to the constant
	_maxHearts = DEFAULT_ENEMY_MAX_HEARTS;
	// Reset this enemy's health to full
	_hearts = _maxHearts;

    return true;
}

/**
 * Updates the enemy's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void EnemyModel::update(float dt) {
	// Update location of detection radius
	/*Vec2 enemyLoc = getPosition() * _drawScale;
	_detectionArea.m_p.Set(enemyLoc.x, enemyLoc.y);*/

	// Handle the enemy's physics depending on their current behavior state
	switch (_behaveState) {
	case (EnemyModel::BehaviorState::PATROLLING):
		// TODO: an actual patrol pattern
		setMoveState(MovementState::STOPPED);
		break;
	case (EnemyModel::BehaviorState::REALIZING):
		// Stop while realizing
		setMoveState(MovementState::STOPPED);
		break;
	case (EnemyModel::BehaviorState::CHASING):
		break;
	case (EnemyModel::BehaviorState::SEARCHING):
		break;
	case (EnemyModel::BehaviorState::RETURNING):
		break;
	}

    // Call parent method at end
    CharacterModel::update(dt);
}

#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 * 
 * In addition to calling the parent method, enemies also have a
 * fixture to act as their detection radius, which is also added here.
 */
void EnemyModel::createFixtures() {
	// Check if there is a body
	if (_body == nullptr) {
		return;
	}

	// Call parent method to get feet/face fixtures
	CharacterModel::createFixtures();

	// Now add a fixture/sensor around the enemy to act as a detection radius
	/*b2CircleShape circle = b2CircleShape();
	circle.m_p = b2Vec2(getWidth() / 2.0f, getHeight() / 2.0f);
	circle.m_radius = _detectionRadius;

	b2FixtureDef sensorDef;
	sensorDef.isSensor = true;
	sensorDef.shape = &circle;
	sensorDef.density = 1.0f;
	sensorDef.userData.pointer = DETECTION_SENSOR_NAME;
	_detectFixture = _body->CreateFixture(&sensorDef);*/
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void EnemyModel::releaseFixtures() {
	if (_body != nullptr) {
		return;
	}

	CharacterModel::releaseFixtures();

	if (_detectFixture != nullptr) {
		_body->DestroyFixture(_detectFixture);
		_detectFixture = nullptr;
	}
}
