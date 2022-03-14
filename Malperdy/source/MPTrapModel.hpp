//
//  MPTrapModel.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/7/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPTrapModel_hpp
#define MPTrapModel_hpp

#include <stdio.h>
#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#pragma mark -
#pragma mark Size Constants

using namespace cugl;


class TrapModel : public cugl::physics2::BoxObstacle{


public:
    enum class TrapState : int{
        ACTIVATED,
        DEACTIVATED
    };
    
    
    /** SceneNode representing the sprite for the trap */
    shared_ptr<scene2::SceneNode> _node;

protected:
#pragma mark -
#pragma mark Constants

    /** The texture for the character avatar */
    const string CHARACTER_TEXTURE;

    
#pragma mark Attributes

    /** A uniform value represneting scale between the physics world and the screen */
    float _drawScale;
    /** Vec2 representing position of the trap within the room */
    cugl::Vec2 _position;
    
    /** The current movement state of the character. */
    TrapState _trapState;

    /**
    * Redraws the outline of the physics fixtures to the debug node
    *
    * The debug node is use to outline the fixtures attached to this object.
    * This is very useful when the fixtures have a very different shape than
    * the texture (e.g. a circular shape attached to a square texture).
    */
    virtual void resetDebug() override;

public:
    
#pragma mark -
#pragma mark Attribute Properties

    /**
     * Returns true if the trap is currently activated.
     *
     * @return true if the trap is currently active and can kill enemies and Reynard.
     */
    bool isActivated() const {
        return (_trapState == TrapState::ACTIVATED);
    }
    
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @param state The new state the trap should be in
     * @return      Whether the change happened successfully
     */
    bool setTrapState(TrapState newState);

    //TODO: figure out what would be needed by creating a setPosition function
    //TODO: And the difference between a cugl vector type and a box2d vector type.
    
    
#pragma mark -
#pragma mark Physics Methods

//    TODO: are funcitons like this really needed in my code
//    TODO: Think on this a bit more spencer
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    void createFixtures() override;

    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    virtual void update(float dt) override;

    
#pragma mark Destructors
    /**
     * Destroys this room, releasing all resources.
     */
    virtual ~TrapModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this room.
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a room may not be used until it is initialized again.
     */
    void dispose();


};


#endif /* MPTrapModel_hpp */
