//
//  MPSpikeTrap.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.

//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

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
    _type = "spike";
    
    _polyNode->setAnchor(Vec2::ZERO);
    _polyNode->setScale(720.0 / _polyNode->getPolygon().getBounds().getMaxX()/2);
    _polyNode->setAbsolute(true);
//    CULog("min X: %f", _polyNode->getPolygon().getBounds().getMinX());
//    CULog("min Y: %f", _polyNode->getPolygon().getBounds().getMinY());
//    CULog("max X: %f", _polyNode->getPolygon().getBounds().getMaxX());
//    CULog("max Y: %f", _polyNode->getPolygon().getBounds().getMaxY());
    CULog("scale: %f", 720.0 / _polyNode->getPolygon().getBounds().getMaxX());
//    _polyNode->setPolygon(poly);
//    _polyNode->setColor(Color4::CYAN);
   return this->TrapModel::init();
}
