//
//  MPGameStateController.h
//  Malperdy
//
//  Keep track of whether the mode is zoomed in or zoomed out.
//  Also keep track of health/other resources
//
//  Owner: TBD
//  Contributors: Barry Wang
//  Version: 2/21/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPGameStateController_h
#define MPGameStateController_h

#include <cugl/cugl.h>

class GameStateController {
private:
    bool _zoomed_in = false;
    int SLOW_MO_SCALAR = 30;
public:
    /**
     * Change parameter as you need
     */
    GameStateController() {};


    /**
     Return whether the game is currently zoomed in
     */
    bool zoomed_in() {
        return _zoomed_in;
    }

    /**
     Set the game state to be [zoomed in]
     */
    void zoom_in() {
        _zoomed_in = true;
    }

    /**
     Set the game state to be [zoomed out]
     */
    void zoom_out() {
        _zoomed_in = false;
    }

    /**
        Switch the game state
     */
    void zoom_switch() {
        _zoomed_in = !_zoomed_in;
    }

    /**
     * @param dt the actual time past
     * @return the time for the physics simulation, based on the state of the game
     */
    float getScaledDtForPhysics(float dt) {
        if (_zoomed_in) {
            return dt;
        } else {
            return dt / SLOW_MO_SCALAR;
        }
    };

};

#endif /* MPGameStateController_h */
