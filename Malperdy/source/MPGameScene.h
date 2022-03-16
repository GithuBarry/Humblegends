//
//  MPGameScene.h
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//  
//  Owner: Barry Wang
//  Contributors: Barry Wang
//  Version: 3/01/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#ifndef __MP_GAME_SCENE_H__
#define __MP_GAME_SCENE_H__

#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <vector>
#include "MPReynardModel.h"
#include "MPReynardController.h"
#include "MPCharacterModel.h"
#include "MPEnemyController.h"
#include "MPInput.h"
#include "MPGameStateController.h"
#include "MPRoomModel.h"
#include "MPGridModel.h"
#include "MPEnvController.h"

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {


protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    InputController _input;
    GameStateController _gamestate;


    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::ScrollPane> _worldnode;

    //    /** Reference to the exit message label */
    //    std::shared_ptr<cugl::scene2::Label> _exitnode;

        /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    //std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;
    /** Reference to the rocket/player avatar */
    //std::shared_ptr<RocketModel> _rocket;

    /** Reference to the Reynard controller */
    std::shared_ptr<ReynardController> _reynardController;
    std::shared_ptr<GridModel> _grid;
    std::shared_ptr<EnvController> _envController;
    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture*> _sensorFixtures;

    /** References to all the enemy controllers */
    std::shared_ptr<vector<std::shared_ptr<EnemyController>>> _enemies = make_shared<vector<std::shared_ptr<EnemyController>>>();

    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;

#pragma mark Internal Object Management

    /**
     * Lays out the game geography.
     *
     */
    void populate();

    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with rocket.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * param obj    The physics object to add
     * param node   The scene graph node to attach it to
     */
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
        const std::shared_ptr<cugl::scene2::SceneNode>& node);

    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;

public:
#pragma mark -
#pragma mark Constructors
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<cugl::scene2::ScrollPane> _debugnode;

    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameScene();

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() {
        dispose();
    }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  This initializer uses the default scale.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect rect);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     * @param gravity   The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect rect, const cugl::Vec2 gravity);


#pragma mark -
#pragma mark State Access

    /**
     * Returns true if the gameplay controller is currently active
     *
     * @return true if the gameplay controller is currently active
     */
    bool isActive() const {
        return _active;
    }

    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug() const {
        return _debug;
    }

    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) {
        _debug = value;
        _debugnode->setVisible(value);
    }

    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete() const {
        return _complete;
    }

    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value) {
        _complete = value;
        //        _exitnode->setVisible(value);
    }


#pragma mark -
#pragma mark Gameplay Handling

    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);

#pragma mark Drawing

    /**
     * Draws all of the children in this scene with the given SpriteBatch. This method assumes that the sprite batch is not actively drawing. It will call both begin() and end().
     * Rendering happens by traversing the the scene graph using an "Pre-Order" tree traversal algorithm ( https://en.wikipedia.org/wiki/Tree_traversal#Pre-order ).
     * That means that parents are always draw before (and behind children). To override this draw order, you should place an scene2::OrderedNode in the scene graph to specify an alternative order.
     * @param batch The SpriteBatch to draw with.
     */
    void render(const std::shared_ptr<SpriteBatch>& batch);


    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();


#pragma mark -
#pragma mark Collision Handling

    /**
    * Processes the start of a collision
    *
    * This method is called when we first get a collision between two objects.  We use
    * this method to test if it is the "right" kind of collision.  In particular, we
    * use it to test if we make it to the win door.  We also us it to eliminate bullets.
    *
    * @param  contact  The two bodies that collided
    */
    void beginContact(b2Contact* contact);

    /**
     * Handles any modifications necessary before collision re
     * lution
     *
     * This method is called just before Box2D resolves a collision.  We use
     * this method to implement sound on contact, using the algorithms outlined
     * in Ian Parberry's "Introduction to Game Physics with Box2D".
     *
     * @param  contact  The two bodies that collided
     * @param  contact  The collision manifold before contact
     */
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);
            
    void endContact(b2Contact* contact);

    bool isReynardCollision(b2Contact* contact);
    
    /**
     * Helper function for detecting a collision between two objects
     *
     * The primary purpose of this function is to detect if one of the physical bodies
     * that have come into contact with one another are a trap.
     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  contact  The two bodies that collided
     */
    bool isTrapCollision(b2Contact* contact);
    
    void resolveTrapCollision();
        
    void isTurnEvent(b2Contact* contact);
        
    b2Fixture* getReynardFixture(b2Contact *contact);
        
};

#endif /* __MP_GAME_MODE_H__ */
