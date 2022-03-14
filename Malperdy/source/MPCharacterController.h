//
// Created by Kristina Gu on 3/8/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef __MP_CHARACTER_CONTROLLER_H__
#define __MP_CHARACTER_CONTROLLER_H__

#include <cugl/cugl.h>
#include "MPCharacterModel.h"

// Type of the model that this character controller holds
template<class ModelType, class ControllerType>
class CharacterController {
protected:
    /** The model storing this character's data */
    shared_ptr<ModelType> _character;

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
     * @param pos       Initial position in world coordinates
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for the character's appearance
     *
     * @return  true if the character is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> image) {
        // Get model cast to subclass type
        _character = make_shared<ModelType>();
        _character->init(pos, drawScale, image);
        return (_character != nullptr);
    }

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
     * @param pos       Initial position in world coordinates
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for the character's appearance
     * 
     * @return  A newly allocated CharacterController for the character at the given position with the given scale
     */
    static shared_ptr<ControllerType> alloc(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> image) {
        std::shared_ptr<ControllerType> result = std::make_shared<ControllerType>();
        return (result->init(pos, drawScale, image) ? result : nullptr);
    }

#pragma mark -
#pragma mark Attribute Methods
    /**This allows someone to grab the instantiated CharacterModel from this controller**/
    shared_ptr<ModelType> getCharacter() { return _character; }

    /**
     * Returns the position of the character's center.
     * 
     * @return  Position of character's center
     */
    Vec2 getPosition() {
        return _character->getPosition();
    }

#pragma mark -
#pragma mark Animation

    /**
     * Returns the scene graph node representing the CharacterModel owned
     * by this controller.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing the owned CharacterModel.
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const {
        return _character->_node;
    }

    /**
     * Sets the scene graph node representing this CharacterModel.
     *
     * Note that this method also handles creating the nodes for the body parts
     * of this CharacterModel. Since the obstacles are decoupled from the scene graph,
     * initialization (which creates the obstacles) occurs prior to the call to
     * this method. Therefore, to be drawn to the screen, the nodes of the attached
     * bodies must be added here.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this CharacterModel, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) {
        _character->setSceneNode(node);
    }

#pragma mark -
#pragma mark Actions

    /**
     * Turns the character around to face the opposite direction.
     *
     * @return  Whether the character turned successfully.
     */
    bool turn() {
        // Don't allow turning if character is falling
        //if (!(_character->isFalling())) return false;

        _character->flipDirection();
        return true;

    }

    /**
     * The character jumps upwards at a set velocity.
     *
     * @return  Whether the character jumped successfully
     */
    bool jump() {
        return _character->setMoveState(CharacterModel::MovementState::JUMPING);
    }

    /**
     * Stops the character from moving.
     *
     * @return  Whether the character stopped moving successfully
     */
    bool stop() {
        return _character->setMoveState(CharacterModel::MovementState::STOPPED);
    }

    /**
     * Starts the character running in the direction they're facing.
     *
     * @return  Whether the character started running successfully
     */
    bool run() {
        return _character->setMoveState(CharacterModel::MovementState::RUNNING);
    }

    /**
     * Sets the character to be falling through the air.
     *
     * @return  Whether the character was set to be falling successfully
     */
    bool fall() {
        return _character->setMoveState(CharacterModel::MovementState::FALLING);
    }

    /**
     * Sets the character to be sliding down a wall for a short amount of time.
     *
     * @return  Whether the character is successfully stuck to the wall
     */
    bool stickToWall() {
        return _character->setMoveState(CharacterModel::MovementState::ONWALL);
    }

    /**
     * If the character is sliding down a wall already, unsticks them and drops
     * them to the ground.
     *
     * @return  Whether the character is successfully unstuck from the wall
     */
    bool unstickFromWall() {
        return _character->setMoveState(CharacterModel::MovementState::FALLING);
    }

    /**
     * Sets the character to be on the ground.
     *
     * @return  Whether the character is successfully marked as being on the ground
     */
    bool land() {
        return _character->setMoveState(CharacterModel::MovementState::RUNNING);
    }

#pragma mark -
#pragma mark Update

    /** 
     * This method handles anything about the character that needs to change over time.
     * 
     * @param delta The amount of time that has passed since the last frame
     */
    virtual void update(float delta) {
        // Update model
        _character->update(delta);
    }

};


#endif //__MP_CHARACTER_CONTROLLER_H__