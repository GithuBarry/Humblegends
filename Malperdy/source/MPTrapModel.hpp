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

#pragma mark -
#pragma mark Size Constants

using namespace cugl;


class TrapModel : public cugl::scene2::SceneNode {


public:
    
    /** The potential activation states of the trap. */
    enum class TrapState : int{
        ACTIVATED,
        DEACTIVATED
    };
    
    /** The current activation state of the trap. */
    TrapState _trapState;
    
    
    /** SceneNode representing the sprite for the trap */
    shared_ptr<scene2::SceneNode> _node;
    
    /** SceneNode representing the sprite for the trap */
    cugl::physics2::BoxObstacle _boxObstacle;

protected:
#pragma mark -
#pragma mark Constants

    /** The texture for the TRAP   */
    const string TRAP_TEXTURE;

    
#pragma mark Attributes

    /** A uniform value represneting scale between the physics world and the screen */
    float _drawScale;
    /** Vec2 representing position of the trap within the room */
    cugl::Vec2 _position;
    

    /**
    * Redraws the outline of the physics fixtures to the debug node
    *
    * The debug node is use to outline the fixtures attached to this object.
    * This is very useful when the fixtures have a very different shape than
    * the texture (e.g. a circular shape attached to a square texture).
    */
    virtual void resetDebug();

public:
#pragma mark -
#pragma mark Hidden Constructors
    
    /**
     * Initializes a trap with the given characteristics in a given location in Room Space.
     *
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x         The x-coordinate of the trap in room space
     * @param y         The y-coordinate of the room in room space
     *
     * @return     Returns True if the space is initialized properly.
     */
    bool init(float x, float y);
    //TODO: This needs to be verified to be in the room space coords and not world space coords
    

    
    
    
#pragma mark -
#pragma mark Attribute Properties

    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @return      Whether the change happened successfully
     */
    cugl::physics2::BoxObstacle getPhysicalBody(){
        return _boxObstacle;
    }
    
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @param x           Float representing x Position
     * @param y           Float representing y Position
     */
    void setPhysicalBodyPos(float x, float y){
        _boxObstacle.setPosition(x, y);
    }
    
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @param pos           Vec2 with pos.x representing x location and pos.y representing y
     */
    void setPhysicalBodyPos(Vec2 pos){
        _boxObstacle.setPosition(pos);
    }
    
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
    
        
    
#pragma mark -
#pragma mark Physics Methods

    
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    virtual void createFixtures();

    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    virtual void releaseFixtures();

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    virtual void update(float dt);

    
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
