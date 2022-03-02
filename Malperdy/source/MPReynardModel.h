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

using namespace cugl;

class ReynardModel : CharacterModel {

    
public:

    /**
     * {@note by Barry Feb 26 I need a constructor}
     * @return a Reynard.
     *
    ReynardModel(Vec2 pos) {
    }

    //{@note by Barry on Feb 26: feature request: alloc() which returns shared ptr so I dont gave to make_shared in Gamescene}

protected:
    cugl::Vec2 _force;;

    /** Vec2 representing speed + direction */ 
    cugl::Vec2 _velocity;
    /** Vec2 representing position */ 
    cugl::Vec2 _position;
    /** Class representing all states Reynard can be in */ 
    enum class ReynardState : int{
        SPAWN,
        MOVING,
        SLOWMOVING, 
        DASHING,
        JUMPING,
        TRAPPED
    };
    /** This ReynardState represents the current state of Reynard at any given moment */
    ReynardState _currentState;
    /** Bool representing if Reynard is currently jumping */ 
    bool _isJumping; 
    /** How long until we can jump again */
    int  _jumpCooldown;
    /** Bool representing if Reyanrd is current dashing */
    bool _isDashing;
    /** How long until we can dash again */
    int  _dashCooldown;
    /** The scene graph node for Reynard. */
    std::shared_ptr<cugl::scene2::SceneNode> _reyardNode;


public:
#pragma mark Accessors
    /**
     * Returns the velocity vector for Reynard 
     *
     * @return the velocity vector applied to Reynard
     */
    const cugl::Vec2 &getVelocity() const { return _velocity; }

    /**
     * Sets the velocity vector for Reynard 
     *
     * @param value  the velocity vector to apply to Reynard
     */
    void setVelocity(const cugl::Vec2 &value) { _velocity.set(value); }

    /**
     * Returns the x-component of the velocity applied to Reynard.
     *
     * @return the x-component of the velocity applied to Reynard.
     */
    float getVelocityX() const { return _velocity.x; }

    /**
     * Returns the x-component of the velocity applied to Reynard.
     *
     * @return the x-component of the velocity applied to Reynard.
     */
    float getVelocityY() const { return _velocity.y; }

    /**
     * Sets the position vector for Reynard 
     *
     * @param value  the position vector to apply to Reynard
     */
    void setPosition(const cugl::Vec2 &value) { _position.set(value); }


    /**
     * Returns the x-component of the position applied to Reynard.
     *
     * @return the x-component of the position applied to Reynard.
     */
    float getPositionX() const { return _position.x; }

    /**
     * Returns the x-component of the position applied to Reynard.
     *
     * @return the x-component of the position applied to Reynard.
     */
    float getPositionY() const { return _position.y; }
    
    /**
     * This function will return the current state of our boy Reynard.
     *
     * @return the enum Reynard State of the instantiated reynard model.
     */
    ReynardState getCurrentState() const { return _currentState; }
    
    /**
     * This function allows you to set the current state of Reynard .
     *
     * @return the enum Reynard State of the instantiated reynard model.
     */
    void setCurrentState(const ReynardState cS) { _currentState = cS; }

    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    bool isJumping() const { return _isJumping && _jumpCooldown <= 0; }
    
    /**
     * Sets whether the dude is actively jumping.
     *
     * @param value whether the dude is actively jumping.
     */
    void setJumping(bool value) { _isJumping = value; }

    /**
     */
    bool getIsDashing() const { return _isDashing; }

     /**
     */
    void setDashing(bool value) { _isDashing = value; }

#pragma mark Constructors
    bool init(const Vec2 pos);

     static std::shared_ptr<ReynardModel> alloc(const Vec2 pos) {
        std::shared_ptr<ReynardModel> result = std::make_shared<ReynardModel>();
        return (result->init(pos) ? result : nullptr);
    }

#pragma mark -
#pragma mark Physics
 
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
    virtual void update(float delta) override;
    
    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();

    
    
    };


#endif /* MPReynardModel_h */
