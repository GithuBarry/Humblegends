//
//  MPCharacterModel.h
//  Malperdy
//
//  This is a superclass for the model that will hold data for a character,
//  which includes enemies and Reynard. The model will simply hold data
//  about the character, and all changes will be done using the controller.
//
//  This class should only be instantiated by a controller.
//
//  Created by Kristina Gu on 3/8/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef __MP_CHARACTER_MODEL_H__
#define __MP_CHARACTER_MODEL_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include <map>
#include "MPAnimation.h"

using namespace cugl;

#pragma mark -
#pragma mark Drawing Constants
/** Identifier to allow us to track the sensor in ContactListener */
#define SENSOR_NAME     "reynardsensor"

#pragma mark Physics Constants
/** The amount to reduce gravity by when the character is sliding down a wall */
// TODO: fix this to use friction instead of gravity
#define WALL_SLIDE_GRAV_SCALE 0.3f

#pragma Movement Constants
/** The default speed at which this character runs */
#define RUN_SPEED 4.0f
/** The default speed at which this character runs */
#define DELAY_SPEED 1.0f
/** The speed at which this character jumps */
#define JUMP_SPEED_X 7.0f
#define JUMP_SPEED_Y 12.8f
/** By how much to decelerate Reynard's movement by */
#define DECELERATION 0.3f
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Amount of time (in seconds) for wall slide duration */
#define WALL_SLIDE_DURATION 1.5f
/** Any character's width*/
#define DUDE_WIDTH  1.0f
/** The amount to shrink the sensor fixture (horizontally) relative to the image */
#define DUDE_SSHRINK  0.3f
/** Height of the sensor attached to the player's feet */
#define SENSOR_HEIGHT   0.1f
/** The density of the character */
#define DUDE_DENSITY    1.0f
/** Debug color for the sensor */
#define DEBUG_COLOR     Color4::RED
/** The multiplier for the character dash */
#define DASH_MULTIPLIER 5.2f

class CharacterModel : public cugl::physics2::CapsuleObstacle {
private:
#pragma mark Constants

    /** The texture for the character avatar */
    const string CHARACTER_TEXTURE;

    /** The amount of time in between each frame update */
    const float FRAME_TIME = 0.03;

    /** The duration in milliseconds of a dash */
    const Uint64 DASH_DURATION = 120;
    const Uint64 DASH_COOLDOWN = 900;

public:
    /** Enum representing the current state of movement that the character is in */
    enum class MovementState : int {
        STOPPED,
        RUNNING,
        JUMPING,
        FALLING,
        ONWALL,
        DASHING,
        DEAD
    };


    /** SceneNode representing the sprite for the character */
    shared_ptr<scene2::SpriteNode> _node;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;

    /** The character's current number of hearts */
    float _hearts;

    /** The number of times the character has hit the ground, as they're only off when this is <= 0 */
    int _groundedCounter = 0;

protected:
    /** The current maximum number of hearts that this character can have */
    float _maxHearts = 2;

    /** The amount of time since last frame update */
    float _elapsed = 0;

    /** represents the actual frame of animation, invariant to texture flips */
    int _currFrame = 0;

    /** the time that the last dash started */
    Timestamp _dashStart = Timestamp();
    
    /** whether the character jumped since leaving the ground */
    bool _jumped = false;

    /** Whether the character was on the ground when they dashed */
    bool _dashedOnGround = false;

#pragma mark -
#pragma mark Attributes

    /** The dictionary of all character animations */
    shared_ptr<Animation> _animation;

    /** The frame data the current animation */
    string _currAnimation;
    int _startframe;
    int _lastframe;
    bool _loop;
    bool _flip;

#pragma mark Attributes


    /** Which direction is the character facing */
    bool _faceRight = true;
    /** The current movement state of the character. */
    MovementState _moveState;
    /** Ground sensor to represent our feet */
    b2Fixture *_feetFixture;
    b2Fixture *_faceFixtureLeft;
    b2Fixture *_faceFixtureRight;
    /** Reference to the sensor name (since a constant cannot have a pointer) */
    std::string _sensorName;
    /** The node for debugging the sensor */
    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;

    /**
    * Redraws the outline of the physics fixtures to the debug node
    *
    * The debug node is use to outline the fixtures attached to this object.
    * This is very useful when the fixtures have a very different shape than
    * the texture (e.g. a circular shape attached to a square texture).
    */
    virtual void resetDebug() override;

public:
    /** The character's current run speed */
    float _speed = RUN_SPEED;
#pragma mark -
#pragma mark Hidden Constructors

    /**
     * Creates a degenerate Character object.
     *
     * This constructor does not initialize any of the character values beyond
     * the defaults. To use a CharacterModel, you must call init().
     */
    CharacterModel() : CapsuleObstacle(), _sensorName(SENSOR_NAME) {}

    /**
     * Destroys this CharacterModel, releasing all resources.
     */
    virtual ~CharacterModel(void){
        _core = nullptr;
        dispose();
    }

    /**
     * Disposes all resources and assets of this CharacterModel
     *
     * Any assets owned by this object will be immediately released. Once
     * disposed, a CharacterModel may not be used until it is initialized
     * again.
     */
    void dispose();

