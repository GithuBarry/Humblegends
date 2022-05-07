//
//  MPArrow.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 5/7/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#include "MPArrow.hpp"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include <map>

#define ARROW_SPEED   20.0f
#define ARROW_OFFSET  10.0f

using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool Arrow::init(const cugl::Vec2 &pos, float drawScale, bool right){
    if(!(BoxObstacle::init(pos, Size(10,5)))) return false;
    setGravityScale(0);
    float offset = (right ? ARROW_OFFSET : -ARROW_OFFSET);
    setPosition(pos.x + ARROW_OFFSET, pos.y)
    float speed  = (right ? ARROW_SPEED : -ARROW_SPEED);
    setVX(speed);

    return true;
};


void Arrow::dispose(){
    _node = nullptr;
};
    
    


