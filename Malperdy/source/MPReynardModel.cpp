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
/** Amount of time (in seconds) for wall slide duration */
#define WALL_SLIDE_DURATION 1.5f
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
#define DUDE_JUMP       500.0f
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

    if (CapsuleObstacle::init(pos,nsize)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);      // Ensure there is no friction between Reynard and walls
        setFixedRotation(true); // Prevent character from rotating
        _moveState = MovementState::RUNNING;

        // Gameplay attributes
//        TODO: Prevent an issue here about spawning him on the left side of rooms by including a part in the init function maybe
//        Specifically for moments where we come from the left side of a room to go to the right.
        _faceRight  = true;
        
        _jumpCooldown  = 0;
        _dashCooldown  = 0;
        _wallSlideDuration = 0;
        return true;
    }
    return false;
}


#pragma mark -
#pragma mark Attribute Properties


/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
//void ReynardModel::update(float dt) {
//    // Apply cooldowns
//    if (_moveState == MovementState::INAIR) {
//        _jumpCooldown = JUMP_COOLDOWN;
//    } else {
//        // Only cooldown while grounded
//        _jumpCooldown = (_jumpCooldown > 0 ? _jumpCooldown-1 : 0);
//        
//    }
//
//    //if (isDashing()) {
//    //    _dashCooldown = DASH_COOLDOWN;
//    //} else {
//    //    // Only cooldown while grounded
//    //    _dashCooldown = (_dashCooldown > 0 ? _dashCooldown-1 : 0);
//    //}
//
//    // Check to make sure Reynard can still continue wall sliding, if he already is
//    if (_moveState == MovementState::ONWALL) {
//        // If Reynard is out of time
//        if (_wallSlideDuration >= WALL_SLIDE_DURATION) {
//            setOnWall(false);
//            setGrounded(false);
//            _wallSlideDuration = 0;
//            _isFallingOffWall = true;
//        }
//        // Otherwise, just increment duration
//        else {
//            _wallSlideDuration += dt;
//        }
//    }
//    else _wallSlideDuration = 0;
//
//    // Update physics obstacle
//    CapsuleObstacle::update(dt);
//
//    if (_node != nullptr) {
//        _node->setPosition(getPosition()*_drawScale);
//        _node->setAngle(getAngle());
//    }
//}