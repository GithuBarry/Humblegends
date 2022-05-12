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

#include "MPTrapModel.hpp"
#include "MPCheckpoint.h"

using namespace cugl;

shared_ptr<vector<shared_ptr<Texture>>> Checkpoint::_bgsCleared = make_shared<vector<shared_ptr<Texture>>>();

#pragma mark -
#pragma mark Constructors

/*
 * Initializes a new checkpoint for a level in the given region.
 *
 * @param roomWidth     The width of the room the checkpoint is in
 * @param roomHeight    The height of the room the checkpoint is in
 * @param isFinal       Whether this is a "final" checkpoint, meaning a goal
 * @param region        An integer from 1-3 indicating which region this checkpoint is in
 * @return  true if the trap is initialized properly, false otherwise.
 */
bool Checkpoint::init(float roomWidth, float roomHeight, bool isFinal, int region) {
    _region = region;

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

/**
 * Clears all the rooms associated with this checkpoint (backgrounds are swapped to
 * the "cleared" option for the associated region).
 *
 * @return  Whether the checkpoint's associated rooms were cleared successfully
 */
bool Checkpoint::clearCheckpoint() {
    // Don't do anything if already cleared
    if (_cleared) return false;

    CULog("CHECKPOINT: cleared");

    // Get the correct cleared background for this checkpoint's region
    shared_ptr<Texture> bgCleared = _bgsCleared->at(_region - 1);

    // For each room linked to this checkpoint, swap to the associated cleared background
    for (vector<shared_ptr<RoomModel>>::iterator itr = _rooms->begin(); itr != _rooms->end(); ++itr) {
        (*itr)->_bgNode->setTexture(bgCleared);
    }

    _cleared = true;
    return true;
}