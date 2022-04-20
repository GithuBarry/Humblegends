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
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include <map>

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for Dashing */
#define DASH_COOLDOWN   20
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
/** The impulse for the character dash */
#define DUDE_DASH       10.0f
/** Debug color for the sensor */
#define DEBUG_COLOR     Color4::RED


using namespace cugl;

#pragma mark -
#pragma mark Constructors

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
bool CharacterModel::init(const cugl::Vec2 &pos, float drawScale, shared_ptr<map<string, CharacterModel::Animation>> animations) {

    _animations = animations;
    // create initial scene node with running animation
    //TODO remove hard code
    setSceneNode(scene2::SpriteNode::alloc((*_animations)["run"]._frames, (*_animations)["run"]._rows, (*_animations)["run"]._cols, (*_animations)["run"]._size));
    _node->setScale(Vec2(-0.2,0.2));

    Size nsize = (*_animations)["default"]._frames->getSize() / drawScale;
    auto s = (*_animations)["default"]._frames;

    nsize = nsize *DUDE_WIDTH/nsize.width; //!! drawScale is effective ignored!
    _drawScale = drawScale;
    
    _currAnimation = (*animations)["run"];

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
 * Replaces the node with the specified animation
 *
 * @param tex is the key representing an animation in _animations
 *
 * @returns whether or not the animation was uplaoded
 */
bool CharacterModel::uploadTexture(string tex) {
    
    // If the animation doesn't exist, return false
    if (!(*_animations).count(tex)) return false;
    
    // Jump to the beginning of the animation
    _currFrame = 0;
    _elapsed = 0;
    
    // Make a new node for the animation
    shared_ptr<scene2::SpriteNode> newNode = make_shared<scene2::SpriteNode>();
    newNode->initWithSprite((*_animations)[tex]._frames, (*_animations)[tex]._rows, (*_animations)[tex]._cols, (*_animations)[tex]._size);
    
    // Add the node to the scenegraph, and then delete the old node
    scene2::SceneNode* p = _node->getParent();
    p->addChild(newNode);
    _node->SceneNode::dispose();
    _node = newNode;
    _node->setPosition(getPosition() * _drawScale);
    
    // Update the current Animation class
    _currAnimation = (*_animations)[tex];
    _node->setAnchor(0.5, 0.5);
    
    // Set the scale appropriately (might have to change this)
    if(tex == "default"){
        _node->setScale(_node->getScale()*Vec2(1,1));
    }
    else if (tex == "run" ){
        _node->setScale(_node->getScale()*Vec2(0.2,0.2));
    }
    else{
        _node->setScale(_node->getScale()*Vec2(-0.2,0.2));
    }
    
    // Flip the node if the character is facing right
    if(isFacingRight()){
        _node->setScale(_node->getScale()*Vec2(-1,1));
    }

    return true;
}

/**
 * Sets the character's movement state, changing physical attributes
 * accordingly as necessary.
 *
 * @param state The new movement state the character should be in
 * @return      Whether the state change happened successfully
 */
bool CharacterModel::setMoveState(MovementState newState) {
    //if (newState != MovementState::STOPPED && newState != MovementState::RUNNING) CULog("%d", newState);

    // Do what needs to be done when leaving the old state
    switch (_moveState) {
        case MovementState::STOPPED:
            break;
        case MovementState::RUNNING:
            break;
        case MovementState::JUMPING:
            break;
        case MovementState::FALLING:
            // Ignore attempts to get on wall if character is falling
            //if (newState == MovementState::ONWALL) return false;
            break;
        case MovementState::ONWALL:
            // Reset gravity to normal
            setGravityScale(1.0f);
            break;
        case MovementState::DASHING:
            // Reset gravity to normal
            setVX((_faceRight ? 1 : -1) * RUN_SPEED);
            setGravityScale(1.0f);
            break;
        case MovementState::DEAD:
            // TODO: any changes for swapping from DEAD state, if any
            break;
    }

    // Do what needs to be done when switching into the new state
    switch (newState) {
        case MovementState::STOPPED:
            //uploadTexture("idle");
            break;
        case MovementState::RUNNING:
            // Set character moving in the given direction at the right speed
            setVX((_faceRight ? 1 : -1) * _speed);
            _hasDashed = false;
            if(_moveState != MovementState::RUNNING){
                uploadTexture("run");
            }

            break;
        case MovementState::JUMPING:
            // Disable double jump (jumping/falling to jumping)
            if (_moveState == MovementState::JUMPING || _moveState == MovementState::FALLING) return false;
            // Jump up
            setVY(JUMP_SPEED);
            setVX((_faceRight ? 1 : -1) * JUMP_SPEED / 1.5);
            // If character is on a wall, then also give a horizontal velocity away
            if (_moveState == MovementState::ONWALL) setVX((_faceRight ? 1 : -1) * JUMP_SPEED / 1.5);
            uploadTexture("jump");
            break;
        case MovementState::FALLING:
            break;
        case MovementState::ONWALL:
            // Reduce gravity so that character "sticks" to wall
            setGravityScale(WALL_SLIDE_GRAV_SCALE);
            // Stop moving temporarily as character sticks
            setVX(0);
            setVY(0);
            break;
        case MovementState::DASHING:
            _hasDashed = true;
            setGravityScale(0);
            setVX((_faceRight ? 1 : -1) * RUN_SPEED * 3.0);
            setVY(0);
            _dashStart = Timestamp();
            break;
        case MovementState::DEAD:
            // TODO: any changes for swapping into DEAD state
            break;
    }

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
    _animations = nullptr;

}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void CharacterModel::update(float dt) {
    // Apply cooldowns
    //if (_moveState == MovementState::FALLING || _moveState == MovementState::JUMPING) {
    //    _jumpCooldown = JUMP_COOLDOWN;
    //} else {
    //    // Only cooldown while grounded
    //    _jumpCooldown = (_jumpCooldown > 0 ? _jumpCooldown-1 : 0);
    //
    //}

    //if (isDashing()) {
    //    _dashCooldown = DASH_COOLDOWN;
    //} else {
    //    // Only cooldown while grounded
    //    _dashCooldown = (_dashCooldown > 0 ? _dashCooldown-1 : 0);
    //}

    // Check to make sure Reynard can still continue wall sliding, if he already is
    //if (_moveState == MovementState::ONWALL) {
    //    // If Reynard is out of time
    //    if (_wallSlideDuration >= WALL_SLIDE_DURATION) {
    //        setOnWall(false);
    //        setGrounded(false);
    //        _wallSlideDuration = 0;
    //        _isFallingOffWall = true;
    //    }
    //    // Otherwise, just increment duration
    //    else {
    //        _wallSlideDuration += dt;
    //    }
    //}
    //else _wallSlideDuration = 0;

    // Handle any necessary behavior for the current move state
    switch (_moveState) {
        case MovementState::STOPPED:
            break;
        case MovementState::RUNNING:
            // Continue moving if in the run state
            if (isRunning()) setVX((_faceRight ? 1 : -1) * _speed);
            break;
        case MovementState::JUMPING:
            // If vertical velocity becomes negative, transition to Falling
            if (getVY() <= 0) setMoveState(MovementState::FALLING);
            break;
        case MovementState::FALLING:
            break;
        case MovementState::ONWALL:
            break;
        case MovementState::DASHING:
            if (Timestamp().ellapsedMillis(_dashStart) > DASH_DURATION) {
                if (_groundedCounter <= 0) {
                    setMoveState(MovementState::FALLING);
                } else {
                    setMoveState(MovementState::RUNNING);
                }
            }
        case MovementState::DEAD:
            // TODO: any updates for when in DEAD state
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
    
    if (_moveState == MovementState::RUNNING || _moveState == MovementState::JUMPING) {
        
        // if it is time to update the frame...
        float frame_time = FRAME_TIME * ((_moveState == MovementState::JUMPING) ? 2.0 : 1.0);
        if (_elapsed > frame_time ) {

            // if on the last frame
            if (_currFrame >= _node->getSize()-1){
                // loop the animation if needed
                if(_currAnimation._loop){
                    _currFrame = 0;
                }
            }
            // if not on the last frame, then increment
            else{
                _currFrame = _currFrame + 1;
            }
            _node->setFrame(_currFrame);
            // reset time since last frame update
            _elapsed = 0;
        }
    }

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
