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

using namespace cugl;

class Checkpoint : public TrapModel {
public:

#pragma mark Constructors
    /*
     * Initializes a new checkpoint
     *
     * @return  true if the trap is initialized properly, false otherwise.
     */
    bool init(float roomWidth, float roomHeight, bool isFinal);

};

#endif /* MPCheckpoint_h */
