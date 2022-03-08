//
// Created by Kristina Gu on 3/8/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPCharacterController.h"

#pragma mark -
#pragma mark Hidden Constructors

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
 * @param pos   Initial position in world coordinates
 * @param size  The size of the character in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the character is initialized properly, false otherwise.
 */
bool CharacterController::init(const cugl::Vec2& pos, const cugl::Size& size, float drawScale) {
    _character = CharacterModel::alloc(pos, size, drawScale);
    return (_character != nullptr);
}

#pragma mark -
#pragma mark Actions

/**
 * Turns the character around to face the opposite direction.
 *
 * @return  Whether the character turned successfully.
 */
bool CharacterController::turn(){
    return (!_character->isFalling());

    _character->flipDirection();
    return true;
    
}

/**
 * The character jumps upwards at a set velocity.
 *
 * @return  Whether the character jumped successfully
 */
bool CharacterController::jump() {
    return _character->setMoveState(CharacterModel::MovementState::JUMPING);
}

/**
 * Stops the character from moving.
 *
 * @return  Whether the character stopped moving successfully
 */
bool CharacterController::stop() {
    return _character->setMoveState(CharacterModel::MovementState::STOPPED);
}

/**
 * Starts the character running in the direction they're facing.
 *
 * @return  Whether the character started running successfully
 */
bool CharacterController::run() {
    return _character->setMoveState(CharacterModel::MovementState::RUNNING);
}

/**
 * Sets the character to be falling through the air.
 *
 * @return  Whether the character was set to be falling successfully
 */
bool CharacterController::fall() {
    return _character->setMoveState(CharacterModel::MovementState::FALLING);
}

/**
 * Sets the character to be sliding down a wall for a short amount of time.
 *
 * @return  Whether the character is successfully stuck to the wall
 */
bool CharacterController::stickToWall() {
    return _character->setMoveState(CharacterModel::MovementState::ONWALL);
}

/**
 * If the character is sliding down a wall already, unsticks them and drops
 * them to the ground.
 *
 * @return  Whether the character is successfully unstuck from the wall
 */
bool CharacterController::unstickFromWall() {
    return _character->setMoveState(CharacterModel::MovementState::FALLING);
}

/**
 * Sets the character to be on the ground.
 *
 * @return  Whether the character is successfully marked as being on the ground
 */
bool CharacterController::land() {
    return _character->setMoveState(CharacterModel::MovementState::RUNNING);
}

/** [update] This will automatically update Reynard's position and look at other
 * things in the future like health.*/
void CharacterController::update(float delta){
    //_reynard->applyForce();
    _character->update(delta);
}