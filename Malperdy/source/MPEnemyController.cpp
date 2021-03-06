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

/** Radius within which an enemy will detect Reynard, squared */
#define DETECTION_RADIUS_SQUARED 115000.0f

#include "MPEnemyController.h"

shared_ptr<physics2::ObstacleWorld> EnemyController::_obstacleWorld = nullptr;
shared_ptr<ReynardController> EnemyController::_reynard = nullptr;

/**
 * Initializes a new controller for the character at the given position.
 *
 * The character is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos       Initial position in world coordinates
 * @param drawScale The drawing scale (world to screen)
 * @param image     The image for the character's appearance
 *
 * @return  true if the character is initialized properly, false otherwise.
 */
bool EnemyController::init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Animation> animation) {
    // If initialization of parent class failed, return immediately
    if (!(CharacterController::init(pos, drawScale, animation))) return false;
    else return true;
    //// Create user data to store in the physics body
    //BodyData* userData = new BodyData();
    //userData->_type = CharacterType::ENEMY;
    //userData->_controller = (void*)this;
    //// Store pointer to user data in attached physics body
    //_character->getBody()->GetUserData().pointer = (uintptr_t)(void*)userData;
}

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
        CULog("EnemyController: ERROR: set Reynard and ObstacleWorld for EnemyController before creating enemies");
        return;
    }

    // Raycast to Reynard
    reyCast();

    // Handle what the enemy does depending on their current behavior state
    switch (_character->getBehaveState()) {
        case (EnemyModel::BehaviorState::PATROLLING):
            // If Reynard is within detection radius and can be seen, transition to REALIZING
            if (distSquaredToReynard() < DETECTION_RADIUS_SQUARED && _target != nullptr) {
                _character->setBehaveState(EnemyModel::BehaviorState::REALIZING);
            }
            break;
        case (EnemyModel::BehaviorState::REALIZING): {
            // If enough time has passed that enemy realizes Reynard's there
            if (_detectTime > DETECTION_TIME) {
                // Do a little jump
                _character->setVY(JUMP_SPEED_Y / 1.5f);
                // Reset detection time
                _detectTime = 0;
                // Start chasing
                _character->setBehaveState(EnemyModel::BehaviorState::CHASING);
            }
                // If not, tick detection time
            else {
                _detectTime += delta;
            }
        }
            break;
        case (EnemyModel::BehaviorState::CHASING): {
            // TODO: make this proper chase behavior

            // Turn to face the direction of the target
            if (!_target) {
                return;
            }
            Vec2 dir = _target->getPosition() - _character->getPosition();


            if ((dir.x < 0 && _character->isFacingRight()) ||
                    (dir.x > 0 && !_character->isFacingRight()))
                _character->flipDirection();


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
            // For now, just stay in place
            _character->setMoveState(CharacterModel::MovementState::STOPPED);
            break;
        case (EnemyModel::BehaviorState::DEAD):
            _character->setMoveState(CharacterModel::MovementState::DEAD);
            break;
    }
}

#pragma mark -
#pragma mark Behavior Methods

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
void updateLine(Vec2 p1, Vec2 p2, shared_ptr<scene2::SceneNode> parent, string name, Color4 color = Color4(0, 0, 0, 0), float width = 5.0f) {
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
    //updateLine(getPosition(), _reynard->getPosition(), _character->_node, "toReynard", Color4::RED);

    // Raycast to Reynard's location
    // Note that the raycast will detect all fixtures in its path, and it will not necessarily
    // do so in any specific order. We will need to identify what the first fixture in the
    // raycast's path is, as that's the one that actually matters.
    _obstacleWorld->rayCast(
            // This callback controls how the ray cast proceeds by returning a float.
            // If -1, it ignores this fixture and continues. If 0, it terminates the
            // ray cast. If 1, it does not clip the ray and continues. Finally, for
            // any fraction, it clips the ray at that point.
            [this](b2Fixture *fixture, const Vec2 point, const Vec2 normal, float fraction) -> float {
                // Draw line from enemy to point of impact
                if (_debug)
                    updateLine(getScenePosition(), point * _character->_drawScale, _character->_node, "cast", Color4::RED, 5.0f);

                // If Reynard is a fixture in the path, then set the point to start going to
                // Otherwise, set the point as a "null" value
                bool isMyBody = _reynard->isMyBody(fixture->GetBody());
                _raycastCache = isMyBody ? point : Vec2(-1, -1);
                if (!isMyBody) {
                    return -1;
                }
                return fraction;
            },
            _character->getPosition(), _reynard->getCharacter()->getPosition());


    float cast_horizon = static_cast<float>(5 * (_character->isFacingRight() ? 1 : -1) * (_character->getBehaveState() == EnemyModel::BehaviorState::PATROLLING ? 0.2 : 1));
    // TODO: move this somewhere more appropriate
    // Wall jump if there's a wall in front
    //bool wallInFront = false;
    _obstacleWorld->rayCast(
            [this](b2Fixture *fixture, const Vec2 point, const Vec2 normal, float fraction) -> float {
                if (_debug)
                    updateLine(getScenePosition(), point * _character->_drawScale, _character->_node, "cast", Color4::RED, 5.0f);
                if ((!_character->isJumping())
                        && _character->isGrounded()
                        && (!_reynard->isMyBody(fixture->GetBody()))
                        && (
                        abs(_reynard->getCharacter()->getLinearVelocity().y) <= 1
                                || (_character->isOnWall() && _reynard->getCharacter()->getPosition().y > _character->getPosition().y)
                )
                        )
                    if (_character->getBehaveState() == EnemyModel::BehaviorState::CHASING) {
                        jump();
                    } else if (_character->getBehaveState() == EnemyModel::BehaviorState::PATROLLING) {
                        if (!_character->isJumping()) {
                            _character->flipDirection();
                        }

                    }

                return fraction;
            },
            _character->getPosition(), _character->getPosition() + Vec2(cast_horizon, -0.3));

    // If the closest hit fixture really was Reynard, then set this enemy's target accordingly and set new target location
    if (_raycastCache != Vec2(-1, -1)) {
        _target = _reynard->getCharacter();
        _targetLoc = _raycastCache;
    }
        // Otherwise, clear the target, as the enemy has thus lost track of Reynard
    else {
        _target = nullptr;
    }

    // Reset raycast cache for next raycast
    _raycastCache = Vec2(-1, -1);
}

bool EnemyController::isDebug() const {
    return _debug;
}

void EnemyController::setDebug(bool debug) {
    _debug = debug;
}


