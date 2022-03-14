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
bool EnemyModel::init(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> image) {
    // If initialization of parent class failed, return immediately
    if (!(CharacterModel::init(pos, drawScale, image))) return false;

    // Have enemies be stopped by default
    _moveState = MovementState::STOPPED;
    // Also have enemies be patrolling by default
    _behaveState = BehaviorState::PATROLLING;

	// DEBUG: Create a detection radius around the enemy
	PolyFactory pf;
	Poly2 circle = pf.makeCircle(getPosition() * _drawScale, _detectionRadius * _drawScale);
	shared_ptr<scene2::PolygonNode> pn = scene2::PolygonNode::allocWithPoly(circle);
	Color4 color = Color4::CORNFLOWER;
	pn->setColor(color.scale(1.0f, 1.0f, 1.0f, 0.3f));
	_node->addChild(pn);
	pn->setAnchor(0.5f, 0.5f);
	//pn->setPosition(getWidth() * _drawScale / 2.0f, getHeight() * _drawScale / 2.0f);
	//pn->setPosition(0, 0);

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
	Vec2 enemyLoc = getPosition() * _drawScale;
	_detectionArea.m_p.Set(enemyLoc.x, enemyLoc.y);

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

    // Call parent method at end
    CharacterModel::update(dt);
}