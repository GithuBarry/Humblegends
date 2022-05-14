//
//  MPCheckpoint.cpp
//  Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin
//  Version: 5/6/22
//
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPCheckpoint.h"

using namespace cugl;

int Checkpoint::ID_COUNTER = 0;

#pragma mark -
#pragma mark Constructors

/*
 * Initializes a new checkpoint for a level in the given region.
 *
 * @param roomWidth     The width of the room the checkpoint is in
 * @param roomHeight    The height of the room the checkpoint is in
 * @param isFinal       Whether this is a "final" checkpoint, meaning a goal
 * @return  true if the trap is initialized properly, false otherwise.
 */
bool Checkpoint::init(float roomWidth, float roomHeight, bool isFinal) {
    // Give this checkpoint an ID number that is unique among all checkpoints
    _id = ID_COUNTER;
    ID_COUNTER++;

    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/checkpoint.png");
    if (isFinal) _type = TrapType::GOAL;
    else _type = TrapType::CHECKPOINT;

    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale((roomWidth / 4) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x + (roomWidth / 4), _sceneNode->getPosition().y + (roomHeight / 8));
    _sceneNode->setAbsolute(true);

    return this->TrapModel::init();
}