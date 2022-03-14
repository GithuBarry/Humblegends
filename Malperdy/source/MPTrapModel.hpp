//
//  MPTrapModel.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/7/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//
//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

#ifndef MPTrapModel_hpp
#define MPTrapModel_hpp

#include <stdio.h>
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#pragma mark -
#pragma mark Size Constants

using namespace cugl;


class TrapModel : public cugl::physics2::BoxObstacle {


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
#pragma mark Hidden Constructors
    
    /**
     * Creates a general Trap object.
     *
     * The constructor will not initialize any of the character values beyond
     * the defaults. To create a TrapModel, you must call init().
     */
    TrapModel () : BoxObstacle(){}
    
    /**
     *
     * Initialize a general character model.
     *
     * @param pos       Initial position in world coordinates as a Vec2
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for the trap's appearance
     *
     * @return  This function will return true if initialized properly.
     */
    virtual bool init(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> image);
    
    
#pragma mark -
#pragma mark Static Constructors
    
    /**
     * Create a given trap in a given position.
     *
     * The trap has a given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     *
     * @param pos                   Initial position in World Coords
     * @param drawScale     The draw scale (world to screen)
     * @param image               The image for the trap's appearance
     */
    static std::shared_ptr<TrapModel> alloc(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> image) {
        std::shared_ptr<TrapModel> result = std::make_shared<TrapModel>();
        return (result->init(pos, drawScale, image) ? result : nullptr);
    }
    
    
    
    
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
    virtual void createFixtures() override;

    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    virtual void releaseFixtures() override;

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
