//
//  MPGameScene.h
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Owner: Barry Wang
//  Contributors: Barry Wang, Jordan Selin
//  Version: 5/06/22
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
#include "MPCheckpointKey.h"
#include "MPCheckpointKeyCrazy.hpp"
#include "MPGameStateController.h"
#include "MPRoomModel.h"
#include "MPGridModel.h"
#include "MPEnvController.h"
#include "MPAudioController.h"
#include "MPTutorial.hpp"

/** Reynard's start location */
// y-position is approx 6 * y-origin of first region
// was (2,16) when region 1 was at (0,0)
#define REYNARD_START Vec2(4,153)
/** The y-offset from the origin for tutorials in rooms */
#define TUTORIAL_Y_OFFSET 23

/** Room dimensions in tiles */
#define ROOM_WIDTH 12
#define ROOM_HEIGHT 8

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
private:
    /** Whether or not to load from a save file on start */
    bool _loadFromSave = false;

protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    InputController _input;
    GameStateController _gamestate;
    Vec2 lastFramePos = Vec2();

    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::ScrollPane> _worldnode;

    /** Reference to the debug root of the scene graph */
    std::shared_ptr<cugl::scene2::ScrollPane> _debugnode;

    /** Reference to the win root of the scene graph */
    std::shared_ptr<cugl::scene2::Label> _winNode;

    /** Reference to the health bar scene node */
    std::shared_ptr<cugl::scene2::PolygonNode> _health;

    /** Reference to the key count icon scene node */
    std::shared_ptr<cugl::scene2::PolygonNode> _keyUI;

    /** Reference to the pause button */
    std::shared_ptr<cugl::scene2::PolygonNode> _pause;

    /** Reference to the pause button scene node */
    std::shared_ptr<cugl::scene2::PolygonNode> _pauseButton;

    // PHYSICS
    /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    /** Reference to the Reynard controller */
    std::shared_ptr<ReynardController> _reynardController;
    std::shared_ptr<GridModel> _grid;
    std::shared_ptr<EnvController> _envController;
    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture *> _sensorFixtures;

    /** References to all the enemy controllers */
    std::shared_ptr<vector<std::shared_ptr<EnemyController>>> _enemies;

    /** References to all the tutorials */
    std::shared_ptr<vector<std::shared_ptr<Tutorial>>> _tutorials;


    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;

    /** checkpoint for swap history length*/
    int _checkpointSwapLen = 0;

    vector<int> _checkpointActivatedCheckpoints;
    vector<Vec2> _checkpointEnemyPos;

    /** A store position of reynard before reset*/
    Vec2 _checkpointReynardPos = REYNARD_START;

    /* Offset of scrolling */
    Vec2 scrollingOffset = Vec2();

    /** A store position of room swapping history */
    vector<vector<Vec2>> _swapHistory;

    /**Workaround for wall jump corner stuck*/
    int corner_num_frames_workaround = 0;

    /*
     Remaining number of frames to color reynard red
     */
    int keepRedFrames = 0;

    /** The enemy locations to spawn keys for next frame, in PHYSICS space */
    shared_ptr<vector<Vec2>> deadKeyEnemyLocs = make_shared<vector<Vec2>>();

    /** All the regular keys in the level */
    vector<std::shared_ptr<CheckpointKey>> _keys;

    /** All the possessed keys in the level */
    vector<std::shared_ptr<CheckpointKeyCrazy>> _keysCrazy;

    /** Whether we're showing the region exit or not */
    bool showingRegExit = false;

public:

    /*
     Flag past from owner of this class to indicate whether to start a new game.
     */
    int _mode = 0;

    void setMode(int mode) {
        _mode = mode;
    }

    void pause() {
        _gamestate.pause();
    }

protected:

    /**
     * Last time reynard hurt
     */
    std::chrono::time_point<std::chrono::system_clock> _lastHurt = std::chrono::system_clock::now();


