//
//  MPStatueTrapModel.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 5/3/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#include "MPStatueTrapModel.hpp"

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPSapTrap.hpp"

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
bool StatueTrap::init(float roomWidth, float roomHeight){
    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/StatueTrap.png");
    _type = TrapType::STATUE;
    
    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale(.5,.5);
//    _sceneNode->setScale(10,10);
//    _sceneNode->setScale((roomWidth/2) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x +(roomWidth/3), _sceneNode->getPosition().y + (roomHeight/8));
    _sceneNode->setAbsolute(true);
    _sceneNode->setColor(cugl::Color4f::GREEN);

    
    return this->TrapModel::init();
}
