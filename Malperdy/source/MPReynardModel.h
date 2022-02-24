//
//  MPReynardModel.h
//  Malperdy

//  Our constantly moving main character
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPReynardModel_h
#define MPReynardModel_h

#include <cugl/cugl.h>
#include "MPCharacterModel.h"

class ReynardModel : CharacterModel {
    /* Vec2 representing speed + direction */ 
    cugl::Vec2 velocity;
    /* Vec2 representing position */ 
    cugl::Vec2 position;

    /**
     * Returns the velocity vector for Reynard 
     *
     * @return the velocity vector applied to Reynard
     */
    const cugl::Vec2 &getVelocity() const {
        return velocity;
    }

    /**
     * Sets the velocity vector for Reynard 
     *
     * @param value  the velocity vector to apply to Reynard
     */
    void setVelocity(const cugl::Vec2 &value) {
        velocity.set(value);
    }


    /**
     * Returns the x-component of the velocity applied to Reynard.
     *
     * @return the x-component of the velocity applied to Reynard.
     */
    float getVelocityX() const {
        return velocity.x;
    }

    /**
     * Returns the x-component of the velocity applied to Reynard.
     *
     * @return the x-component of the velocity applied to Reynard.
     */
    float getVelocityY() const {
        return velocity.y;
    }


    /**
     * Sets the position vector for Reynard 
     *
     * @param value  the position vector to apply to Reynard
     */
    void setPosition(const cugl::Vec2 &value) {
        position.set(value);
    }


    /**
     * Returns the x-component of the position applied to Reynard.
     *
     * @return the x-component of the position applied to Reynard.
     */
    float getPositionX() const {
        return position.x;
    }

    /**
     * Returns the x-component of the position applied to Reynard.
     *
     * @return the x-component of the position applied to Reynard.
     */
    float getPositionY() const {
        return position.y;
    }

};

#endif /* MPReynardModel_h */
