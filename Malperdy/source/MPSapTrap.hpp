//
//  MPSapTrap.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 4/24/22.
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPSapTrap_hpp
#define MPSapTrap_hpp

#include <stdio.h>

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include "MPTrapModel.hpp"
#include "MPRoomModel.h"

using namespace cugl;


#pragma mark -
#pragma mark SapTrap


class SapTrap : public TrapModel{

public:

#pragma mark Constructor

    /**
     * Initializes a new trap at the given position
     *
     * Currently hard coded spawn in across bottom of the room.
     *
     * @return  true if the trap is initialized properly, false otherwise.
     */
    bool init();


};


#endif /* MPSapTrap_hpp */
