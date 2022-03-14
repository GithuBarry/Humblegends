//
//  MPSpikeTrap.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>


using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool SpikeTrap::init(const Vec2 &pos, float drawScale, shared_ptr<Texture> image){
    if(!TrapModel::init(pos, drawScale, image) return false){
        return false;
    }
    
}
