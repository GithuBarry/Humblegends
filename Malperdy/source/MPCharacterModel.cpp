//
//  MPCharacterModel.cpp
//  Malperdy
//
//  This is a superclass for the model that will hold data for a character,
//  which includes enemies and Reynard. The model will simply hold data
//  about the character, and all changes will be done using the controller.
//
//  This class should only be instantiated by a controller.
//
//  Created by Kristina Gu on 3/8/22.
//  Copyright � 2022 Humblegends. All rights reserved.
//

#include "MPCharacterModel.h"
#include "MPAudioController.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include <map>

#pragma mark -
#pragma mark Physics Constants
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
#define DASH_MULTIPLIER 5

/** The distance **/

using namespace cugl;

#pragma mark -
#pragma mark Constructors

//TODO: Refactor this naming convention.
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
 * @param pos       Initial position in world coordinates
 * @param drawScale The drawing scale (world to screen)
 * @param image     The image for the character's appearance
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool CharacterModel::init(const cugl::Vec2 &pos, float drawScale, shared_ptr<Animation> animation) {

    _animation = animation;
    // create initial scene node with running animation

    setSceneNode(scene2::SpriteNode::alloc(_animation->getSheet(), _animation->getRows(), _animation->getCols()));

    _node->setScale(Vec2(-0.2,0.2));

    Size nsize = _animation->getSheet()->getSize() / drawScale / Vec2(_animation->getCols(), _animation->getRows());
    auto s = _animation->getSheet();

    nsize = nsize *DUDE_WIDTH/nsize.width; //!! drawScale is effective ignored!
    _drawScale = drawScale;

    setAnimation("run");

    // Create physics
    if (CapsuleObstacle::init(pos, nsize)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);      // Ensure there is no friction between Reynard and walls
        setFixedRotation(true); // Prevent character from rotating

        /*_jumpCooldown  = 0;
        _dashCooldown  = 0;
        _wallSlideDuration = 0;*/
        return true;
    }
    return false;
}


#pragma mark -
#pragma mark Attribute Properties

/**
 * Sets the character's movement state, changing physical attributes
 * accordingly as necessary.
 * 
 * The second argument can be used to pass in relevant info for a specific
 * state change (defaults to 0).
 * - To DASHING: this indicates the direction of the dash (-1 or 1)
 * - From DASHING: 0 if the dash is not over, 1 otherwise
 *
 * @param state The new movement state the character should be in
 * @param param An argument that can be used for additional state change info
 * @return      Whether the state change happened successfully
 */
