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

#include "MPCharacterModel.h"

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
class ReynardModel : public CharacterModel {

protected:
    /** Vec2 representing position */
    cugl::Vec2 _position;
    /** Which direction is the character facing */
    bool _faceRight;
    /** How long until Reynard drops off a wall */
    float _wallSlideDuration;
    /** How long until we can jump again */
    int _jumpCooldown;
    /** How long until we can dash again */
    int  _dashCooldown;

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


//#pragma mark Hidden Constructors
//
//    /**
//     * Creates a degenerate Dude object.
//     *
//     * This constructor does not initialize any of the dude values beyond
//     * the defaults.  To use a DudeModel, you must call init().
//     */
//    ReynardModel() : CharacterModel(), _sensorName(SENSOR_NAME) { }
//
//    /**
//     * Destroys this DudeModel, releasing all resources.
//     */
//    virtual ~ReynardModel(void) { dispose(); }
//
//    /**
//     * Disposes all resources and assets of this DudeModel
//     *
//     * Any assets owned by this object will be immediately released.  Once
//     * disposed, a DudeModel may not be used until it is initialized again.
//     */
//    void dispose();
//
    /**
     * Initializes Reynard at the given position.
     *
     * Reynard is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of Reynard in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    bool init(const cugl::Vec2 &pos, const cugl::Size &size, float drawScale);

#pragma mark -
#pragma mark Static Constructors

    /**
     * Creates a new Reynard at the given position.
     *
     * Reynard has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  A newly allocated ReynardModel at the given position with the given scale
     */
    static std::shared_ptr<ReynardModel> alloc(const cugl::Vec2 &pos, const cugl::Size &size, float drawScale) {
        std::shared_ptr<ReynardModel> result = std::make_shared<ReynardModel>();
        return (result->init(pos, size, drawScale) ? result : nullptr);
    }


//#pragma mark -
//#pragma mark Animation
//
//    /**
//     * Returns the scene graph node representing this DudeModel.
//     *
//     * By storing a reference to the scene graph node, the model can update
//     * the node to be in sync with the physics info. It does this via the
//     * {@link Obstacle#update(float)} method.
//     *
//     * @return the scene graph node representing this DudeModel.
//     */
//    const std::shared_ptr<cugl::scene2::SceneNode> &getSceneNode() const {
//        return _node;
//    }
//
//    /**
//     * Sets the scene graph node representing this DudeModel.
//     *
//     * Note that this method also handles creating the nodes for the body parts
//     * of this DudeModel. Since the obstacles are decoupled from the scene graph,
//     * initialization (which creates the obstacles) occurs prior to the call to
//     * this method. Therefore, to be drawn to the screen, the nodes of the attached
//     * bodies must be added here.
//     *
//     * The bubbler also uses the world node when adding bubbles to the scene, so
//     * the input node must be added to the world BEFORE this method is called.
//     *
//     * By storing a reference to the scene graph node, the model can update
//     * the node to be in sync with the physics info. It does this via the
//     * {@link Obstacle#update(float)} method.
//     *
//     * @param node  The scene graph node representing this DudeModel, which has been added to the world node already.
//     */
//    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode> &node) {
//        _node = node;
//        _node->setPosition(getPosition() * _drawScale);
//    }
};

#endif /* __PF_REYNARD_MODEL_H__ */
