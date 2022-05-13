//
//  FallingTrap.cpp
//  Malperdy
//
//  Created by Abu Qader on 5/7/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#include "MPFallingTrap.hpp"

#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include "MPReynardController.h"


#pragma mark -
#pragma mark Initializers

using namespace cugl;


// _node = scene2::SceneNode::alloc();
// _node->initWithFile("textures/falling.png");

bool FallingTrap::init(const Poly2& poly, const Vec2 origin) {
    if (PolygonObstacle::init(poly, origin)) {
        return true;
    }
    return false;
}

bool FallingTrap::initWithAnchor(const Poly2& poly, const Vec2 anchor) {
    if (PolygonObstacle::initWithAnchor(poly, anchor)) {
        return true;
    }
    return false;
}

void FallingTrap::fall() {
    CULog("FAD:KAFJSLADKFDASD");
    setVX(10);
}

void FallingTrap::update(const shared_ptr<ReynardController>& reynard, float dt) {
    Vec2 _reyPos = reynard->getSce iternePosition();
    Vec2 _trapPos = _node->getWorldPosition();
    CULog("trap x %f y %f", _trapPos.x, _trapPos.y);
    CULog("rap x %f y %f", _reyPos.x, _reyPos.y);
    
    if (roundf(_reyPos.x) == roundf(_trapPos.x)) {
        
        fall();
        fallen = true;
    }
    PolygonObstacle::update(dt);
    
    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
    }

}