    /**
     * Initializes a new character at the given position.
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
    virtual bool init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Animation> animation);


#pragma mark -
#pragma mark Static Constructors

    /**
     * Creates a new character at the given position.
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
     * @return  A newly allocated CharacterModel at the given position with the given scale
     */
    static std::shared_ptr<CharacterModel> alloc(const cugl::Vec2 &pos, float drawScale, shared_ptr<Animation> animation) {
        std::shared_ptr<CharacterModel> result = std::make_shared<CharacterModel>();

        return (result->init(pos, drawScale, animation) ? result : nullptr);
    }

#pragma mark -
#pragma mark Animation

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
    void setSceneNode(const std::shared_ptr<cugl::scene2::SpriteNode> &node) {
        _node = node;
        _node->setPosition(getPosition() * _drawScale);
    }

    /** Sets the animation to the string specified, and changes the relevant frame data
     * returns whether the animation was swapped successsfully
     */
    bool setAnimation(string anim){

        // return false if the animation doesn't exist, or we are already on the animation
        if (!_animation->hasKey(anim)) return false;
        if (_currAnimation == anim) return false;


        // change frame data
        _currAnimation = anim;
        _startframe = _animation->getStart(anim);
        _lastframe = _animation->getLast(anim);
        _loop = _animation->isLoop(anim);

        // flip the animation if we need to
        _node->setVisible(false);
        if (_flip ^ _animation->isFlip(anim)){
            _node->setScale(_node->getScale() * Vec2(-1,1));
        }
        _node->setFrame(_currFrame);
        _node->setVisible(true);
        _flip = _animation->isFlip(anim);

        _currFrame = _animation->isReversed() ? _lastframe : _startframe;
        return true;
    }

#pragma mark -
#pragma mark Attribute Properties


    void slowCharacter() {
        _speed = DELAY_SPEED;
    }

    void restoreSpeed() {
        _speed = RUN_SPEED;
    }
    
    bool canJump(){
        return !_jumped;
    }

    /**
     * Sets the character's direction to be facing in the opposite direction that
     * it is currently facing in.
     */
    float getHearts() {
        return _hearts;
    }

    /**
     * Sets the character's direction to be facing in the opposite direction that
     * it is currently facing in.
     */
    void setHearts(float v) {
        _hearts = v;
    }

    /**
     * Sets the character's direction to be facing in the opposite direction that
     * it is currently facing in.
     */
    void flipDirection() {
        _faceRight = !_faceRight;
        int sign = _faceRight?1:-1;
        setLinearVelocity(sign*abs(getLinearVelocity().x), getLinearVelocity().y);
        _node->setScale(_node->getScale()*Vec2(-1,1));
    }

    /**
     * Returns true if the character is currently running.
     *
     * @return true if the character is currently running.
     */
    bool isRunning() const {
        return (_moveState == MovementState::RUNNING);
    }

    /**
     * Returns true if the character is currently jumping.
     *
     * @return true if the character is currently jumping.
     */
    bool isJumping() const {
        return (_moveState == MovementState::JUMPING);
    }

    /**
     * Returns true if the character is on the ground.
     *
     * @return true if the character is on the ground.
     */
    bool isGrounded() const {
        return (_moveState == MovementState::STOPPED || _moveState == MovementState::RUNNING);
    }

    /**
     * Returns true if the character is falling.
     *
     * @return true if the character is falling.
     */
    bool isFalling() const {
        return (_moveState == MovementState::FALLING);
    }

    /**
     * Returns true if the character is on the wall.
     *
     * @return true if the character is on the wall.
     */
    bool isOnWall() const {
        return (_moveState == MovementState::ONWALL);
    }

    /**
     * Returns true if the character is dashing.
     *
     * @return true if the character is on the dashing.
     */
    bool isDashing() const {
        return (_moveState == MovementState::DASHING);
    }

    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string *getSensorName() {
        return &_sensorName;
    }

    /**
     * Returns true if this character is facing right
     *
     * @return true if this character is facing right
     */
    bool isFacingRight() const {
        return _faceRight;
    }

    /**
     * Sets the character's movement state, changing physical attributes
     * accordingly as necessary.
     *
     * The second argument can be used to pass in relevant info for a specific
     * state change (defaults to 0).
     * - To DASHING: this indicates the direction of the dash (-1 or 1)
     *
     * @param state The new movement state the character should be in
     * @param param An argument that can be used for additional state change info
     * @return      Whether the state change happened successfully
     */
    bool setMoveState(MovementState newState, int param=0);

    /**
     * Sets the current position for this physics body
     *
     * This method converts from a CUGL vector type to a Box2D vector type. This
     * cuts down on the confusion between vector types.
     *
     * @param value  the current position for this physics body
     */
    virtual void setPosition(const Vec2 value) override {
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
    virtual void update(float dt) override;
    
    virtual float x_scale(){
        return 1;
    }

    /**
     * Returns the fixture currently associated with this character's face.
     *
     * @return  The fixture currently on the character's face
     */
    b2Fixture *getFaceFixture() {
        return (isFacingRight() ? _faceFixtureRight : _faceFixtureLeft);
    }

    b2Body *getBody() override {
        return _body;
    }
};



#endif /* MPCharacterModel_h */