bool CharacterModel::setMoveState(MovementState newState, int param) {
    //if (newState != MovementState::STOPPED && newState != MovementState::RUNNING) CULog("%d", newState);

    // Fail if trying to transition into the same state
    if (_moveState == newState) return false;

    // Do what needs to be done when leaving the old state
    switch (_moveState) {
//        case MovementState::STOPPED:
        //case MovementState::RUNNING:
//        case MovementState::JUMPING:
//            //Nothing should happen
//            return false;
//            break;
//        case MovementState::FALLING:
//            // Ignore attempts to get on wall if character is falling
//            //if (newState == MovementState::ONWALL) return false;
//            break;
        case MovementState::ONWALL:
            // Reset gravity to normal
            setGravityScale(1.0f);
            break;
        case MovementState::DASHING:
            // Only allow breaking from dash if dash has ended or switching to wall slide OR jump (jump cancel)
            if (!param && newState != MovementState::ONWALL &&
                newState != MovementState::JUMPING) return false;
            setGravityScale(1.0f);
            break;
        case MovementState::DEAD:
            // TODO: any changes for swapping from DEAD state, if any
            //Nothing should happen
            return false;
            break;
    }
    


    // Do what needs to be done when switching into the new state
    switch (newState) {
        case MovementState::STOPPED:
            setAnimation("idle");
            _jumped = false;
            break;
        case MovementState::RUNNING:
            // Set character moving in the given direction at the right speed
            //setVX(0);
            //setVX((_faceRight ? 1 : -1) * RUN_SPEED);

            if (_moveState == MovementState::JUMPING || _moveState == MovementState::FALLING || _moveState == MovementState::ONWALL)
            {
                AudioController::playSFX(LAND_SOUND);
            }

            _speed = RUN_SPEED;
            setAnimation("run");
            _jumped = false;
            break;
        case MovementState::JUMPING:
            // Disable double jump (jumping/falling to jumping)
            if (_moveState == MovementState::FALLING) return false;
            // Jump up
            //setVX(JUMP_SPEED/1.8);
            setVY(JUMP_SPEED);
            setPosition(getPosition() + Vec2(0, 0.1));

            // If character is on a wall, then also give a horizontal velocity away
            //if (_moveState == MovementState::ONWALL) setVX((_faceRight ? 1 : -1) * JUMP_SPEED / 1.8);
            setAnimation("jump");
            _jumped = true;

            break;
        case MovementState::FALLING:
            break;
        case MovementState::ONWALL:
            setVX(0);
            setVY(0);
            // Reduce gravity so that character "sticks" to wall
            setGravityScale(WALL_SLIDE_GRAV_SCALE);
            // Stop moving temporarily as character sticks
            _currAnimation = "";
            _jumped = false;
            break;
        case MovementState::DASHING:
            // Don't allow dashing if dash cooldown hasn't finished
            if (Timestamp().ellapsedMillis(_dashStart) <= DASH_COOLDOWN) {
                return false;
            }
            
            if ((param > 0) != _faceRight){
                flipDirection();
                // Return so that flip direction does not incur colddown for player flexibility
                return true;
            }
            _dashStart = Timestamp();

            // Play dash sound since we can actually perform a dash.
            //(this->getPosition() - _worldnode->getPaneTransform().transform(Vec2()) / _scale).norm()
            AudioController::playSFX(DASH_SOUND);

            // Set to 0 in case of dashing in the air
            setVY(0);
            setGravityScale(0);
            // Flip the direction to dash direction
            setVX(param * RUN_SPEED * DASH_MULTIPLIER * x_scale());
            
            // Freeze animation while dashing
            _currAnimation = "";
            break;
        case MovementState::DEAD:
            // TODO: any changes for swapping into DEAD state
            setVX(0);
            setVY(0);

            setAnimation("dead");

            break;
    }

//    if(_moveState == MovementState::DASHING && newState == MovementState::RUNNING){
//        _jumped = true;
//    }
    
    // Officially change state
    _moveState = newState;

    return true;
}

#pragma mark -
#pragma mark Physics Methods

/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void CharacterModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }

    CapsuleObstacle::createFixtures();

    // Ground fixtures
    b2FixtureDef feetFixture;
    b2PolygonShape feetFixtureShape;
    // Setting it as sensor allows for overlapping elements
    feetFixture.isSensor = true;
    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -DUDE_SSHRINK * getWidth() / 2.0f;
    corners[0].y = (-getHeight() + SENSOR_HEIGHT) / 2.0f;
    corners[1].x = -DUDE_SSHRINK * getWidth() / 2.0f;
    corners[1].y = (-getHeight() - SENSOR_HEIGHT) / 2.0f;
    corners[2].x = DUDE_SSHRINK * getWidth() / 2.0f;
    corners[2].y = (-getHeight() - SENSOR_HEIGHT) / 2.0f;
    corners[3].x = DUDE_SSHRINK * getWidth() / 2.0f;
    corners[3].y = (-getHeight() + SENSOR_HEIGHT) / 2.0f;
    feetFixtureShape.Set(corners, 4);
    feetFixture.shape = &feetFixtureShape;
    feetFixture.userData.pointer = 4;
    _feetFixture = _body->CreateFixture(&feetFixture);

    // Vars needed for positioning on either side of characters
    float halfWidth = getWidth() / 2.0f;
    float quarterWidth = halfWidth / 2.0f;

    // Left fixture
    b2FixtureDef faceFixture;
    b2PolygonShape faceFixtureShape;
    faceFixture.isSensor = true;
    faceFixture.density = 1;
    faceFixtureShape.SetAsBox(quarterWidth / 20.0f, .00001, b2Vec2(halfWidth, .25), 0);
    faceFixture.shape = &faceFixtureShape;
    faceFixture.userData.pointer = 5;
    _faceFixtureLeft = _body->CreateFixture(&faceFixture);

    // Right fixture
    b2FixtureDef faceFixtureR;
    b2PolygonShape faceFixtureShapeR;
    faceFixtureR.isSensor = true;
    faceFixtureR.density = 1;
    faceFixtureShapeR.SetAsBox(quarterWidth, .00001, b2Vec2(-quarterWidth, .25), 0);
    faceFixtureR.shape = &faceFixtureShapeR;
    faceFixtureR.userData.pointer = 6;
    _faceFixtureRight = _body->CreateFixture(&faceFixtureR);

}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void CharacterModel::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_feetFixture != nullptr) {
        _body->DestroyFixture(_feetFixture);
        _feetFixture = nullptr;
    }
    if (_faceFixtureLeft != nullptr) {
        _body->DestroyFixture(_faceFixtureLeft);
        _faceFixtureLeft = nullptr;
    }
    if (_faceFixtureRight != nullptr) {
        _body->DestroyFixture(_faceFixtureRight);
        _faceFixtureRight = nullptr;
    }
}

