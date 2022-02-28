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
        MOVING,
        /** Form of moving when game is zoomed out */
        SLOWMOVING, 
        DASHING,
        JUMPING
    };
    /** Bool representing if Reynard is currently jumping */ 
    bool _isJumping; 
    /** How long till Reynard can hump again */ 
    float _jumpCooldown;
    /** Bool representing if Reyanrd is current dashing */ 
    bool _isDashing; 
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
     */
    bool getIsJumping() const { return _isJumping; }

    /**
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
     * Applies jump action to Reynard
     */
    void applyJump();

    /**
     * Applies dash action to Reynard
     */
    void applyDash();

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * This method is called AFTER the collision resolution state. Therefore, it
     * should not be used to process actions or any other gameplay information.
     * Its primary purpose is to adjust changes to the fixture, which have to
     * take place after collision.
     *
     * In other words, this is the method that updates the scene graph.  If you
     * forget to call it, it will not draw your changes.
     *
     * @param delta Timing values from parent loop
     */
    virtual void update(float delta) override;
    };


#endif /* MPReynardModel_h */
