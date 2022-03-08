//
// Created by Spencer Hurst on 2/25/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MALPERDY_MPREYNARDCONTROLLER_H
#define MALPERDY_MPREYNARDCONTROLLER_H

#define MOVEMENT_FORCE 5.0F


#include <cugl/cugl.h>
#include "MPCharacterController.h"
#include "MPReynardModel.h"

class ReynardController : public CharacterController {

private:
    shared_ptr<ReynardModel> _reynard;
    float _currentMovement = MOVEMENT_FORCE;

public:
    /**
     * Initializes a new controller for Reynard at the given position.
     *
     * Reynard is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the character in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  true if Reynard is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2& pos, const cugl::Size& size, float drawScale);

    /**This allows someone to grab the instantiated CharacterModel from this controller**/
    virtual shared_ptr<ReynardModel> getReynard() { return _reynard; }

    // TODO: use templates
    static std::shared_ptr<ReynardController> allocReynard(const cugl::Vec2& pos, const cugl::Size& size, float drawScale) {
        std::shared_ptr<ReynardController> result = std::make_shared<ReynardController>();
        return (result->init(pos, size, drawScale) ? result : nullptr);
    }
};


#endif //MALPERDY_MPREYNARDCONTROLLER_H
