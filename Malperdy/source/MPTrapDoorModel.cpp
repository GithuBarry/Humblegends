//
//  MPTrapDoorModel.cpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader
//  Version: 4/16/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPTrapDoorModel.hpp"

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPTrapDoorModel.hpp"

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
bool TrapDoor::init(float roomWidth, float roomHeight){
    _polyNode = make_shared<scene2::SpriteNode>();
    _polyNode->initWithFile("textures/MP_TrapDoor-1.png");
    _type = TrapType::TRAPDOOR;
    // Inherantly starts activated
    _trapState = TrapModel::TrapState::ACTIVATED;
    
//    FULL SIZE IS 720
    
    _polyNode->setAnchor(Vec2::ZERO);
    _polyNode->setScale((roomWidth/2) / _polyNode->getPolygon().getBounds().getMaxX());
    _polyNode->setPosition(_polyNode->getPosition().x +(roomWidth/4), _polyNode->getPosition().y + (roomHeight/16));
    _polyNode->setAbsolute(true);
    
    return this->TrapModel::init();
}
