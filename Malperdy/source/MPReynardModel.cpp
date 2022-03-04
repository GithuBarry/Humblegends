//
//  PFDudeModel.cpp
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
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#include "MPReynardModel.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#define SIGNUM(x)  ((x > 0) - (x < 0))

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for Dashing */
#define DASH_COOLDOWN   20
/** Cooldown (in animation frames) for shooting */
#define SHOOT_COOLDOWN  20
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.95f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.7f
/** The amount to shrink the sensor fixture (horizontally) relative to the image */
#define DUDE_SSHRINK  0.6f
/** Height of the sensor attached to the player's feet */
#define SENSOR_HEIGHT   0.1f
/** The density of the character */
#define DUDE_DENSITY    1.0f
/** The impulse for the character jump */
#define DUDE_JUMP       1000.0f
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
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool ReynardModel::init(const cugl::Vec2& pos, const cugl::Size& size, float drawScale) {
    Size nsize = size;
    nsize.width  *= DUDE_HSHRINK;
    nsize.height *= DUDE_VSHRINK;
    _drawScale = drawScale;

    _position = pos;
    _movement = 0.0f;

    if (CapsuleObstacle::init(pos,nsize)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);      // HE WILL STICK TO WALLS IF YOU FORGET
        setFixedRotation(true); // OTHERWISE, HE IS A WEEBLE WOBBLE
        _currentState = ReynardState::SPAWN;

        // Gameplay attributes
        _isGrounded = false;
        _isJumping  = false;
        _isDashing  = false;
//        TODO: Prevent an issue here about spawning him on the left side of rooms by including a part in the init function maybe
//        Specifically for moments where we come from the left side of a room to go to the right.
        _faceRight  = true;
        
        _jumpCooldown  = 0;
        _dashCooldown  = 0;
        return true;
    }
    return false;
}


#pragma mark -
#pragma mark Attribute Properties

/**
 * Sets left/right movement of this character.
 *
 * This is the result of input times dude force.
 *
 * @param value left/right movement of this character.
 */
void ReynardModel::setMovement(float value) {
    _movement = value;
    bool face = _movement > 0;
    if (_movement == 0 || _faceRight == face) {
        return;
    }

    // Change facing
    scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
    if (image != nullptr) {
        image->flipHorizontal(!image->isFlipHorizontal());
    }
    _faceRight = face;
}


#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void ReynardModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }

    CapsuleObstacle::createFixtures();
    b2FixtureDef sensorDef;
    sensorDef.density = DUDE_DENSITY;
    sensorDef.isSensor = true;

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

    b2PolygonShape sensorShape;
    sensorShape.Set(corners,4);

    sensorDef.shape = &sensorShape;
    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());
    _sensorFixture = _body->CreateFixture(&sensorDef);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void ReynardModel::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }
}

/**
 * Disposes all resources and assets of this DudeModel
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a DudeModel may not be used until it is initialized again.
 */
void ReynardModel::dispose() {
    _node = nullptr;
    _sensorNode = nullptr;
}

/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void ReynardModel::applyForce() {
    if (!isEnabled()) {
        return;
    }

//  TODO: Dampen Player Movement
//    if (getMovement() == 0.0f) {
//        if (isGrounded()) {
//            // Instant friction on the ground
//            b2Vec2 vel = _body->GetLinearVelocity();
//            vel.x = 0; // If you set y, you will stop a jump in place
//            _body->SetLinearVelocity(vel);
//        } else {
//            // Damping factor in the air
//            b2Vec2 force(-getDamping()*getVX(),0);
//            _body->ApplyForce(force,_body->GetPosition(),true);
//        }
//    }
    
    // Get scalar representing Reynard's direction
    int direction = _faceRight ? 1 : -1;

    // If Reynard has reached his max speed, then clamp his speed
    if (fabs(getVX()) >= REYNARD_MAX_SPEED) {
        setVX((getVX() / fabs(getVX())) * REYNARD_MAX_SPEED);
    }
    // Otherwise, continue accelerating
    else {
        b2Vec2 force(direction * REYNARD_ACC,0);
        _body->ApplyForce(force,_body->GetPosition(),true);
    }

    // Jump!
    if (!isJumping() && isGrounded()) {
        b2Vec2 force(0, DUDE_JUMP);
        _body->ApplyLinearImpulse(force,_body->GetPosition(),true);
    }
}

// The reason for this duplicate code existing is complicated and will be gone over with Barry.
bool ReynardModel::applyJumpForce() {
//    TODO: Should only jump when grounded .
    if (true) {
        cout<<"ACTUALLY_JUMPING"<<endl;
        b2Vec2 force(0, DUDE_JUMP);
        _body->ApplyLinearImpulse(force,_body->GetPosition(),true);
        return true;
    }
    return false;
}

bool ReynardModel::applyDashForce() {
//    TODO: Check what direction this is going in before calling ti. 
    if (isDashing()) {
        b2Vec2 force(DUDE_DASH, 0);
        _body->ApplyLinearImpulse(force,_body->GetPosition(),true);
        return true;
//      TODO: TEST THAT THIS WILL GET
    }
    return false;
}


/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void ReynardModel::update(float dt) {
    // Apply cooldowns
    if (isJumping()) {
        _jumpCooldown = JUMP_COOLDOWN;
    } else {
        // Only cooldown while grounded
        _jumpCooldown = (_jumpCooldown > 0 ? _jumpCooldown-1 : 0);
        
    }

    if (isDashing()) {
        _dashCooldown = DASH_COOLDOWN;
    } else {
        // Only cooldown while grounded
        _dashCooldown = (_dashCooldown > 0 ? _dashCooldown-1 : 0);
    }

    // Update physics obstacle
    CapsuleObstacle::update(dt);

    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
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
void ReynardModel::resetDebug() {
    CapsuleObstacle::resetDebug();
    float w = DUDE_SSHRINK*_dimension.width;
    float h = SENSOR_HEIGHT;
    Poly2 poly(Rect(-w/2.0f,-h/2.0f,w,h));

    _sensorNode = scene2::WireNode::allocWithTraversal(poly, poly2::Traversal::INTERIOR);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width/2.0f, 0.0f));
    _debug->addChild(_sensorNode);
}
