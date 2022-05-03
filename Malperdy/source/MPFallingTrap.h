//
//  MPFallingTrap.hpp
//  Malperdy
//
//  Created by Abu Qader on 5/1/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPFallingTrap_hpp
#define MPFallingTrap_hpp

#include <stdio.h>

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPTrapModel.hpp"
#include "MPRoomModel.h"
#include "MPReynardController.h"


#include <stdio.h>
using namespace cugl;

#pragma mark -
#pragma mark Falling Trap


class FallingTrap : public TrapModel{

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

    void update(const shared_ptr<ReynardController>& reynard);

    void fall();
};


#endif /* MPFallingTrap_hpp */
