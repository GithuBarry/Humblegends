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

class EnemyController : public CharacterController<EnemyModel, EnemyController> {

private:
    /** The last location at which this enemy saw Reynard */
    Vec2 _lastKnownReynardLocation;

public:
    /**
     * This method handles anything about the character that needs to change over time.
     *
     * @param delta The amount of time that has passed since the last frame
     */
    void update(float delta);
};


#endif //__MP_ENEMY_CONTROLLER_H__