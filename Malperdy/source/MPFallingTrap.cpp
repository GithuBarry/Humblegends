//
//  MPFallingTrap.cpp
//  Malperdy
//
//  Created by Abu Qader on 4/28/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPFallingTrap.h"
#include "MPReynardController.h"


using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new trap at the given position
 *
 * Currently hard coded spawn in across bottom of the room.
 *
 * @return  true if the trap is initialized properly, false otherwise.
 */
bool FallingTrap::init(){
    CULog("INIT CALLED");
    _polyNode = make_shared<scene2::PolygonNode>();
    _polyNode->initWithFile("textures/spikes.png");
    _type = TrapType::FALLING;
    
    _polyNode->setAnchor(Vec2::ZERO);
    _polyNode->setScale(720.0 / _polyNode->getPolygon().getBounds().getMaxX());
    _polyNode->setScale(_polyNode->getScale() * Vec2(1,0.5));
    _polyNode->setAbsolute(true);
    _polyNode->setColor(cugl::Color4f::ORANGE);
    return this->TrapModel::init();
}

void FallingTrap::fall() {
    _obstacle->setVY(-1);
}

void FallingTrap::update(const shared_ptr<ReynardController>& reynard) {
    Vec2 _reyPos = reynard->getScenePosition();
    Vec2 _trapPos = _obstacle->getPosition();
    if (_reyPos.y == _trapPos.y) {
        CULog("ASDFKJASKDLFJASKDLFJAMSDKFa");
        fall();
    }
    
}
