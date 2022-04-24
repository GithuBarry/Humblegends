//
//  MPTrapDoorModel.hpp
//  Rocket Demo (Mac)
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//
//
//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22
//

#ifndef MPTrapDoorModel_hpp
#define MPTrapDoorModel_hpp

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPTrapModel.hpp"
#include "MPRoomModel.h"

#include <stdio.h>
using namespace cugl;

#pragma mark -
#pragma mark SpikeTrap


class TrapDoor : public TrapModel{

public:

#pragma mark Constructor

    /**
     * Initializes a new trap at the given position
     *
     * Currently hard coded spawn in across bottom of the room.
     *
     * @return  true if the trap is initialized properly, false otherwise.
     */
    bool init(float roomWidth, float roomHeight);


};


#endif /* MPTrapDoorModel_hpp */
