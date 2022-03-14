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

#include "MPTrapModel.h"
#include "MPSpikeTrap.h"


using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool SpikeTrap::init(const Vec2 &pos, float drawScale, shared_ptr<Texture> image){
    if(!TrapModel::init(pos, drawScale, image) return false){
        return false;
    }
}
