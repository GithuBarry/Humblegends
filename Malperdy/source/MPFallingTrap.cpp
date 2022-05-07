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
    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/falling.png");
    _type = TrapType::FALL;
    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale((roomWidth/2) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x +(roomWidth/4), _sceneNode->getPosition().y);
    _sceneNode->setAbsolute(true);
    _sceneNode->setColor(cugl::Color4f::BLUE);
    return this->TrapModel::init();
}

bool FallingTrap::init(const cugl::Vec2 &pos){
    CULog("POS %f %f", pos.x, pos.y);
    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/falling.png");
    _type = TrapType::FALL;
    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setPosition(pos);
    _sceneNode->setAbsolute(true);
    _sceneNode->setColor(cugl::Color4f::BLUE);
    return this->TrapModel::init();
}

void FallingTrap::fall() {
    CULog("[MPFallingTrap] Reynard detected");
    Vec2 obPos = _obstacle->getPosition();
    _obstacle->PolygonObstacle::setBodyType(b2_dynamicBody);
    _obstacle->setVY(-1);
}

void FallingTrap::update(const shared_ptr<ReynardController>& reynard, float dt) {
    Vec2 _reyPos = reynard->getScenePosition();
    Vec2 _trapPos = _sceneNode->getWorldPosition();
    if (roundf(_reyPos.x) == roundf(_trapPos.x)) {
        fall();
        fallen = true;
    }
    _obstacle->PolygonObstacle::update(dt);
    
    // if (_sceneNode != nullptr) {
    //    _sceneNode->setPosition(_obstacle->getPosition());
    //    _sceneNode->setAngle(getAngle());
    // }

}

void FallingTrap::dispose(){
    _sceneNode = nullptr;
};
