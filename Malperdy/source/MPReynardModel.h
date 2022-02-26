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
    cugl::Vec2 _force;;
public:

    /**
     * {@note by Barry Feb 26 I need a constructor}
     * @return a Reynard.
     *
    ReynardModel(Vec2 pos) {
    }

    //{@note by Barry on Feb 26: feature request: alloc() which returns shared ptr so I dont gave to make_shared in Gamescene}


    /**
     * Returns the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @return the force applied to this rocket.
     */
    const cugl::Vec2 &getForce() const {
        return _force;
    }

    /**
     * Sets the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @param value  the force applied to this rocket.
     */
    void setForce(const cugl::Vec2 &value) {
        _force.set(value);
    }

    /**
     * Returns the x-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @return the x-component of the force applied to this rocket.
     */
    float getFX() const {
        return _force.x;
    }

    /**
     * Sets the x-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @param value the x-component of the force applied to this rocket.
     */
    void setFX(float value) {
        _force.x = value;
    }

    /**
     * Returns the y-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @return the y-component of the force applied to this rocket.
     */
    float getFY() const {
        return _force.y;
    }


    /**
     * Sets the x-component of the force applied to this rocket.
     *
     * Remember to modify the input values by the thrust amount before assigning
     * the value to force.
     *
     * @param value the x-component of the force applied to this rocket.
     */
    void setFY(float value) {
        _force.y = value;
    }
};

#endif /* MPReynardModel_h */
