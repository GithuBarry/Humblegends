//
//  MPReynardController.h
//  Malperdy
//
//  Owner: Abu Qader
//  Contributors: Spencer Hurst, Kristina Gu
//  Version: 2/25/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef __MP_REYNARD_CONTROLLER_H__
#define __MP_REYNARD_CONTROLLER_H__

#include <cugl/cugl.h>

#include "MPCharacterController.h"
#include "MPReynardModel.h"

/** How many frames' worth of "scent trail" locations Reynard should store. The longer
this is, the further away enemies have to be before Reynard loses them */
#define TRAIL_LENGTH 60

class ReynardController : public CharacterController<ReynardModel, ReynardController> {

private:

    /**
     * Position difference since last update
     */
    Vec2 _delta;

    /**
     * Reynard's position during last frame, updated at the end of update()
     */
    Vec2 _lastPosition = Vec2();

    /**
     * Whether last position and _delta are valid.
     */
    bool _lastPositionValid = false;

    /** Reynard's location in world space over the last TRAIL_LENGTH frames (queue) */
    shared_ptr<deque<Vec2>> _trail = make_shared<deque<Vec2>>();

public:

    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);

    /**
     * @return how much movement there has been since last frame as a vec2
     */
    Vec2 getMovementSinceLastFrame() {
        return _delta;
    }
};


#endif //__MP_REYNARD_CONTROLLER_H__