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
#define RUN_SPEED 3.7f
/** The speed at which this character jumps */
#define JUMP_SPEED 10.5f

class CharacterModel : public cugl::physics2::CapsuleObstacle {
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
    
    /** Class representing an animation */
    class Animation{
    public:
        // The sprite sheet
        shared_ptr<Texture> _frames;
        
        // Frame data
        int _size;
        int _cols;
        int _rows;
        bool _loop = false;
        
        // Empty constructor, must initialize to usse
        Animation(){};
        
        // Sets all attributes
        bool init(shared_ptr<Texture> frames, int size, int cols, string loop){
            // Frame data
            _frames = frames;
            _size = size;
            _cols = cols;
            // Calculate the number of rows from size & cols
            _rows = (_size-1) / _cols + 1;
            if (loop == "true") _loop = true;
            
            // return false if spritesheet is null or the size is nonpositive
            return frames && size > 0;
        }
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

    /** if the character has dashed since last touching the ground */
    bool _hasDashed = false;

    /** the time that the last dash started */
    Timestamp _dashStart = Timestamp();

#pragma mark -
#pragma mark Constants

    /** The texture for the character avatar */
    const string CHARACTER_TEXTURE;

    /** The amount of time in between each frame update */
    const float FRAME_TIME = 0.03;

    /** The duration in milliseconds of a dash */
    const Uint64 DASH_DURATION = 200;

#pragma mark Attributes

    /** The dictionary of all character animations */
    shared_ptr<map<string, Animation>> _animations;
    
    /** The frame data the current animation */
    Animation _currAnimation;

#pragma mark Attributes

    /** The character's current run speed */
    float _speed = RUN_SPEED;
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
#pragma mark -
#pragma mark Hidden Constructors

    /**
     * Creates a degenerate Character object.
     *
     * This constructor does not initialize any of the character values beyond
     * the defaults. To use a CharacterModel, you must call init().
     */
    CharacterModel() : CapsuleObstacle(), _sensorName(SENSOR_NAME) {
    }

    /**
     * Destroys this CharacterModel, releasing all resources.
     */
    virtual ~CharacterModel(void){
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
    virtual bool init(const cugl::Vec2 &pos, float drawScale, shared_ptr<map<string, Animation>> animations);


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
    static std::shared_ptr<CharacterModel> alloc(const cugl::Vec2 &pos, float drawScale, shared_ptr<map<string, Animation>> animations) {
        std::shared_ptr<CharacterModel> result = std::make_shared<CharacterModel>();

        return (result->init(pos, drawScale, animations) ? result : nullptr);
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

#pragma mark -
#pragma mark Attribute Properties

    /** whether or not the character can dash */
    bool canDash() {
        return (Timestamp().ellapsedMillis(_dashStart) > DASH_DURATION) && !_hasDashed;
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
     * @param state The new movement state the character should be in
     * @return      Whether the state change happened successfully
     */
    bool setMoveState(MovementState newState);

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

    /**
     * Replaces the node with the specified animation
     *
     * @param tex is the key representing an animation in _animations
     *
     * @returns whether or not the animation was uplaoded
     */
    bool uploadTexture(string tex);

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
