//
// Created by Kristina Gu on 3/8/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef __MP_CHARACTER_CONTROLLER_H__
#define __MP_CHARACTER_CONTROLLER_H__

#include <cugl/cugl.h>
#include "MPCharacterModel.h"

class CharacterController {

private:
    /** The model storing this character's data */
    shared_ptr<CharacterModel> _character;

public:

#pragma mark -
#pragma mark Hidden Constructors

    /**
     * Creates a degenerate CharacterController object.
     *
     * This constructor does not initialize any of the character values beyond
     * the defaults. To use a CharacterController, you must call init().
     */
    CharacterController() {};

    /**
     * Initializes a new controller for the character at the given position.
     *
     * The character is sized according to the given drawing scale.
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
     * @return  true if the character is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float drawScale);

#pragma mark -
#pragma mark Static Constructors

    /**
     * Creates a new controller for the character at the given position.
     *
     * The character has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the character in world units
     *
     * @return  A newly allocated CharacterController for the character at the given position with the given scale
     */
    static std::shared_ptr<CharacterController> alloc(const cugl::Vec2& pos, const cugl::Size& size, float drawScale) {
        std::shared_ptr<CharacterController> result = std::make_shared<CharacterController>();
        return (result->init(pos, size, drawScale) ? result : nullptr);
    }

#pragma mark -
#pragma mark Attribute Methods
    /**This allows someone to grab the instantiated CharacterModel from this controller**/
    virtual shared_ptr<CharacterModel> getCharacter(){ return _character; }

    /**
     * [update] This will automatically update the character's position and look at other
     * things in the future like health.
     */
    void update(float delta);

#pragma mark -
#pragma mark Actions

    /**
     * Turns the character around to face the opposite direction.
     * 
     * @return  Whether the character turned successfully.
     */
    bool turn();

    /**
     * Stops the character from moving.
     * 
     * @return  Whether the character stopped moving successfully
     */
    bool stop();
    
    /**
     * Starts the character running in the direction they're facing.
     *
     * @return  Whether the character started running successfully
     */
    bool run();

    /**
     * The character jumps upwards at a set velocity.
     *
     * @return  Whether the character jumped successfully
     */
    bool jump();
    
    /**
     * Sets the character to be falling through the air.
     *
     * @return  Whether the character was set to be falling successfully
     */
    bool fall();

    /**
     * Sets the character to be sliding down a wall for a short amount of time.
     *
     * @return  Whether the character is successfully stuck to the wall
     */
    bool stickToWall();

    /**
     * If the character is sliding down a wall already, unsticks them and drops
     * them to the ground.
     *
     * @return  Whether the character is successfully unstuck from the wall
     */
    bool unstickFromWall();

    /**
     * Sets the character to be on the ground.
     *
     * @return  Whether the character is successfully marked as being on the ground
     */
    bool land();

};


#endif //MALPERDY_MPREYNARDCONTROLLER_H
