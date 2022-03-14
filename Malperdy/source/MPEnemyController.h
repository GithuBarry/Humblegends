//
//  MPEnemyController.h
//  Malperdy
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 3/10/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef __MP_ENEMY_CONTROLLER_H__
#define __MP_ENEMY_CONTROLLER_H__

#include <cugl/cugl.h>
#include "MPCharacterController.h"
#include "MPEnemyModel.h"
#include "MPReynardController.h"

class EnemyController : public CharacterController<EnemyModel, EnemyController> {

private:

    /** Pointer to Reynard's controller just for convenience */
    shared_ptr<ReynardController> _reynard;

public:
    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);

    void setReynard(shared_ptr<ReynardController> reynard) { _reynard = reynard; }
};


#endif //__MP_ENEMY_CONTROLLER_H__