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

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for Dashing */
#define DASH_COOLDOWN   20
/** Amount of time (in seconds) for wall slide duration */
#define WALL_SLIDE_DURATION 1.5f
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.45f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.4f
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
bool CharacterModel::init(const cugl::Vec2& pos, float drawScale, shared_ptr<Texture> defaultTexture, shared_ptr<Texture> runAnimation) {
    
    _defaultTexture = defaultTexture;
    _runAnimation = runAnimation;
    
    if(runAnimation){
        uploadTexture("run");
    }
    else{
        uploadTexture("default");
    }
    
    
    Size nsize = defaultTexture->getSize() / drawScale;
    nsize.width  *= DUDE_HSHRINK;
    nsize.height *= DUDE_VSHRINK;
    _drawScale = drawScale;

    // Create physics
    if (CapsuleObstacle::init(pos,nsize)) {
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

bool CharacterModel::uploadTexture(string tex){
    if (tex == "default"){
        // Create sprite for this character from texture and store
        setSceneNode(scene2::SpriteNode::alloc(_defaultTexture, 1, 1));
        _node->setAnchor(0.5, 0.5);
        _node->setScale(0.5);
    }
    else if (tex == "run"){
        // Create sprite for this character from texture and store
        setSceneNode(scene2::SpriteNode::alloc(_runAnimation, 5, 5));
        _node->setAnchor(0.5, 0.5);
        _node->setScale(0.2);
        
        // initial flip of the image
        scene2::TexturedNode* im = dynamic_cast<scene2::TexturedNode*>(_node.get());

        if (im != nullptr) {
            im->flipHorizontal(!im->isFlipHorizontal());
        }
    }
    else{
        return false;
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
    }

    //uploadTexture("default");
    // Do what needs to be done when switching into the new state
    switch (newState) {
    case MovementState::STOPPED:
        break;
    case MovementState::RUNNING:
        // Set character moving in the given direction at the right speed
        setVX((_faceRight ? 1 : -1) * _speed);
        //uploadTexture("run");
        break;
    case MovementState::JUMPING:
        // Jump up
        setVY(JUMP_SPEED);
        // If character is on a wall, then also give a horizontal velocity away
        if (_moveState == MovementState::ONWALL) setVX((_faceRight ? 1 : -1) * RUN_SPEED);
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
    corners[0].x = -DUDE_SSHRINK*getWidth()/2.0f;
    corners[0].y = (-getHeight()+SENSOR_HEIGHT)/2.0f;
    corners[1].x = -DUDE_SSHRINK*getWidth()/2.0f;
    corners[1].y = (-getHeight()-SENSOR_HEIGHT)/2.0f;
    corners[2].x =  DUDE_SSHRINK*getWidth()/2.0f;
    corners[2].y = (-getHeight()-SENSOR_HEIGHT)/2.0f;
    corners[3].x =  DUDE_SSHRINK*getWidth()/2.0f;
    corners[3].y = (-getHeight()+SENSOR_HEIGHT)/2.0f;
    feetFixtureShape.Set(corners,4);
    feetFixture.shape = &feetFixtureShape;
    feetFixture.userData.pointer = 4;
    _feetFixture = _body->CreateFixture(&feetFixture);
    
    // Vars needed for positioning on either side of characters
    float halfWidth = getWidth()/2.0f;
    float quarterWidth = halfWidth/2.0f;
    
    // Left fixture
    b2FixtureDef faceFixture;
    b2PolygonShape faceFixtureShape;
    faceFixture.isSensor = true;
    faceFixture.density = 1;
    faceFixtureShape.SetAsBox(quarterWidth/20.0f, .00001, b2Vec2(halfWidth + quarterWidth, .25), 0);
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

    // If enough time has passed since the last trail marking, add a trail marking
    if (_trailAcc >= TRAIL_INCREMENT) {
        _trailAcc = 0;
        // Get the character's position and add it to their trail
        _trail->push_back(getPosition());
        // Trim trail back down to size if necessary
        if (_trail->size() > TRAIL_LENGTH) _trail->pop_back();
    }
    // Otherwise increment the trail time accumulator
    else {
        _trailAcc += dt;
    }

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
    }

    // Update physics obstacle
    CapsuleObstacle::update(dt);

    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
    }
    
    if(_moveState == MovementState::RUNNING && _runAnimation){
        _elapsed += dt;

        if (_elapsed > FRAME_TIME){
            _currFrame = _currFrame+1;
            if (_currFrame >= _node->getSize())  _currFrame = 0;

            if(!_faceRight){
                _node->setFrame(_currFrame);
            }
            else{
                int row = _currFrame / 5;
                int col = _currFrame % 5;
                col = 4 - col;
                int frame = row*5 + col;
                _node->setFrame(frame);
            }
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
    float w = DUDE_SSHRINK*_dimension.width;
    float h = SENSOR_HEIGHT;
    Poly2 poly(Rect(-w/2.0f,-h/2.0f,w,h));

    _sensorNode = scene2::WireNode::allocWithTraversal(poly, poly2::Traversal::INTERIOR);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width/2.0f, 0.0f));
    _debug->addChild(_sensorNode);
}
