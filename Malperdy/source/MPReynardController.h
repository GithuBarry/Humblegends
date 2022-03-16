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

class ReynardController : public CharacterController<ReynardModel, ReynardController> {

public:

    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);

    /**
     * Applies a knockback force to the character in the direction of the
     * given vector.
     *
     * @param dir   Direction to apply knockback force in
     */
    void knockback(b2Vec2 dir);
};


#endif //__MP_REYNARD_CONTROLLER_H__