#pragma mark Internal Object Management

    /**
     * Lays out the game geography and characters
     *
     */
    void populate();

    /**
     * Lays out the game geography.Part of populate()
     *
     */
    void populateEnv();

    /**
     * Places all the characters, including Reynard and enemies, in the game world.
     */
    void populateChars();

    /**
     * Places Reynard in the game world.
     */
    void populateReynard();

    /**
     * Places all the enemies for the active regions in the game world.
     */
    void populateEnemies();

    /**
     * Places the enemies for the given region in the game world.
     * 
     * Allows us to populate enemies on a per-region basis, instead
     * of loading them all in at once and potentially causing runtime
     * issues.
     * 
     * @param region    The region to populate the enemies for.
     */
    void populateEnemiesInRegion(shared_ptr<RegionModel> region);

    /**
     * Place all the tutorials at their correct locations in Region 1.
     */
    void populateTutorials();

    /**
     * Helper function that creates, transforms, and places the node for an environment image
     * 
     * @param x     x-coordinate in ROOM coordinates
     * @param y     y-coordinate in ROOM coordinates
     */
    void placeEnvImage(float x, float y, float scale, string TextureName);

public:
    /**
     * Save all the states of the game to a file
     */
    void rewriteSaveFile() {
        //Init a JSON file
        vector<std::string> file_path_list = vector<std::string>(2);
        file_path_list[0] = Application::get()->getSaveDirectory();
        file_path_list[1] = "state.json";
        if (filetool::file_exists(cugl::filetool::join_path(file_path_list))) {
            cugl::filetool::file_delete(cugl::filetool::join_path(file_path_list));
        }
        shared_ptr<JsonWriter> jw = JsonWriter::alloc(cugl::filetool::join_path(file_path_list));
        if (jw == nullptr) {
            CULog("GameScene.h: Saving failed");
            return;
        }

        /**
         * JSON structure:
         * "state.json"
         *  - "EnemyPos" :      [enemy1Pos's x, enemy1Pos's y, ...]
         *  - "ReynardPos" :    [reynardPos's x, reynardPos's y]
         *  - "RoomSwap" :     [Swap1-Room1-x, Swap1-Room1-y, Swap1-Room2-x, Swap1-Room2-y, Swap2....]
         */
        //Init json objects
        std::shared_ptr<JsonValue> jsonRoot = JsonValue::alloc(JsonValue::Type::ObjectType);
        jsonRoot->initObject();

        // JSON - Enemy
        std::shared_ptr<JsonValue> jsonEnemyPos = JsonValue::alloc(JsonValue::Type::ArrayType);
        jsonEnemyPos->initArray();
        for (auto thisEnemy: *_enemies) {
            jsonEnemyPos->appendValue(thisEnemy->getCharacter()->getPosition().x);
            jsonEnemyPos->appendValue(thisEnemy->getCharacter()->getPosition().y);
        }
        jsonRoot->appendChild("EnemyPos", jsonEnemyPos);

        // JSON - Reynard
        std::shared_ptr<JsonValue> jsonReynardPos = JsonValue::alloc(JsonValue::Type::ArrayType);
        jsonReynardPos->initArray();
        jsonReynardPos->appendValue(_reynardController->getCharacter()->getPosition().x);
        jsonReynardPos->appendValue(_reynardController->getCharacter()->getPosition().y);
        jsonRoot->appendChild("ReynardPos", jsonReynardPos);

        // JSON - Checkpoint
        std::shared_ptr<JsonValue> jsonCheckpoints = JsonValue::alloc(JsonValue::Type::ArrayType);
        for (int i: _checkpointActivatedCheckpoints) {
            float fi = i;
            jsonCheckpoints->appendValue(fi);
        }
        jsonRoot->appendChild("ActivatedCheckpoints", jsonCheckpoints);

        // JSON - Room Swapping
        std::shared_ptr<JsonValue> jsonRoomSwap = JsonValue::alloc(JsonValue::Type::ArrayType);
        jsonRoomSwap->initArray();
        int hisotoryLen = static_cast<int>(_envController->getSwapHistory().size());
        for (int i = 0; i < hisotoryLen; i++) {
            jsonRoomSwap->appendValue(_envController->getSwapHistory()[i][0].x);
            jsonRoomSwap->appendValue(_envController->getSwapHistory()[i][0].y);
            jsonRoomSwap->appendValue(_envController->getSwapHistory()[i][1].x);
            jsonRoomSwap->appendValue(_envController->getSwapHistory()[i][1].y);
        }
        jsonRoot->appendChild("RoomSwap", jsonRoomSwap);

        // Save JSON file
        jw->writeJson(jsonRoot);
        jw->flush();
        jw->close();
    }

    /**
    * Save all the states of the game to a file
    */
    bool readSaveFile() {
        //Init a JSON
        vector<std::string> file_path_list = vector<std::string>(2);
        file_path_list[0] = Application::get()->getSaveDirectory();
        file_path_list[1] = "state.json";
        if (!filetool::file_exists(cugl::filetool::join_path(file_path_list))) {
            return false;
        }
        shared_ptr<JsonReader> jr = JsonReader::alloc(cugl::filetool::join_path(file_path_list));
        jr->reset();
        if (!jr->ready()) {
            cugl::filetool::file_delete(cugl::filetool::join_path(file_path_list));
            return false;
        }
        std::shared_ptr<JsonValue> jsonRoot = jr->readJson();

        //Read files
        if (jsonRoot->get("EnemyPos") == nullptr
                || jsonRoot->get("ReynardPos") == nullptr
                || jsonRoot->get("RoomSwap") == nullptr
                || jsonRoot->get("ActivatedCheckpoints") == nullptr) {
            cugl::filetool::file_delete(cugl::filetool::join_path(file_path_list));
            return false;
        }
        std::vector<float> enemyPos1D = jsonRoot->get("EnemyPos")->asFloatArray();
        std::vector<float> reynardPos1D = jsonRoot->get("ReynardPos")->asFloatArray();
        std::vector<float> swapHistory1D = jsonRoot->get("RoomSwap")->asFloatArray();
        std::vector<int> activatedcheckpts1D = jsonRoot->get("ActivatedCheckpoints")->asIntArray();
        if (enemyPos1D.size() % 2 != 0 || reynardPos1D.size() != 2 || swapHistory1D.size() % 4 != 0) {
            cugl::filetool::file_delete(cugl::filetool::join_path(file_path_list));
            return false;
        }

        // JSON - Enemy
        int index = 0;
        _checkpointEnemyPos = vector<Vec2>();
        for (int i = 0; i < enemyPos1D.size(); i += 2) {
            _checkpointEnemyPos.push_back(Vec2(enemyPos1D[index], enemyPos1D[index + 1]));
        }

        // JSON - Checkpoint
        _checkpointActivatedCheckpoints = activatedcheckpts1D;

        // JSON - Reynard
        _checkpointReynardPos = Vec2(reynardPos1D[0], reynardPos1D[1]);

        // JSON - Room Swapping
        std::shared_ptr<JsonValue> jsonRoomSwap = JsonValue::alloc(JsonValue::Type::ObjectType);
        jsonRoomSwap->initArray();
        _checkpointSwapLen = static_cast<int>(swapHistory1D.size() / 4);
        int hisotoryLen = static_cast<int>(swapHistory1D.size());
        _swapHistory = vector<vector<Vec2>>();
        for (int i = 0; i < hisotoryLen; i += 4) {
            vector<Vec2> thisSwap = vector<Vec2>();
            thisSwap.push_back(Vec2(swapHistory1D[i], swapHistory1D[i + 1]));
            thisSwap.push_back(Vec2(swapHistory1D[i + 2], swapHistory1D[i + 3]));
            _swapHistory.push_back(thisSwap);
        }

        return true;
    }

