//
//  MPTrapDoorModel.hpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader
//  Version: 3/10/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
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


class TrapDoor : public TrapModel {

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


    /**
     * Change the traps animation/image when you run into it. I need a way to do this
     *.
     *
     */
    void changeTexture() {

    }

};


#endif /* MPTrapDoorModel_hpp */