/**
 * Disposes all resources and assets of this CharacterModel
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a CharacterModel may not be used until it is initialized again.
 */
void CharacterModel::dispose() {
    _node = nullptr;
    _sensorNode = nullptr;
    _animation = nullptr;

}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void CharacterModel::update(float dt) {

    float jump_x = JUMP_SPEED/1.8 ;

    // Handle any necessary behavior for the current move state
    switch (_moveState) {
        case MovementState::STOPPED:
            break;
        case MovementState::RUNNING:
            // Continue moving if in the run state
            if (_speed>RUN_SPEED){
                //_speed -= 0.1;
            }
            if (isRunning()) setVX((_faceRight ? 1 : -1) * _speed);
            break;
        case MovementState::JUMPING:
            // If vertical velocity becomes negative, transition to Falling

            if (_speed>JUMP_SPEED/1.8){
                _speed -= 0.1;
                jump_x = _speed;
            }
            setVX((_faceRight ? 1 : -1) * jump_x);
            if (getVY() <= -0.2)
                setMoveState(MovementState::FALLING);
            break;
        case MovementState::FALLING:
            if (_speed>JUMP_SPEED/1.8){
                _speed -= 0.1;
                jump_x = _speed;
            }
            setVX((_faceRight ? 1 : -1) * jump_x);
            break;
        case MovementState::ONWALL:
            break;
        case MovementState::DASHING:
            if (Timestamp().ellapsedMillis(_dashStart) > DASH_DURATION) {
                setMoveState(MovementState::FALLING, 1);
            }
            break;
        case MovementState::DEAD:
            // TODO: any updates for when in DEAD state
            setBodyType(b2_staticBody);
            break;
    }

    // Update physics obstacle
    CapsuleObstacle::update(dt);

    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

    // UPDATE THE ANIMATION

    // update time since last frame update
    _elapsed += dt;

    //if (true && (_moveState == MovementState::RUNNING || _moveState == MovementState::JUMPING)) {

        // if it is time to update the frame...
        float frame_time = FRAME_TIME;

        switch (_moveState)
        {
            case MovementState::JUMPING:
                frame_time *= 2;
                break;
            case MovementState::DASHING:
                frame_time *= 0.1;
                break;
        }

        if (_elapsed > frame_time ) {

            // if on the last frame
            if ((_animation->isReversed() ? _currFrame <= _startframe : _currFrame >= _lastframe)){
                // loop the animation if needed

                if(_loop){
                    _currFrame = (_animation->isReversed()  ? _lastframe : _startframe);
                }
            }
            // if not on the last frame, then increment
            else{
                _currFrame = (_animation->isReversed() ? _currFrame - 1 : _currFrame + 1);
            }
            _node->setFrame(_currFrame);
            // reset time since last frame update
            _elapsed = 0;
        }
    //}

}


#pragma mark -
#pragma mark Scene Graph Methods

/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void CharacterModel::resetDebug() {
    CapsuleObstacle::resetDebug();
    float w = DUDE_SSHRINK * _dimension.width;
    float h = SENSOR_HEIGHT;
    Poly2 poly(Rect(-w / 2.0f, -h / 2.0f, w, h));

    _sensorNode = scene2::WireNode::allocWithTraversal(poly, poly2::Traversal::INTERIOR);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width / 2.0f, 0.0f));
    _debug->addChild(_sensorNode);
}
