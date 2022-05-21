//
//  MPTutorial.cpp
//  Rocket Demo
//
//  Created by Spencer Hurst on 5/14/22.
//  Copyright © 2022 Game Design Initiative at Cornell. All rights reserved.
//

#include "MPTutorial.hpp"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include <map>


using namespace cugl;

#pragma mark -
#pragma mark Constructors

bool Tutorial::init(const cugl::Vec2 &pos, float width = 3, float height = 3) {
    if (!(BoxObstacle::init(pos, Size(width, height)))) return false;
    setGravityScale(0);
    setPosition(pos.x, pos.y);
    setSensor(true);

    return true;
};


void Tutorial::dispose() {
    _node = nullptr;
};
    

