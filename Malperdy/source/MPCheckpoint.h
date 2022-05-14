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

public:
#pragma mark Constructors
    /*
     * Initializes a new checkpoint for a level in the given region.
     *
     * @param roomWidth     The width of the room the checkpoint is in
     * @param roomHeight    The height of the room the checkpoint is in
     * @param isFinal       Whether this is a "final" checkpoint, meaning a goal
     * @return  true if the trap is initialized properly, false otherwise.
     */
    bool init(float roomWidth, float roomHeight, bool isFinal);

#pragma mark Getters
    /**
     * Returns the unique ID number for this checkpoint.
     * 
     * @return  The unique ID number for this checkpoint
     */
    int getID() { return _id; }

};
#endif /* MPCheckpoint_h */