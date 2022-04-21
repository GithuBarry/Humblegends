//
//  MPCheckpoint.h
//  Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin
//  Version: 4/19/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPCheckpoint_h
#define MPCheckpoint_h

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPTrapModel.hpp"
#include "MPRoomModel.h"

#include <stdio.h>
using namespace cugl;

class Checkpoint : public TrapModel {

public:

#pragma mark Constructor

    /*
     * Initializes a new checkpoint
     *
     * @return  true if the trap is initialized properly, false otherwise.
     */
    bool init(float roomWidth, float roomHeight);
};

#endif /* MPCheckpoint_h */
