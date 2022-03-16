//
//  MPSpikeTrap.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.

//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPSpikeTrap.hpp"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new SpikeTrap at the given position.
 *
 * The SpikeTrap is sized according to the given drawing scale.
 *
 * @param pos       Initial position in world coordinates
 * @param drawScale The drawing scale (world to screen)
 * @param image     The image for the character's appearance
 *
 * @return  true if the character is correctly initialized, false otherwise.
 */
//bool SpikeTrap::init(float x, float y, shared_ptr<Texture> image){
//
//    if(!(TrapModel::init(x,y))) return false;
////     Spike Traps are initially set to being deactivated
//    _trapState = TrapModel::TrapState::DEACTIVATED;
//    return true;
//}
