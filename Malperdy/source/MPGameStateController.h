//
//  MPGameStateController.h
//  Malperdy
//
//  Keep track of whether the mode is zoomed in or zoomed out.
//  Also keep track of health/other resources
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPGameStateController_h
#define MPGameStateController_h
#include <cugl/cugl.h>
class GameStateController{
public:
    /**
     * Change parameter as you need
     */
    GameStateController() {
    }

    void zoom_in();

    void zoom_out();

    /**
     * @param dt the actual time past
     * @return the time for the physics simulation, based on the state of the game
     */
    float getScaledDtForPhysics(float dt);

};
#endif /* MPGameStateController_h */
