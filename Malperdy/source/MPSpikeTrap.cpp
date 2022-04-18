//
//  MPSpikeTrap.cpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader, Jordan Selin
//  Version: 4/16/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPSpikeTrap.hpp"

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
bool SpikeTrap::init(){
    _polyNode = make_shared<scene2::PolygonNode>();
    _polyNode->initWithFile("textures/spikes.png");
    _type = TrapType::SPIKE;
    
    _polyNode->setAnchor(Vec2::ZERO);
    _polyNode->setScale(720.0 / _polyNode->getPolygon().getBounds().getMaxX());
    _polyNode->setAbsolute(true);

    return this->TrapModel::init();
}
