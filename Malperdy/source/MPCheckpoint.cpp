//
//  MPCheckpoint.cpp
//  Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin
//  Version: 4/19/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPCheckpoint.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/*
* Initializes a new checkpoint
*
* @return  true if the trap is initialized properly, false otherwise.
*/
bool Checkpoint::init(float roomWidth, float roomHeight) {
    _polyNode = make_shared<scene2::PolygonNode>();
    _polyNode->initWithFile("textures/checkpoint.png");
    _type = TrapType::CHECKPOINT;

    _polyNode->setAnchor(Vec2::ZERO);
    _polyNode->setScale((roomWidth / 4) / _polyNode->getPolygon().getBounds().getMaxX());
    _polyNode->setPosition(_polyNode->getPosition().x + (roomWidth / 4), _polyNode->getPosition().y + (roomHeight / 8));
    _polyNode->setAbsolute(true);

    return this->TrapModel::init();
}