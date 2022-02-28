//
//  RDRocketModel.cpp
//  Rocket Demo
//
//  This encapsulates all of the information for the rocket demo.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Pay close attention to how this class designed. This class uses our standard
//  shared-pointer architecture which is common to the entire engine.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  Note that this object manages its own texture, but DOES NOT manage its own
//  scene graph node.  This is a very common way that we will approach complex
//  objects.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "MPReynardModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Animation and Physics Constants

#define DEFAULT_IS_JUMPING false
#define DEFAULT_JUMP_COOLDOWN 2.0f
#define DEFAULT_IS_DASHING false
#define DASH_SPEED 3.0f
 

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new Reynard at the given position. 
 *
 * @param  pos      Initial position of Reynard
 * @param  size   	The dimensions of the box.
 *
 * @return  true if Reynard is initialized properly, false otherwise.
 */
bool ReynardModel::init(const Vec2 pos) {
    physics2::BoxObstacle::init(pos);
    setPosition(pos);
    _velocity = Vec2(1.0, 1.0);
    setVelocity(_velocity);
    setJumping(DEFAULT_IS_JUMPING);
    setDashing(DEFAULT_IS_DASHING);
    return true;
}



#pragma mark -
#pragma mark Physics

/**
 * Updates the Reynard's physics state (NOT GAME LOGIC).
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
void ReynardModel::update(float delta) {
    Obstacle::update(delta); 