protected:
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
    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle> &obj,
            const std::shared_ptr<cugl::scene2::SceneNode> &node);

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
    bool init(const std::shared_ptr<cugl::AssetManager> &assets);

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
    bool init(const std::shared_ptr<cugl::AssetManager> &assets, const cugl::Rect rect);

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
    bool init(const std::shared_ptr<cugl::AssetManager> &assets, const cugl::Rect rect, const cugl::Vec2 gravity);


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
        _winNode->setVisible(value);
        //_health->setVisible(!value);
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
    void render(const std::shared_ptr<SpriteBatch> &batch);


    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

    /**
     * Resets the status of the game so that we can play again.
     */
    void revert(bool totalReset);


#pragma mark -
#pragma mark Collision Handling

    /**
     * Helper function that checks if fixture is a Character Ground fixture
     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  fixture  Fixture on body that made contact
     */
    bool isCharacterGroundFixture(b2Fixture *fixture);

    /**
     * Helper function that checks if fixture is a Character Right fixture
     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  fixture  Fixture on body that made contact
     */
    bool isCharacterRightFixture(b2Fixture *fixture);

    /**
     * Helper function that checks if fixture is a Character Right fixture
     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  fixture  Fixture on body that made contact
     */
    bool isCharacterLeftFixture(b2Fixture *fixture);

    /**
     * Helper function that checks if fixture is an Enemy Detect fixture
     *
     * The enemy detect fixture fires off a contact event when Reynard enters
     * the enemies "sight" i.e some range set by the Enemy Detect fixture
     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  fixture  Fixture on body that made contact
     */
    bool isEnemyDetectFixture(b2Fixture *fixture);


    /**
     * Helper function that returns the fixture associated with Reynard
     * on a contact event
     *
     * This function ASSUMES you have checked that Reynard was involved in
     * the collision. If he wasn't, this function will not error out and return an incorrect
     * fixture
     *
     * The function will return a Fixture object associated with Reynard
     *
     * @param  contact  Contact event generated by beginContact / endContact callbacks
     */
    b2Fixture *getReynardFixture(b2Contact *contact);

    /**
     * Helper function that returns the fixture associated with the Enemy
     * on a contact event
     *
     * This function ASSUMES you have checked that an enemy was involved in
     * the collision. If he wasn't, this function will not error out and return an incorrect
     * fixture
     *
     * The function will return a Fixture object associated with the Enemy
     *
     * @param  contact  Contact event generated by beginContact / endContact callbacks
     */
    b2Fixture *getEnemyFixture(b2Contact *contact);

    /**
     * Detects if a collision includes a trap object, and if so returns the trap type
     *
     * @param  contact  The two bodies that collided
     *
     * @return  trap type if one body is a trap
                or UNTYPED if neither body is a trap
     */
    shared_ptr<TrapModel> isTrapCollision(b2Contact *contact);

    /**
     * Detects if a collision includes a tutorial object, and if so returns the tutorial's pointer
     *
     * @param  contact  The two bodies that collided
     *
     * @return  trap type if one body is a trap
                or UNTYPED if neither body is a trap
     */
    shared_ptr<Tutorial> isTutorialCollision(b2Contact *contact);


    /**
     * Helper function that checks if a contact event includes Reynard
     *
     * Neccesary to call this before calling getReynardFixture.
     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  contact  Contact event generated by beginContact / endContact callbacks
     */
    bool isReynardCollision(b2Contact *contact);

    /**
     * Returns whether the given body belongs to a character in the scene.
     *
     * @param body  The body to check against characters in the scene
     * @return      Whether the given body belongs to a character in the scene
     */
    bool isCharacterBody(b2Body *body);

    /**
     * Returns a pointer to the character's body if the collision involved a character
     * and a non-character object.
     *
     * @param contact   Contact event generated by beginContact / endContact callbacks
     * @return          Pointer to the character's body, or nullptr if collision isn't character-on-object
     */
    b2Body *getCharacterBodyInObjectCollision(b2Contact *contact);

    /**
     * Returns a pointer to the relevant enemy controller if it is involved in the collision; otherwise
     * returns nullptr.
     *
     * This assumes that the given body has already been confirmed to be a character. Thus, if nullptr
     * is returned, that means that the body must belong to Reynard (but it's easier to get a pointer to
     * Reynard than it is to get it to the right enemy, and we can't just return it here because of
     * template issues).
     *
     * @param body  The body of the character to get the controller for
     * @return      Pointer to the enemy controller if it's in the collision, or nullptr otherwise
     */
    shared_ptr<EnemyController> getEnemyControllerInCollision(b2Contact *contact);

    /**
     * Helper function that checks if a contact event is a Reynard <> Wall contact
     *     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  contact  Contact event generated by beginContact / endContact callbacks
     * @param  reynardIsRight  Boolean corresponding to the direction Reynard is facing
     */
    bool isThisAReynardWallContact(b2Contact *contact, bool reynardIsRight);

    /**
     * Helper function that checks if a contact event is a Reynard <> Ground contact
     *     *
     * The function will return true if it is the case and false otherwise.
     *
     * @param  contact  Contact event generated by beginContact / endContact callbacks
     */
    bool isThisAReynardGroundContact(b2Contact *contact);

    /**
     * Resolver function that fires when Reynard makes contact with a wall
     */
    void resolveReynardWallOnContact();

    /**
     * Resolver function that fires when Reynard makes contact with the ground
     */
    void resolveReynardGroundOnContact();

    void resolveEnemyGroundOffContact(shared_ptr<EnemyController> enemy);


    /**
     * Resolver function that fires when Reynard ends contact with the ground
     */
    void resolveReynardGroundOffContact();

    /**
     * Resolver function that fires when Reynard makes contact with a trap
     */
    void resolveTrapOnContact();

    /**
     * Resolver function that fires when Reynard makes contact with a trap
     * but only in the case where he is wall jumping through a trap (from underneath)
     *
     * @param  reynardVY  Reynard's Y velocity (we expect it to be negative)
     */
    void resolveWallJumpOntoTrap(float reynardVY);

    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects.  We use
     * this method to test if it is the "right" kind of collision and to execute resolvers when
     * needed.
     *
     * @param  contact  The two bodies that collided
     */
    void beginContact(b2Contact *contact);

    /**
     * Processes the end of a collision
     *
     * This method is called when we end a collision between two objects.  We use
     * this method to reset resolvers.
     *
     * @param  contact  The two bodies that have ended colliding
     */
    void endContact(b2Contact *contact);

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
    void beforeSolve(b2Contact *contact, const b2Manifold *oldManifold);

    void resolveEnemyWallJumpOntoTrap(float enemyVY, shared_ptr<EnemyController> enemy);

    void resolveEnemyTrapOnContact(shared_ptr<EnemyController> enemy);

    void resolveEnemyWallOnContact(shared_ptr<EnemyController> enemy);

    bool isThisAEnemyWallContact(b2Contact *contact, bool enemyIsRight, shared_ptr<EnemyController> enemy);

    bool isThisAEnemyGroundContact(b2Contact *contact, shared_ptr<EnemyController> enemy);

    void resolveEnemyGroundOnContact(shared_ptr<EnemyController> enemy);

    void dealReynardDamage();


#pragma mark Helper Functions

    /* Converts input coordinates to coordinates in the game world */
    Vec2 inputToGameCoords(Vec2 inputCoords);

    /**
     * Create a key at the given location in PHYSICS space
     */
    void createKey(Vec2 pos, bool isPossesed, bool isPathFinding);

    void createKeyCrazy(Vec2 enemyPos);

    /**
     * Mark a key to be removed from the game during the next update
     */
    void removeKey(shared_ptr<CheckpointKey> k);

    /**
     * Mark a possessed key to be removed from the game during the next update
     */
    void removeKeyCrazy(shared_ptr<CheckpointKeyCrazy> k);

};

#endif /* __MP_GAME_MODE_H__ */
