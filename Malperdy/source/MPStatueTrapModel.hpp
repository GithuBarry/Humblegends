//
//  MPStatueTrapModel.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 5/3/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPStatueTrapModel_hpp
#define MPStatueTrapModel_hpp

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


class StatueTrap : public TrapModel{

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


#endif /* MPStatueTrapModel_hpp */
