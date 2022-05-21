//
//  MPCheckpoint.h
//  Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin
//  Version: 5/6/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPCheckpoint_h
#define MPCheckpoint_h

#include <cugl/cugl.h>
#include <stdlib.h>
#include "MPTrapModel.hpp"
#include "MPRoomModel.h"
#include "MPRegionModel.h"

using namespace cugl;

#define LOCK_FADE_TIME 0.6f

class Checkpoint : public TrapModel {
private:
    /** A counter to ensure each checkpoint instance has a unique ID number */
    static int ID_COUNTER;

    /** The unique ID number for this checkpoint */
    int _id;

    /** Whether this checkpoint requires a key (false by default) */
    bool _isLocked = false;

    /** Pointer to the SceneNode with the lock asset */
    shared_ptr<scene2::PolygonNode> _lockNode;

    /** Opacity of lock asset */
    float lockOpacity = 1.0f;

    /** Whether to start fading the lock asset */
    bool startFadeLock = false;

public:
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
    bool init(float roomWidth, float roomHeight, bool isFinal, bool locked = false);

#pragma mark Getters
    /**
     * Returns the unique ID number for this checkpoint.
     * 
     * @return  The unique ID number for this checkpoint
     */
    int getID() { return _id; }

    /**
     * Returns whether this checkpoint is locked and requires a key or not.
     * 
     * @return  Whether this checkpoint requires a key to activate
     */
    bool isLocked() { return _isLocked; }

#pragma mark Setters
    /**
     * Unlocks the checkpoint if it is locked, allowing the lock asset to
     * fade away.
     */
    void unlock();

#pragma mark Update
    void update(float dt) override;

};
#endif /* MPCheckpoint_h */