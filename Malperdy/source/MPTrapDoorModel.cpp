//
//  MPTrapDoorModel.cpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader, Jordan Selin
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
    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/MP_TrapDoor-1.png");
    _type = TrapType::TRAPDOOR;
    // Inherantly starts activated
    _trapState = TrapModel::TrapState::ACTIVATED;
    
//    FULL SIZE IS 720
    
    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale((roomWidth/2) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x +(roomWidth/4), _sceneNode->getPosition().y + (roomHeight/16));
    _sceneNode->setAbsolute(true);
    
    return this->TrapModel::init();
}

bool TrapDoor::init2(float roomWidth, float roomHeight){
    shared_ptr<JsonReader> jr = JsonReader::allocWithAsset("jsons/framedata");
    shared_ptr<JsonValue> framedata = jr->readJson();
    
    shared_ptr<Texture> sheet = make_shared<Texture>();
    sheet->initWithFile("trapdoorsheet");
    
    
    
}
