//
//  MPSpikeTrap.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//
//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

#ifndef MPSpikeTrap_hpp
#define MPSpikeTrap_hpp

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPTrapModel.hpp"

#include <stdio.h>
using namespace cugl;

#pragma mark -
#pragma mark SpikeTrap


class SpikeTrap : public TrapModel{
    
public:
    
#pragma mark Constructor
    
    
    
    bool init(const Vec2 &pos, float drawScale, shared_ptr<Texture> image);

    
};


#endif /* MPSpikeTrap_hpp */
