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

#include "MPReynardController.h"

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
bool ReynardController::init(const cugl::Vec2& pos, float drawScale, shared_ptr<map<string, CharacterModel::Animation>> animations) {
    // If initialization of parent class failed, return immediately
    return CharacterController::init(pos, drawScale, animations);

    //// Create user data to store in the physics body
    //BodyData* userData = new BodyData();
    //userData->_type = CharacterType::REYNARD;
    //userData->_controller = (void*)this;
    //// Store pointer to user data in attached physics body
    //_character->getBody()->GetUserData().pointer = (uintptr_t)(void*)userData;
}

/**
 * This method handles anything about the character that needs to change over time.
 *
 * @param delta The amount of time that has passed since the last frame
 */
void ReynardController::update(float delta) {
	// Call parent method at the end
	CharacterController::update(delta);
    if (lastCheckPointPosition == Vec2(-1,-1)){
        lastCheckPointPosition = getPosition();
    }
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
    //if ((dir.x < 0 && _character->isFacingRight()) ||
    //    (dir.x > 0 && !_character->isFacingRight())) turn();
    turn();

    // Take damage (TODO: make this not a constant)
    _character->_hearts--;
}
