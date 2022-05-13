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

using namespace cugl;

#pragma mark -
#pragma mark Falling Trap


class FallingTrap : public TrapModel {

private:
    bool fallen;
    
public:

#pragma mark Constructor

    FallingTrap() : TrapModel() { }
    
    virtual ~FallingTrap(void) { dispose(); }
    
    void dispose();
    
    static std::shared_ptr<FallingTrap> alloc(const cugl::Vec2& pos) {
        std::shared_ptr<FallingTrap> result = std::make_shared<FallingTrap>();
        return (result->init(pos) ? result : nullptr);
    }
    
    virtual bool init(const cugl::Vec2 &pos);
    
    bool init(float roomWidth, float roomHeight);

    void update(const shared_ptr<ReynardController>& reynard, float dt);

    void fall();
};


#endif /* MPFallingTrap_hpp */
