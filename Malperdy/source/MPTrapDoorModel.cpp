//
//  MPTrapDoorModel.cpp
//  Rocket Demo (Mac)
//
//  Created by Spencer Hurst on 3/27/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//
//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

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
bool TrapDoor::init(){
    
    _polyNode = make_shared<scene2::PolygonNode>();
    _polyNode->initWithFile("textures/spikes.png");
    _type = "trapdoor";
    // Inherantly starts activated
    _trapState = TrapModel::TrapState::ACTIVATED;
    
    
    _polyNode->setAnchor(Vec2::ZERO);
    _polyNode->setScale(720.0 / _polyNode->getPolygon().getBounds().getMaxX());
    _polyNode->setAbsolute(true);
    _polyNode->setColor(Color4::GREEN);
    
    return this->TrapModel::init();
    
}
