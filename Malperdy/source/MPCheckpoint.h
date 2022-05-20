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

class Checkpoint : public TrapModel {
private:
    /** A counter to ensure each checkpoint instance has a unique ID number */
    static int ID_COUNTER;

    /** The unique ID number for this checkpoint */
    int _id;

<<<<<<< HEAD
    /** Whether or not this checkpoint is a locked one */
    bool _isLocked = false;

=======
    /** Whether this checkpoint requires a key (false by default) */
    bool _isLocked = false;

    /** Pointer to the SceneNode with the lock asset */
    shared_ptr<scene2::PolygonNode> _lockNode;

>>>>>>> main
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
<<<<<<< HEAD
     * Returns whether this checkpoint is locked and so requires
     * a key.
     * 
     * @return  Whether this checkpoint is locked
=======
     * Returns whether this checkpoint is locked and requires a key or not.
     * 
     * @return  Whether this checkpoint requires a key to activate
>>>>>>> main
     */
    bool isLocked() { return _isLocked; }

};
#endif /* MPCheckpoint_h */