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
 * @param locked        Whether this checkpoint requires a key
 * @return  true if the trap is initialized properly, false otherwise.
 */
bool Checkpoint::init(float roomWidth, float roomHeight, bool isFinal, bool locked) {
    // Give this checkpoint an ID number that is unique among all checkpoints
    _id = ID_COUNTER;
    ID_COUNTER++;

    _sceneNode = make_shared<scene2::SpriteNode>();
    shared_ptr<Texture> s = TrapModel::ASSETS->get<Texture>("checkpoint");
    _sceneNode->initWithSprite(s, 6, 5);
    if (isFinal) _type = TrapType::GOAL;
    else _type = TrapType::CHECKPOINT;

    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale((roomWidth / 4) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x + (roomWidth / 4), _sceneNode->getPosition().y + (roomHeight / 8));
    _sceneNode->setAbsolute(true);

    _currFrame = 29;

    // Also display the lock asset if the checkpoint requires a key
    if (locked) {
        _isLocked = locked;
        shared_ptr<Texture> temp =TrapModel::ASSETS->get<Texture>("checkpoint_lock");
        _lockNode = scene2::PolygonNode::allocWithTexture(temp);
        _lockNode->setVisible(true);
        _sceneNode->addChild(_lockNode);
    }

    return this->TrapModel::init();
}

/**
 * Unlocks the checkpoint if it is locked, allowing the lock asset to
 * fade away.
 */
void Checkpoint::unlock() {
    if (_isLocked) {
        _isLocked = false;
        startFadeLock = true;
    }
}

void Checkpoint::update(float dt) {
    TrapModel::update(dt);

    if (startFadeLock) {
        lockOpacity -= dt / LOCK_FADE_TIME;
        if (lockOpacity <= 0) {
            lockOpacity = 0;
            startFadeLock = false;
        }
        _lockNode->setColor(Color4(Vec4(1, 1, 1, lockOpacity)));
    }
}