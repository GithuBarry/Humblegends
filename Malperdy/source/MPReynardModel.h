//
//  PFDudeModel.h
//  PlatformDemo
//
//  This encapsulates all of the information for the character avatar.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  Pay close attention to how this class designed.  Subclasses of Cocos2d classes
//  (which are all subclasses of the class Ref) should never have normal public
//  constructors.  Instead, you should organize their constructors into three parts,
//  just like we have done in this class.
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#ifndef __MP_REYNARD_MODEL_H__
#define __MP_REYNARD_MODEL_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define DUDE_TEXTURE    "rocket"
/** Identifier to allow us to track the sensor in ContactListener */
#define SENSOR_NAME     "reynardsensor"


#pragma mark -
#pragma mark Physics Constants
/** The factor to multiply by the input */
#define DUDE_FORCE      20.0f
/** The amount to slow the character down */
#define DUDE_DAMPING    10.0f
/** The maximum character speed */
#define DUDE_MAXSPEED   5.0f

#pragma Movement Constants
/** The rate at which Reynard accelerates */
#define REYNARD_ACC 5000.0f
/** The maximum speed at which Reynard can run */
#define REYNARD_MAX_SPEED 120.0f

using namespace cugl;

#pragma mark -
#pragma mark Reynard Model

/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/
class ReynardModel : public cugl::physics2::BoxObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */


protected:
    

    /** The current horizontal movement of the character */
    float _movement;
    /** Vec2 representing position */
    cugl::Vec2 _position;
    /** Which direction is the character facing */
    bool _faceRight;
    /** How long until we can jump again */
    int _jumpCooldown;
    /** Whether we are actively jumping */
    bool _isJumping;
    /** Bool representing if Reyanrd is current dashing */
    bool _isDashing;
    /** How long until we can dash again */
    int  _dashCooldown;
    /** How long until we can shoot again */
    bool _isGrounded;
    /** Ground sensor to represent our feet */
    b2Fixture *_sensorFixture;
    /** Reference to the sensor name (since a constant cannot have a pointer) */
    std::string _sensorName;
    /** The node for debugging the sensor */
    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;

    /** The scene graph node for the Dude. */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;


    /**
    * Redraws the outline of the physics fixtures to the debug node
    *
    * The debug node is use to outline the fixtures attached to this object.
    * This is very useful when the fixtures have a very different shape than
    * the texture (e.g. a circular shape attached to a square texture).
    */
    virtual void resetDebug() override;

public:
    
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


#pragma mark Hidden Constructors

    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a DudeModel, you must call init().
     */
    ReynardModel() : BoxObstacle(), _sensorName(SENSOR_NAME) { }

    /**
     * Destroys this DudeModel, releasing all resources.
     */
    virtual ~ReynardModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this DudeModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a DudeModel may not be used until it is initialized again.
     */
    void dispose();

    /**
     * Initializes a new dude at the given position.
     *
     * The dude is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 &pos, const cugl::Size &size, float drawScale);


#pragma mark -
#pragma mark Static Constructors

    /**
     * Creates a new dude at the given position.
     *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
      * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  A newly allocated DudeModel at the given position with the given scale
     */
    static std::shared_ptr<ReynardModel> alloc(const cugl::Vec2 &pos, const cugl::Size &size, float drawScale) {
        std::shared_ptr<ReynardModel> result = std::make_shared<ReynardModel>();
        return (result->init(pos, size, drawScale) ? result : nullptr);
    }


#pragma mark -
#pragma mark Animation

    /**
     * Returns the scene graph node representing this DudeModel.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this DudeModel.
     */
    const std::shared_ptr<cugl::scene2::SceneNode> &getSceneNode() const {
        return _node;
    }

    /**
     * Sets the scene graph node representing this DudeModel.
     *
     * Note that this method also handles creating the nodes for the body parts
     * of this DudeModel. Since the obstacles are decoupled from the scene graph,
     * initialization (which creates the obstacles) occurs prior to the call to
     * this method. Therefore, to be drawn to the screen, the nodes of the attached
     * bodies must be added here.
     *
     * The bubbler also uses the world node when adding bubbles to the scene, so
     * the input node must be added to the world BEFORE this method is called.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this DudeModel, which has been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode> &node) {
        _node = node;
        _node->setPosition(getPosition() * _drawScale);
    }


#pragma mark -
#pragma mark Attribute Properties

    /**
     * Returns left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @return left/right movement of this character. (SHOULD ALWAYS BE 1, -1)
     */
    float getMovement() const { return _movement; }

    /**
     * Sets left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @param value left/right movement of this character.
     */
    void setMovement(float value);

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
    bool isJumping() const {
        return _isJumping && _jumpCooldown <= 0;
    }

    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    bool isDashing() const { return _isDashing && _dashCooldown <= 0; }
    
    /**
     * Sets whether the dude is actively jumping.
     *
     * @param value whether the dude is actively jumping.
     */
    void setJumping(bool value) { _isJumping = value; }

    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    float getCooldownJump() const { return _jumpCooldown; }
    
    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    float getCooldownDash() const { return _dashCooldown; }

    /**
     * Returns true if the dude is on the ground.
     *
     * @return true if the dude is on the ground.
     */
    bool isGrounded() const { return _isGrounded; }

    /**
     * Sets whether the dude is on the ground.
     *
     * @param value whether the dude is on the ground.
     */
    void setGrounded(bool value) { _isGrounded = value; }

    /**
     * Returns how much force to apply to get the dude moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the dude moving
     */
    float getForce() const { return DUDE_FORCE; }

    /**
     * Returns ow hard the brakes are applied to get a dude to stop moving
     *
     * @return ow hard the brakes are applied to get a dude to stop moving
     */
    float getDamping() const { return DUDE_DAMPING; }

    /**
     * Returns the upper limit on dude left-right movement.
     *
     * This does NOT apply to vertical movement.
     *
     * @return the upper limit on dude left-right movement.
     */
    float getMaxSpeed() const { return DUDE_MAXSPEED; }

    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string* getSensorName() { return &_sensorName; }

    /**
     * Returns true if this character is facing right
     *
     * @return true if this character is facing right
     */
    bool isFacingRight() const {
        return _faceRight;
    }


    /**
     * Sets the current position for this physics body
     *
     * This method converts from a CUGL vector type to a Box2D vector type. This
     * cuts down on the confusion between vector types.
     *
     * @param value  the current position for this physics body
     */
    virtual void setPosition(const Vec2 value) override {
        CULog("CALLED");
        SimpleObstacle::setPosition(value);
        _node->setPosition(value * _drawScale);
    }


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
    void update(float dt) override;

    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce();

//    /**
//     * Applies the jump force to the body of Reynard
//     *
//     * This method should be called after the force attribute is set.
//     */
//    bool applyJumpForce();
//
//    /**
//     * Applies the jump force to the body of Reynard
//     *
//     * This method should be called after the force attribute is set.
//     */
//    bool applyDashForce();


};

#endif /* __PF_REYNARD_MODEL_H__ */
