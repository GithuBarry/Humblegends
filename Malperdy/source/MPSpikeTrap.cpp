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
bool SpikeTrap::init(float roomWidth, float roomHeight, bool big) {
    _sceneNode = make_shared<scene2::SpriteNode>();

    //_sceneNode->initWithFile("textures/spikes.png");
    shared_ptr<Texture> s = TrapModel::ASSETS->get<Texture>("spikes");
    _sceneNode->initWithSprite(s, 1, 1);
    _type = TrapType::SPIKE;

    _sceneNode->setAnchor(Vec2::ZERO);
    if (big) {
        _sceneNode->setScale((roomWidth) / _sceneNode->getPolygon().getBounds().getMaxX());
        _sceneNode->setPosition(_sceneNode->getPosition().x, _sceneNode->getPosition().y);
    } else {
        _sceneNode->setScale((roomWidth / 2) / _sceneNode->getPolygon().getBounds().getMaxX());
        _sceneNode->setPosition(_sceneNode->getPosition().x + (roomWidth / 4), _sceneNode->getPosition().y);
    }


    _sceneNode->setAbsolute(true);

    return this->TrapModel::init();
}
