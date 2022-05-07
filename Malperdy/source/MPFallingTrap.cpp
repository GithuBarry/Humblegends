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
#include "MPFallingTrap.hpp"
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

bool FallingTrap::init(float roomWidth, float roomHeight){
    CULog("INIT CALLED");
    _obstacle = make_shared<cugl::physics2::PolygonObstacle>();
    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/spikes.png");
    _type = TrapType::FALL;

    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale((roomWidth/2) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x +(roomWidth/4), _sceneNode->getPosition().y);
    _sceneNode->setAbsolute(true);
    _sceneNode->setColor(cugl::Color4f::BLUE);
    
    _obstacle->setPosition(_sceneNode->getPosition().x +(roomWidth/4), _sceneNode->getPosition().y);
    _obstacle->setVY(0);
    _obstacle->setVX(0);
    this->TrapModel::init();
    return this->TrapModel::initObstacle(_obstacle);
}


void FallingTrap::fall() {
    CULog("ASDCAVCKLADSJFCKLAS");
    Vec2 obPos = _obstacle->getPosition();
    CULog("OBSTACLE: X %f Y %f", obPos.x, obPos.y);
    _obstacle->setVY(-10);
    CULog("OBSTACLE: X %f Y %f", obPos.x, obPos.y);
}

void FallingTrap::update(const shared_ptr<ReynardController>& reynard) {
    Vec2 _reyPos = reynard->getScenePosition();
    Vec2 _trapPos = _sceneNode->getWorldPosition();
    CULog("TRAP: X %f", _trapPos.x);
    CULog("TRAP: Y %f", _trapPos.y);
    CULog("REY: X %f", _reyPos.x);
    if (roundf(_reyPos.x) == roundf(_trapPos.x)) {
        fall();
        fallen = true;
    }
    
}
