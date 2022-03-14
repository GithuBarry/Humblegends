//
//  MPTrapModel.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/7/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPTrapModel_hpp
#define MPTrapModel_hpp

#include <stdio.h>
#include <cugl/cugl.h>

class TrapModel : public cugl::physics2::BoxObstacle{


public:
    enum class TrapState : int{
        ACTIVATED,
        DEACTIVATED
    };
    
#pragma mark Destructors
    /**
     * Destroys this room, releasing all resources.
     */
    virtual ~TrapModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this room.
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a room may not be used until it is initialized again.
     */
    void dispose();

private:
    TrapState _trapState;

};


#endif /* MPTrapModel_hpp */
