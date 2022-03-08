//
// Created by Spencer Hurst on 2/25/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#define VELOCITY    10
#define DUDE_JUMP   5.5f

#include "MPReynardController.h"

// TODO: construct a ReynardModel within the ReynardController
/**This is the constructor of the Reynard Controller**/
//ReynardController::ReynardController(shared_ptr<ReynardModel> r){
//    _reynard = r;
//}

// TODO: add alloc method

/**
 * Initializes a new controller for Reynard at the given position.
 *
 * Reynard is sized according to the given drawing scale.
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
 * @return  true if Reynard is initialized properly, false otherwise.
 */
bool ReynardController::init(const cugl::Vec2& pos, const cugl::Size& size, float drawScale) {
    _reynard = ReynardModel::alloc(pos, size, drawScale);
    return (_reynard != nullptr);
}