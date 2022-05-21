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
#include "MPGameScene.h"

#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

using namespace cugl;
using namespace std;

/** Whether or not the game will even bother loading from a save */
#define LOAD_FROM_SAVE 0

#pragma mark -
#pragma mark Level Geography

/** This is the size of the active portion of the screen */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH 32.0f

/** Height of the game world in Box2d units */
float DEFAULT_HEIGHT = DEFAULT_WIDTH / SCENE_WIDTH * SCENE_HEIGHT;

/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -26.0f

/** The default value of Spike damage */
#define SPIKE_DAMAGE 1.0f

/** To automate the loading of crate files */
#define NUM_CRATES 2

#pragma mark Assset Constants
/** The key for the some texture in the asset manager {@example}*/
#define SOME_TEXTURE "sometexturename"

/** Color to outline the physics nodes */
#define STATIC_COLOR Color4::WHITE
/** Opacity of the physics outlines {@example} */
#define SOME_COLOR Color4::YELLOW

/** The key for the font reference */
#define PRIMARY_FONT "retro"

float REYNARD_POS[] = {30, 10};

#pragma mark Physics Constants

#pragma mark -
#pragma mark Constructors

/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : cugl::Scene2(),
                         _complete(false),
                         _debug(false)
{
}

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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets)
{
    return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY));
}

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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets, const Rect rect)
{
    return init(assets, rect, Vec2(0, DEFAULT_GRAVITY));
}

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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets, const Rect rect, const Vec2 gravity)
{
    Size dimen = computeActiveSize();
    _checkpointReynardPos = REYNARD_START;
    if (assets == nullptr)
    {
        return false;
    }
    else if (!Scene2::init(dimen))
    {
        return false;
    }

    // Start up the input handler
    _assets = assets;
    _input.init();

    TrapModel::ASSETS = _assets;

    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect, gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact *contact)
    {
        beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact *contact)
    {
        endContact(contact);
    };

    _world->beforeSolve = [this](b2Contact *contact, const b2Manifold *oldManifold)
    {
        beforeSolve(contact, oldManifold);
    };

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world. Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    // Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f); //BUGGY

    // CULog("Size: %f %f", getSize().width, getSize().height);
    //  Create the scene graph
    _worldnode = scene2::ScrollPane::allocWithBounds(10, 10); // Number does not matter when constraint is false
    _worldnode->setMinZoom(0.5);
    _debugnode = scene2::ScrollPane::allocWithBounds(10, 10); // Number does not matter when constraint is false
    _debugnode->setScale(_scale);
    _debugnode->setMinZoom(0.5);// Debug node draws in PHYSICS coordinates
    setDebug(false);

    _winNode = scene2::Label::allocWithText("VICTORY!", _assets->get<Font>(PRIMARY_FONT));
    _winNode->setAnchor(Vec2::ANCHOR_CENTER);
    _winNode->setBackground(Color4::BLACK);
    _winNode->setForeground(STATIC_COLOR);
    _winNode->setPadding(dimen.width / 2, dimen.height / 2, dimen.width / 2, dimen.height / 2);
    setComplete(false);

    _health = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("healthbar"));
    _health->setAnchor(Vec2::ANCHOR_TOP_LEFT);
    Vec2 padding = Vec2(30, -20);
    _health->setPosition(Vec2(0, getSize().height) + padding);
    _health->setScale(0.75);

    _keyUI = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("keys_none.png"));
    _keyUI->setAnchor(Vec2::ANCHOR_TOP_LEFT);
    padding = Vec2(30, -80);
    _keyUI->setPosition(Vec2(0, getSize().height) + padding);
    _keyUI->setScale(0.75);

    _pause = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("pause"));
    _pause->setAnchor(Vec2::ANCHOR_TOP_LEFT);
    padding = Vec2(computeActiveSize().width - 100, -10);
    _pause->setPosition(Vec2(0, getSize().height) + padding);
    _pause->setScale(0.3);

    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_health);
    addChild(_keyUI);

    addChild(_pause);

    addChild(_winNode);

    // Give all enemies a reference to the ObstacleWorld for raycasting
    EnemyController::setObstacleWorld(_world);
    if (_mode == 1)
    {

        vector<std::string> file_path_list = vector<std::string>(2);
        file_path_list[0] = Application::get()->getSaveDirectory();
        file_path_list[1] = "state.json";
        if (filetool::file_exists(cugl::filetool::join_path(file_path_list)))
        {
            cugl::filetool::file_delete(cugl::filetool::join_path(file_path_list));
        }

        populate();
    }
    else
    {
        revert(false);
    }

    _active = true;
    _complete = false;

    // XNA nostalgia
    Application::get()->setClearColor(Color4f::BLACK);

    // Finally start playing level music
    AudioController::playMusic(LEVEL_MUSIC);

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose()
{
    if (_active)
    {
        removeAllChildren();
        _input.dispose();
        _world = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winNode = nullptr;
        _health = nullptr;
        _keyUI = nullptr;
        _pause = nullptr;
        _complete = false;
        _debug = false;
        Scene2::dispose();
    }
}

#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset()
{
    revert(true);
}

/**
 *
 * @param totalReset Whether or not to start the game from the beginning
 */
void GameScene::revert(bool totalReset)
{
    if (_envController != nullptr)
    {
        _swapHistory = _envController->getSwapHistory();
    }

    totalReset = !readSaveFile();
    scrollingOffset = Vec2();

    _reynardController = nullptr;
    _grid = nullptr;
    _envController = nullptr;
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    _gamestate.reset();
    _enemies = nullptr;
    setComplete(false);
    if (totalReset)
    {
        populate();
    }
    else
    {
        populateEnv();
        populateChars();
        for (int i = 0; i < _checkpointSwapLen; i++)
        {
            _envController->swapRoomOnGrid(_swapHistory[i][0], _swapHistory[i][1], true);
        }
        _reynardController->getCharacter()->setPosition(_checkpointReynardPos);
        for (int i = 0; i < _enemies->size(); i++)
        {
            (*_enemies)[i]->getCharacter()->setPosition(_checkpointEnemyPos[i]);
        }
        for (int index : _checkpointActivatedCheckpoints)
        {
            _envController->getGrid()->getCheckpoints()[index]->setTrapState(TrapModel::TrapState::ACTIVATED);
            _grid->clearCheckpoint(_envController->getGrid()->getCheckpoints()[index]->getID());
            _envController->getGrid()->getCheckpoints()[index]->unlock();
        }
    }
}

/**
 * Lays out the game geography.
 *
 * Pay close attention to how we attach physics objects to a scene graph.
 * The simplest way is to make a subclass, like we do for the rocket.  However,
 * for simple objects you can just use a callback function to lightly couple
 * them.  This is what we do with the crates.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */
void GameScene::populate()
{
    populateEnv();
    populateChars();
}

void GameScene::populateEnv()
{
    _envController = make_shared<EnvController>();
#pragma mark Rooms
    _grid = _envController->getGrid();
    _grid->init(_assets, _scale);

    _worldnode->addChild(_grid);
    _grid->setScale(0.4);

    //_grid->setPosition(0,-240);

    // Populate physics obstacles for grid
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> physics_objects = _grid->getPhysicsObjects();
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = physics_objects->begin(); itr != physics_objects->end(); ++itr)
    {
        _world->addObstacle(*itr);
        (*itr)->setDebugScene(_debugnode);
        (*itr)->setDebugColor(Color4::RED);
        // CULog("populate: %f %f ", (*itr)->getPosition().x);
    }

    // Start the camera above Reynard and pan to him
    _worldnode->applyPan(Vec2(0, -500));

    // Add tutorial icons
    populateTutorials();

    // POPULATE KEYS
    // Do regular keys first
    Vec2 keyCoords;

    auto keyItr = _grid->_loneKeyLocs->begin();
    while (keyItr != _grid->_loneKeyLocs->end()) {
        // Note that these are in HOUSE space, so first go to ROOM? space
        keyCoords.x = (*keyItr).x - _grid->getOriginX() + 0.5f;
        keyCoords.y = (*keyItr).y - _grid->getOriginY() + 0.5f;

        CULog("KEY: (%f, %f)", keyCoords.x, keyCoords.y);
        // Then ROOM to GRID space?
        keyCoords = _grid->roomSpaceToGrid(keyCoords);
        // Then go from GRID space to WORLD space
        keyCoords = _grid->nodeToWorldCoords(keyCoords);
        // Then go to PHYSICS space
        keyCoords /= _scale;

        // Create key with transformed coordinates
        createKey(keyCoords, false, false);

        ++keyItr;
    }
}

/**
 * Places all the characters, including Reynard and enemies, in the game world.
 */
void GameScene::populateChars()
{
    populateReynard();
    populateEnemies();
}

/**
 * Places Reynard in the game world.
 */
void GameScene::populateReynard()
{
    Vec2 pos = _checkpointReynardPos;

    Vec2 temp = pos;
    temp *= _scale; // To world
    // Now world to grid
    temp = _grid->worldSpaceToRoom(temp);
    //CULog("Reynard Pos: (%f, %f)", temp.x, temp.y);

    shared_ptr<Animation> reynard_animations = make_shared<Animation>(_assets->get<Texture>("reynard_all"), _assets->get<JsonValue>("framedata2")->get("reynard"));
    // initialize reynardController with the final animation map
    _reynardController = ReynardController::alloc(pos, _scale, reynard_animations);

    // Add Reynard to physics world
    Vec2 pos_temp = _reynardController->getCharacter()->getPosition();
    _reynardController->getCharacter()->setPosition(Vec2(4, 3));
    addObstacle(_reynardController->getCharacter(), _reynardController->getCharacter()->_node); // Put this at the very front
    _reynardController->getCharacter()->setPosition(pos_temp);
}

/**
 * Places all the enemies for the active regions in the game world.
 */
void GameScene::populateEnemies()
{
    /*shared_ptr<vector<shared_ptr<RegionModel>>> _activeRegions = _grid->getActiveRegions();
    for (vector<shared_ptr<RegionModel>>::iterator itr = _activeRegions->begin();
         itr != _activeRegions->end(); ++itr)
    {
        populateEnemiesInRegion(*itr);
    }*/


    shared_ptr<Animation> rabbit_animations = make_shared<Animation>(_assets->get<Texture>("rabbit_all"), _assets->get<JsonValue>("framedata2")->get("rabbit"));

    // Initialize new enemy
    _enemies = make_shared<vector<std::shared_ptr<EnemyController>>>();

    Vec2 enemyCoords;

    // For each enemy to spawn
    for (auto itr = _grid->_enemySpawnInfo->begin(); itr != _grid->_enemySpawnInfo->end();
        ++itr) {
        // Note that these are in HOUSE space, so first go to ROOM? space
        enemyCoords.x = ((*itr).first).x - _grid->getOriginX() + 0.5f;
        enemyCoords.y = ((*itr).first).y - _grid->getOriginY() + 0.5f;
        // Then ROOM to GRID space?
        enemyCoords = _grid->roomSpaceToGrid(enemyCoords);
        // Then go from GRID space to WORLD space
        enemyCoords = _grid->nodeToWorldCoords(enemyCoords);
        // Then go to PHYSICS space
        enemyCoords /= _scale;

        // Now create the actual enemy

        // initialize it
        //                _enemies->push_back(EnemyController::alloc(enemypos * Vec2(12,8), _scale, rabbit_animations));
        _enemies->push_back(EnemyController::alloc(Vec2::ZERO, _scale, rabbit_animations));

        _enemies->back()->setObstacleWorld(_world);
        _enemies->back()->setReynardController(_reynardController);
        _enemies->back()->_isKeyed = (*itr).second;

        //addObstacle(_enemies->back()->getCharacter(), _enemies->back()->getCharacter()->_node);

        //_enemies->back()->getCharacter()->setPosition(enemyCoords);
    }

    _checkpointEnemyPos = vector<Vec2>();
    _checkpointReynardPos = _reynardController->getCharacter()->getPosition();
    for (auto enemy : *_enemies)
    {
        _checkpointEnemyPos.push_back(enemy->getCharacter()->getPosition());
    }
}

/**
 * NOT CURRENTLY USED
 * 
 * Places the enemies for the given region in the game world.
 *
 * Allows us to populate enemies on a per-region basis, instead
 * of loading them all in at once and potentially causing runtime
 * issues.
 *
 * @param region    The region to populate the enemies for.
 */
void GameScene::populateEnemiesInRegion(shared_ptr<RegionModel> region)
{

    // NOT CURRENTLY USED

    shared_ptr<Animation> rabbit_animations = make_shared<Animation>(_assets->get<Texture>("rabbit_all"), _assets->get<JsonValue>("framedata2")->get("rabbit"));

    // Initialize new enemy
    _enemies = make_shared<vector<std::shared_ptr<EnemyController>>>();


    // get Level data from the JSON
    shared_ptr<JsonValue> levelJSON = _assets->get<JsonValue>(region->getName());
    // get the layer containing entities
    shared_ptr<JsonValue> entityLayer;
    for (int i = 0; i < levelJSON->get("layers")->size(); i++)
    {
        if (levelJSON->get("layers")->get(i)->get("name")->asString() == "entities")
        {
            entityLayer = levelJSON->get("layers")->get(i);
        }
    }
    // get the entity tileset offset from levelJSON
    int entity_offset;
    for (int i = 0; i < levelJSON->get("tilesets")->size(); i++)
    {
        if (levelJSON->get("tilesets")->get(i)->get("source")->asString().find("entities") != string::npos)
        {
            entity_offset = levelJSON->get("tilesets")->get(i)->get("firstgid")->asInt();
        }
    }

    // For each tile in the entity layer,
    for (int i = 0; i < entityLayer->get("data")->size(); i++)
    {

        // if there is something in the tile
        if (entityLayer->get("data")->get(i)->asInt() != 0)
        {
            int tile = entityLayer->get("data")->get(i)->asInt() - entity_offset;

            // determine if the entity corresponds to an enemy
            shared_ptr<JsonValue> temp1 = _assets->get<JsonValue>("tileset_entities")->get("tiles");
            string temp = temp1->get(tile - temp1->get(0)->get("id")->asInt())->get("image")->asString();

            // if it is an enemy...
            // TODO: change code in this if statement to add key enemy instead of regular enemy
            if (temp.find("keyenemy") != string::npos)
            {
                // Get enemy location, converting from tile space
                int room = i / ROOM_HEIGHT / ROOM_WIDTH;
                int x = room % (levelJSON->get("width")->asInt() / ROOM_WIDTH);
                int y = room / (levelJSON->get("width")->asInt() / ROOM_WIDTH);
                Vec2 enemypos = Vec2(x, levelJSON->get("height")->asInt() / ROOM_HEIGHT - 1 - y);

                // initialize it
                //                _enemies->push_back(EnemyController::alloc(enemypos * Vec2(12,8), _scale, rabbit_animations));
                _enemies->push_back(EnemyController::alloc(Vec2::ZERO, _scale, rabbit_animations));

                _enemies->back()->setObstacleWorld(_world);
                _enemies->back()->setReynardController(_reynardController);
                addObstacle(_enemies->back()->getCharacter(), _enemies->back()->getCharacter()->_node);

                _enemies->back()->getCharacter()->setPosition((enemypos + Vec2::ZERO) * Vec2(23, 12));
            }
            else if (temp.find("enemy") != string::npos)
            {

                // Get enemy location, converting from tile space
                int room = i / ROOM_HEIGHT / ROOM_WIDTH;
                int x = room % (levelJSON->get("width")->asInt() / ROOM_WIDTH);
                int y = room / (levelJSON->get("width")->asInt() / ROOM_WIDTH);
                Vec2 enemypos = Vec2(x, levelJSON->get("height")->asInt() / ROOM_HEIGHT - 1 - y);

                // initialize it
                //                _enemies->push_back(EnemyController::alloc(enemypos * Vec2(ROOM_WIDTH,ROOM_HEIGHT), _scale, rabbit_animations));
                _enemies->push_back(EnemyController::alloc(Vec2::ZERO, _scale, rabbit_animations));

                _enemies->back()->setObstacleWorld(_world);
                _enemies->back()->setReynardController(_reynardController);
                addObstacle(_enemies->back()->getCharacter(), _enemies->back()->getCharacter()->_node);

                //_enemies->back()->getCharacter()->setPosition((enemypos + Vec2::ZERO) * Vec2(20, 14));
                _enemies->back()->getCharacter()->setPosition((enemypos + Vec2(1, 1)) * Vec2(8.2, 5.1));
            }
        }
    }

    // Initialize EnemyController with the final animation map and store in vector of enemies
    //_enemies->push_back(EnemyController::alloc(Vec2(3, 3), _scale, rabbit_animations));

    //        for(shared_ptr<EnemyController> enemy : *_enemies){
    //            enemy->setObstacleWorld(_world);
    //            enemy->setReynardController(_reynardController);
    //            addObstacle(enemy->getCharacter(), enemy->getCharacter()->_node); // Put
    //        }

    _checkpointEnemyPos = vector<Vec2>();
    _checkpointReynardPos = _reynardController->getCharacter()->getPosition();
    for (auto enemy : *_enemies)
    {
        _checkpointEnemyPos.push_back(enemy->getCharacter()->getPosition());
    }
}

/**
 * Place all the tutorials at their correct locations in Region 1.
 */
void GameScene::populateTutorials()
{
    // First Jump
    placeEnvImage(1.1f, 1.7f, 0.1f, "JumpTutorial");

    // Wall Jump
    placeEnvImage(2.2f, 2, 0.1f, "JumpTutorial");
    placeEnvImage(1.55f, 2.5f, 0.1f, "JumpTutorial");
    placeEnvImage(2.2f, 2.9f, 0.1f, "JumpTutorial");

    // Top Left
    placeEnvImage(2.63f, 4.1f, 0.1f, "ZoomOutTutorial");
    placeEnvImage(1.3f, 4.3f, 0.25f, "SwapTutorial");
    placeEnvImage(1.9f, 4.1f, 0.1f, "ZoomInTutorial");

    placeEnvImage(7.75f, 4.8f, 0.1f, "DashTutorial");
}

/**
 * Helper function that creates, transforms, and places the node for an environment image
 *
 * @param x     x-coordinate in ROOM coordinates
 * @param y     y-coordinate in ROOM coordinates
 */
void GameScene::placeEnvImage(float x, float y, float scale, string TextureName)
{
    y += TUTORIAL_Y_OFFSET;

    // Make scene node for the given tutorial image
    shared_ptr<scene2::PolygonNode> tutorialNode =
        scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(TextureName));

    // Scale and place node
    tutorialNode->setScale(scale);
    tutorialNode->setPosition(x * ROOM_WIDTH * _scale, y * ROOM_HEIGHT * _scale);
    tutorialNode->setAbsolute(true);

    // Attach to the world node
    _worldnode->addChild(tutorialNode);
}

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
void GameScene::addObstacle(const std::shared_ptr<physics2::Obstacle> &obj,
                            const std::shared_ptr<scene2::SceneNode> &node)
{
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);
    obj->setDebugColor(Color4::RED);

    // Position the scene graph node (enough for static objects)
    _worldnode->addChild(node);
    node->setPosition(obj->getPosition() * _scale);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody)
    {
        scene2::SceneNode *weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle *obs)
                         {
            weak->setPosition(obs->getPosition() * _scale);
            weak->setAngle(obs->getAngle()); });
    }
}

#pragma mark -
#pragma mark Physics Handling

float frameAcc = 0;
int fps = 0;

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
void GameScene::update(float dt)
{
    _input.update(dt);
    Vec2 inputPos = inputToGameCoords(_input.getPosition());

    _envController->getGrid()->update(dt);

    _world->garbageCollect();

    // Clear regions if we hit the debug key
    if (_input.didClearRegion1()) {
        _grid->clearRegion(1);
    }
    if (_input.didClearRegion2()) {
        // Actually region 3, "region 2" is the first transitional region
        _grid->clearRegion(3);
    }

    // Process the toggled key commands
    if (_input.didDebug())
    {
        setDebug(!isDebug());
        //_worldnode->setVisible(!_worldnode->isVisible());
        vector<std::shared_ptr<EnemyController>>::iterator itr;
        for (itr = _enemies->begin(); itr != _enemies->end(); ++itr)
        {
            (*itr)->setDebug(true);
        }
    }

    // Reset Process toggled by key command
    if (_input.didReset())
    {
        reset();
    }
    // Exit Process toggled by key command
    if (_input.didExit())
    {
        CULog("Shutting down");
        Application::get()->quit();
    }

    // If a key enemy died since last frame and needs to spawn a key
    if (deadKeyEnemyLocs->size() > 0) {
        // Create a key object and place at enemy position
        // int v2 = rand() % 100 + 1;
        auto itr = deadKeyEnemyLocs->begin();
        int v2 = 51;
        while (itr != deadKeyEnemyLocs->end()) {
            if (v2 > 50) {
                //CULog("TRYING TO MAKE KEY");
                createKey(*itr, false, true);
            }
            else {
                Vec2 reyPos = _reynardController->getCharacter()->getPosition();
                createKey(Vec2(reyPos.x + 3, reyPos.y), true, true);
            }
            // Untrack now that it's been spawned
            itr = deadKeyEnemyLocs->erase(itr);
        }
    }

    if (_keys.size() > 0) {
        for (int i = 0; i < _keys.size(); i++){
            shared_ptr<CheckpointKey> k = _keys.at(i);
            if (k != nullptr && k->isPathFinding()) {
                Vec2 reyPos = _reynardController->getCharacter()->getPosition();
                Vec2 currPos = k->getPosition();
                float x = reyPos.x - currPos.x;
                float y = reyPos.y - currPos.y;
                k->setVX(x);
                k->setVY(y);
            }
        }
    }

    // Spawn key to chase Reynard
    if (_keysCrazy.size() > 0) {
        for (int i = 0; i < _keysCrazy.size(); i++){
            shared_ptr<CheckpointKeyCrazy> k = _keysCrazy.at(i);
            if (k != nullptr && k->isPathFinding()) {
                Vec2 reyPos = _reynardController->getCharacter()->getPosition();
                bool isFacingRight = _reynardController->getCharacter()->isFacingRight();
                Vec2 currPos = k->getPosition();
                //_keyCrazy->setVX(3.0f);
                float x = reyPos.x - currPos.x;
                float y = reyPos.y - currPos.y;
                if (isFacingRight) {
                    k->setVX(-x);
                    k->setVY(-y);
                }
                else {
                    k->setVX(x);
                    k->setVY(y);
                }
            }
        }
    }

    // reynard red when hurt/dealt damage
    if (keepRedFrames > 0)
    {
        // keep time (frame)
        keepRedFrames -= 1;
    }
    else
    {
        // restore
        _reynardController->getSceneNode()->setColor(Color4::WHITE);
    }

    // Variables to indicate which forms of room swap are being used
    bool usingClick = true;
    bool usingDrag = true;

    bool triedSwap = false;
    Vec2 progressCoords = Vec2(-1, -1);
    // Room swap by click
    if (_input.didPress() && (_gamestate.secondsAfterResume() > 3))
    {
        Vec2 node_coord = _pause->screenToNodeCoords(_input.getPosition());
        if ((node_coord - Vec2(123, 123)).length() < 150)
        {
            _gamestate.pauseSwitch();

        }
    }
    if (usingClick && !_gamestate.zoomed_in() && _input.didPress())
    {
        if (_envController->hasSelected())
        {
            if (_envController->swapWithSelected(inputPos, _reynardController, _enemies))
            {
                AudioController::playSFX(SWAP_SOUND);
            }
            else
            {
                AudioController::playSFX(NOSWAP_SOUND);
            }
            triedSwap = true;
        }
        else
        {
            _envController->selectRoom(inputPos, _reynardController, _enemies);
        }
    }
    if (_gamestate.isPaused()){
        _pause->setTexture("textures/PauseScreen/Play_Button.png");
    }
    if (_gamestate.secondsAfterResume() < 1)
    {
        _pause->setTexture("textures/PauseScreen/Pause_Count_Down_3.png");
    }
    if (_gamestate.secondsAfterResume() < 2)
    {
        _pause->setTexture("textures/PauseScreen/Pause_Count_Down_2.png");
    }
    if (_gamestate.secondsAfterResume() < 3)
    {
        _pause->setTexture("textures/PauseScreen/Pause_Count_Down_1.png");
    }
    if (_gamestate.secondsAfterResume() < 4)
    {
        _pause->setTexture("textures/PauseScreen/Pause_Button.png");
    }
    if (_gamestate.isPaused() || _gamestate.secondsAfterResume() < 3)
        return;

    // Room swap by drag
    if (usingDrag && !_gamestate.zoomed_in())
    {
        if (_input.didPress() && !triedSwap)
        {
            _envController->selectRoom(inputPos, _reynardController, _enemies);
        }
        else if (_input.didEndDrag() && _envController->hasSelected())
        {
            if (_envController->swapWithSelected(inputPos, _reynardController, _enemies))
            {
                AudioController::playSFX(SWAP_SOUND);
            }
            else
            {
                AudioController::playSFX(NOSWAP_SOUND);
            }
        }
        if (_input.isDragging() && _envController->hasSelected())
        {
            progressCoords = inputPos;
        }
    }

    if ((_input.isScrolling()||(_gamestate.isSuperZoomOut()&& _input.isDragging()))&& !_gamestate.zoomed_in())
    {
        Vec2 incrementalOffset = _input.getPosition() - lastFramePos;
        scrollingOffset = scrollingOffset + Vec2(incrementalOffset.x, -incrementalOffset.y);
    }
    if (_gamestate.zoomed_in())
    {
        scrollingOffset = Vec2();
        lastFramePos = Vec2();
    }

    // Only allow jumping while zoomed in
    if (_input.didJump() && _gamestate.zoomed_in())
    {
        bool i = _reynardController->isGrounded();
        bool j = _reynardController->getCharacter()->isOnWall();

        if (i || j)
        {
            AudioController::playSFX(JUMP_SOUND);
        }

        _reynardController->jump();
        corner_num_frames_workaround = 0;
        // cout << "Press Jump Button" << endl;
        // CULog("jumpin");
    }

    // If dash was pressed
    else if (_input.getDashDirection() != 0 && _gamestate.zoomed_in())
    {
        _reynardController->dash(_input.getDashDirection());
    }

    // When zooming in
    if (_input.didZoomIn())
    {
        if (!_gamestate.isPaused())
        {
            if (!_gamestate.zoomed_in())
            {
                AudioController::playSFX(ZOOMIN_SOUND);
            }
            _gamestate.zoom_in(_worldnode->getZoom());
        }
        _envController->zoomIn();
    }

    if (_reynardController->getCharacter()->getHearts() <= 0)
    {
        revert(false);
        return;
    }

    // When zooming out
    if (_input.didZoomOut())
    {
        if (!_gamestate.isPaused())
        {
            if (_gamestate.zoomed_in())
            {
                AudioController::playSFX(ZOOMOUT_SOUND);
            }
            _gamestate.zoom_out(_worldnode->getZoom());
        }
        _envController->zoomOut();
    }

    float scaled_dt = _gamestate.getScaledDtForPhysics(dt);
    // TODO: Why does both these updates exist you only need the _world one
    _reynardController->update(scaled_dt);
    _world->update(scaled_dt);
    _world->garbageCollect();

    // TODO debugging area. Disable for releases
    if ((!_reynardController->getCharacter()->isOnWall()) && abs(_reynardController->getCharacter()->getLinearVelocity().x) <= 0.5)
    {
        //CULog("likely Error 01: Reynard stuck. See MPGameScene.c update() and breakpoint here");
        _reynardController->getCharacter()->setLinearVelocity((3.7 * (_reynardController->getCharacter()->isFacingRight() ? 1 : -1)), 0.1);
    }
    else if (abs(_reynardController->getCharacter()->getLinearVelocity().x) + abs(_reynardController->getCharacter()->getLinearVelocity().y) == 0)
    {
        corner_num_frames_workaround += 1;
        if (corner_num_frames_workaround > 2)
        {
            _reynardController->getCharacter()->setLinearVelocity((3.7 * (_reynardController->getCharacter()->isFacingRight() ? 1 : -1)), 0);
            corner_num_frames_workaround = 0;
            _reynardController->getCharacter()->setMoveState(CharacterModel::MovementState::RUNNING);
        }
    }
    if (_reynardController->getCharacter()->isJumping() && abs(_reynardController->getCharacter()->getLinearVelocity().x) < 7)
    {
        // CULog("likely Error 02: Reynard jumping slow. See MPGameScene.c update() and breakpoint here");
    }

    // Camera following reynard, with some non-linear smoothing
    Vec2 currentTranslation = _worldnode->getPaneTransform().getTranslation();
    Vec2 reynardScreenPosition = _worldnode->getPaneTransform().transform(_reynardController->getSceneNode()->getPosition());

    bool faceRight = _reynardController->getCharacter()->isFacingRight();
    Vec2 reynardVelocity = _reynardController->getCharacter()->getLinearVelocity();

    _worldnode->applyPan(_gamestate.getPan(currentTranslation, reynardScreenPosition - scrollingOffset, _scale, getSize(), faceRight, reynardVelocity));
    _worldnode->applyZoom(_gamestate.getZoom(_worldnode->getZoom()));

    // Copy World's zoom and transform
    _debugnode->applyPan(-_debugnode->getPaneTransform().transform(Vec2()));
    _debugnode->applyPan(_worldnode->getPaneTransform().transform(Vec2()) / _scale);
    _debugnode->applyZoom(1 / _debugnode->getZoom());
    _debugnode->applyZoom(_worldnode->getZoom());

    // Update all enemies
    vector<std::shared_ptr<EnemyController>>::iterator itr;
    for (itr = _enemies->begin(); itr != _enemies->end(); ++itr)
    {
        (*itr)->update(dt);
    }

    // Update the environment
    _envController->update(progressCoords, !_gamestate.zoomed_in(), _reynardController, _enemies);

    // Update the health UI
    if (_reynardController->getCharacter()->getHearts() >= 3)
    {
        if (_health->getName() != "3")
        {
            _health->setTexture("textures/Health_Bar_Full.png");
            _health->setName("3");
        }
    }
    else if (_reynardController->getCharacter()->getHearts() == 2)
    {
        if (_health->getName() != "2")
        {
            _health->setTexture("textures/Health_Bar_Two_Third.png");
            _health->setName("2");
        }
    }
    else if (_reynardController->getCharacter()->getHearts() == 1)
    {
        if (_health->getName() != "1")
        {
            _health->setTexture("textures/Health_Bar_One_Third.png");
            _health->setName("1");
        }
    }
    else if (_reynardController->getCharacter()->getHearts() <= 0)
    {
        if (_health->getName() != "0")
        {
            _health->setTexture("textures/Health_Bar_None.png");
            _health->setName("0");
        }
    }

    // Update the key UI
    if (_reynardController->getKeysCount() >= 3)
    {
        if (_keyUI->getName() != "3")
        {
            _keyUI->setTexture("textures/keys_three.png");
            _keyUI->setName("3");
        }
    }
    else if (_reynardController->getKeysCount() == 2)
    {
        if (_keyUI->getName() != "2")
        {
            _keyUI->setTexture("textures/keys_two.png");
            _keyUI->setName("2");
        }
    }
    else if (_reynardController->getKeysCount() == 1)
    {
        if (_keyUI->getName() != "1")
        {
            _keyUI->setTexture("textures/keys_one.png");
            _keyUI->setName("1");
        }
    }
    else if (_reynardController->getKeysCount() <= 0)
    {
        if (_keyUI->getName() != "0")
        {
            _keyUI->setTexture("textures/keys_none.png");
            _keyUI->setName("0");
        }
    }
    lastFramePos = _input.getPosition();

}

#pragma mark -
#pragma mark Collision Handling

bool GameScene::isCharacterGroundFixture(b2Fixture *fixture)
{
    return (fixture->GetUserData().pointer == 4);
}

bool GameScene::isCharacterRightFixture(b2Fixture *fixture)
{
    return (fixture->GetUserData().pointer == 5);
}

bool GameScene::isCharacterLeftFixture(b2Fixture *fixture)
{
    return (fixture->GetUserData().pointer == 6);
}

bool GameScene::isEnemyDetectFixture(b2Fixture *fixture)
{
    return (fixture->GetUserData().pointer == 10);
}

b2Fixture *GameScene::getReynardFixture(b2Contact *contact)
{
    // We assume that you have checked to see that at least
    // one of the bodies in the contact event have been
    // verified to be a Reynard object
    // assert(isReynardCollision(contact))
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody() ||
        body2 == _reynardController->getCharacter()->getBody())
    {
        return contact->GetFixtureA();
    }
    else
    {
        return contact->GetFixtureB();
    }
}

b2Fixture *GameScene::getEnemyFixture(b2Contact *contact)
{
    // TODO this function is not right. Also what if both are enemies?
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody() ||
        body2 == _reynardController->getCharacter()->getBody())
    {
        return contact->GetFixtureA();
    }
    else
    {
        return contact->GetFixtureB();
    }
}

/**
* Detects if a collision includes a trap object, and if so returns the trap type
*
* @param  contact  The two bodies that collided
*
* @return  trap type if one body is a trap
        or UNTYPED if neither body is a trap
*/
shared_ptr<TrapModel> GameScene::isTrapCollision(b2Contact *contact)
{
    if (_grid == nullptr)
        return nullptr;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    shared_ptr<RoomModel> room;
    for (int col = 0; col < _grid->getWidth(); col++)
    {
        for (int row = 0; row < _grid->getHeight(); row++)
        {
            room = _grid->getRoom(col, row);
            if (room != nullptr && room->getTrap() != nullptr)
            {
                shared_ptr<TrapModel> _trap = _grid->getRoom(col, row)->getTrap();
                b2Body *body = _trap->getObstacle()->getBody();
                bool isCollision = body == body1 || body == body2;
                if (isCollision)
                    return _trap;
            }
        }
    }
    return nullptr;
}

bool GameScene::isReynardCollision(b2Contact *contact)
{
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    return body1 == _reynardController->getCharacter()->getBody() || body2 == _reynardController->getCharacter()->getBody();
}

/**
 * Returns whether the given body belongs to a character in the scene.
 *
 * @param body  The body to check against characters in the scene
 * @return      Whether the given body belongs to a character in the scene
 */
bool GameScene::isCharacterBody(b2Body *body)
{
    if (_reynardController != nullptr)
    {
        // Return true if it's Reynard
        if (body == _reynardController->getCharacter()->getBody())
            return true;
    }
    // Otherwise check against the enemies and return true if it's one of them
    for (auto itr = _enemies->begin(); itr != _enemies->end(); ++itr)
    {
        if ((*itr)->getCharacter()->getBody() == body)
            return true;
    }

    // If not, return false
    return false;
}

/**
 * Returns a pointer to the character's body if the collision involved a character
 * and a non-character object.
 *
 * @param contact   Contact event generated by beginContact / endContact callbacks
 * @return          Pointer to the character's body, or nullptr if collision isn't character-on-object
 */
b2Body *GameScene::getCharacterBodyInObjectCollision(b2Contact *contact)
{
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();

    //// Only characters have their body user data set to something nonzero
    //// Thus, it's a character-on-object collision if only one of these bodies has nonzero user data
    //// Return nullptr if it's not character-on-object
    // if (!((body1->GetUserData().pointer == 0 && body2->GetUserData().pointer != 0) ||
    //     (body1->GetUserData().pointer != 0 && body2->GetUserData().pointer == 0))) return nullptr;

    //// Return whichever body is the character involved in the collision
    // return body1->GetUserData().pointer == 0 ? body2 : body1;

    // Return nullptr if it's not character-on-object
    if ((isCharacterBody(body1) && isCharacterBody(body2)) || (!isCharacterBody(body1) && !isCharacterBody(body2)))
        return nullptr;

    // Otherwise return whichever one is the character
    return isCharacterBody(body1) ? body1 : body2;
}

/**
 * Returns a pointer to Reynard's controller if he is involved in the collision; otherwise
 * returns nullptr.
 *
 * @param body  The body of the character to get the controller for
 * @return      Pointer to Reynard's controller if he's in the collision, or nullptr otherwise
 */
shared_ptr<EnemyController> GameScene::getEnemyControllerInCollision(b2Contact *contact)
{
    //// Get body user data and convert to BodyData
    // CharacterController<EnemyModel, EnemyController>::BodyData* bodyData =
    //     static_cast<CharacterController<EnemyModel, EnemyController>::BodyData*>
    //     ((void*)body->GetUserData().pointer);

    //// Return nullptr now if it's not for an enemy
    // if (bodyData->_type != CharacterController<EnemyModel, EnemyController>::CharacterType::ENEMY)
    //     return nullptr;

    //// Otherwise, cast the given pointer to an EnemyController pointer and return
    // EnemyController* enemyPtr = static_cast<EnemyController*>(bodyData->_controller);

    // return make_shared<EnemyController>(*enemyPtr);

    // Check body against all enemies in level
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    for (auto itr = _enemies->begin(); itr != _enemies->end(); ++itr)
    {
        if (((*itr)->getCharacter()->getBody() == body1) || ((*itr)->getCharacter()->getBody() == body2))
            return (*itr);
    }

    // If not an enemy, return nullptr
    return nullptr;
}

bool GameScene::isThisAReynardWallContact(b2Contact *contact, bool reynardIsRight)
{
    b2Fixture *reynardFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody())
    {
        reynardFixture = contact->GetFixtureA();
    }
    else
    {
        reynardFixture = contact->GetFixtureB();
    }
    if (reynardIsRight && isCharacterRightFixture(reynardFixture))
    {
        return true;
    }
    else
        return !reynardIsRight && isCharacterLeftFixture(reynardFixture);
}

bool GameScene::isThisAEnemyWallContact(b2Contact *contact, bool enemyIsRight, shared_ptr<EnemyController> enemy)
{
    b2Fixture *enemyFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == enemy->getCharacter()->getBody())
    {
        enemyFixture = contact->GetFixtureA();
    }
    else
    {
        enemyFixture = contact->GetFixtureB();
    }
    if (enemyIsRight && isCharacterRightFixture(enemyFixture))
    {
        return true;
    }
    else
        return !enemyIsRight && isCharacterLeftFixture(enemyFixture);
}

bool GameScene::isThisAEnemyGroundContact(b2Contact *contact, shared_ptr<EnemyController> enemy)
{
    b2Fixture *enemyFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == enemy->getCharacter()->getBody())
    {
        enemyFixture = contact->GetFixtureA();
    }
    else
    {
        enemyFixture = contact->GetFixtureB();
    }
    return isCharacterGroundFixture(enemyFixture);
}

bool GameScene::isThisAReynardGroundContact(b2Contact *contact)
{
    b2Fixture *reynardFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody())
    {
        reynardFixture = contact->GetFixtureA();
    }
    else
    {
        reynardFixture = contact->GetFixtureB();
    }
    return isCharacterGroundFixture(reynardFixture);
}

void GameScene::resolveReynardWallOnContact()
{
    _reynardController->hitWall();
}

void GameScene::resolveEnemyWallOnContact(shared_ptr<EnemyController> enemy)
{
    enemy->hitWall();
}

void GameScene::resolveReynardGroundOnContact()
{
    _reynardController->hitGround();
}

void GameScene::resolveEnemyGroundOffContact(shared_ptr<EnemyController> enemy)
{
    enemy->offGround();
}

void GameScene::resolveEnemyGroundOnContact(shared_ptr<EnemyController> enemy)
{
    enemy->hitGround();
}

void GameScene::resolveReynardGroundOffContact()
{
    _reynardController->offGround();
}

void GameScene::resolveTrapOnContact()
{
    if (_reynardController->canBeHit())
    {
        dealReynardDamage();
    }
}

void GameScene::resolveWallJumpOntoTrap(float reynardVY)
{
    // We expect reynardVY to be a negative value
    // KNOCKBACK
    _reynardController->getCharacter()->setVY(5);
    //    _reynardController->getCharacter()->setVY(-1 * reynardVY);
}

void GameScene::resolveEnemyTrapOnContact(shared_ptr<EnemyController> enemy)
{
    enemy->hurt(SPIKE_DAMAGE);
    if (!enemy->getCharacter()->getBody()->GetWorld()->IsLocked())
    {
        enemy->jump();
    }
    // enemy->getCharacter()->setMoveState(CharacterModel::MovementState::DEAD);
}

void GameScene::resolveEnemyWallJumpOntoTrap(float enemyVY, shared_ptr<EnemyController> enemy)
{
    // We expect reynardVY to be a negative value
    // TODO: Spencer--thats me--changed this for game feel to make spikes no longer feel like trampoleens
    enemy->getCharacter()->setVY(5);
    //    enemy->getCharacter()->setVY(-1 * enemyVY);
}

void GameScene::beginContact(b2Contact *contact)
{
    // TODO: all of these collisions need to apply for every character, not just Reynard
    // Try to get the character, assuming it's a character-on-object collision
    b2Body *charInCharOnObject = getCharacterBodyInObjectCollision(contact);
    // If it is a character-on-object collision
    if (charInCharOnObject != 0)
    {
        // Now try to get if it's an enemy-on-object collision
        shared_ptr<EnemyController> enemy = getEnemyControllerInCollision(contact);
        // If it's nullptr, then it's Reynard, and handle all that accordingly
#pragma mark REYNARD COLLISION SECTION
        if (enemy == nullptr)
        {
            // KEY COLLISIONS
            if (_keys.size()>0 || _keysCrazy.size()>0) {
                b2Body *body1 = contact->GetFixtureA()->GetBody();
                b2Body *body2 = contact->GetFixtureB()->GetBody();
                b2Body* kBody;

                // Regular keys
                for (int i = 0; i < _keys.size(); i++){
                    shared_ptr<CheckpointKey> k = _keys.at(i);
                    kBody = k->getBody();
                    if (kBody == body1 || kBody == body2) {
                        // Only remove the key if Reynard successfully picks it up
                        if (_reynardController->pickupKey(k->getID())) removeKey(k);
                        // Otherwise turn it into a regular key
                        else { k->setIsPathFinding(false); }
                    }
                }

                // Now do possessed keys
                for (int i = 0; i < _keysCrazy.size(); i++){
                    shared_ptr<CheckpointKeyCrazy> k = _keysCrazy.at(i);
                    kBody = k->getBody();
                    if (kBody == body1 || kBody == body2) {
                        // Only remove the key if Reynard successfully picks it up
                        if (_reynardController->pickupKey(k->getID())) removeKeyCrazy(k);
                    }
                }

            }
            bool reynardIsRight = _reynardController->getCharacter()->isFacingRight();
#pragma mark TRAP COLLISION CODE
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr)
            {
                trapType = trap->getType();
            }
            if (trapType == TrapModel::TrapType::SPIKE)
            {
                float reynardVY = _reynardController->getCharacter()->getVY();
                if (reynardVY < 0)
                {
                    resolveTrapOnContact();
                    resolveWallJumpOntoTrap(reynardVY);
                }
            }
            else if (trapType == TrapModel::TrapType::SAP)
            {
                // This line of code is sufficient to slow Reynard
                // No helper is used because the abstraction is unnecessary
                _reynardController->getCharacter()->slowCharacter();
            }
            // CHECKPOINT COLLISIONS
            else if (trapType == TrapModel::TrapType::CHECKPOINT)
            {
                Checkpoint* cp = dynamic_cast<Checkpoint*>(&(*trap));

                // Only allow clearing if Reynard has enough keys and it's locked, or if it's not locked
                if (!cp->isLocked() || (cp->isLocked() && _reynardController->getKeysCount() > 0)) {

                    _checkpointSwapLen = static_cast<int>(_envController->getSwapHistory().size());
                    _checkpointEnemyPos = vector<Vec2>();
                    _checkpointReynardPos = _reynardController->getCharacter()->getPosition();
                    for (auto thisEnemy : *_enemies)
                    {
                        _checkpointEnemyPos.push_back(thisEnemy->getCharacter()->getPosition());
                    }

                    // Only use a key if the checkpoint isn't already activated and it needs a key
                    if (!trap->isActivated() && cp->isLocked()) _reynardController->useKey();

                    trap->setTrapState(TrapModel::TrapState::ACTIVATED);

                    // Clear all the associated rooms
                    int outcome = _grid->clearCheckpoint(cp->getID());
                    cp->unlock();

                    vector<Checkpoint *> cps = _envController->getGrid()->getCheckpoints();
                    for (int i = 0; i < cps.size(); i++)
                    {
                        if (cps[i] == cp)
                        {
                            if (_checkpointActivatedCheckpoints.size()==0 || _checkpointActivatedCheckpoints[_checkpointActivatedCheckpoints.size() - 1] != i)
                            {
                                _checkpointActivatedCheckpoints.push_back(i);
                            }
                            break;
                        }
                    }
                    rewriteSaveFile();
                }
            }
            else if (trapType == TrapModel::TrapType::GOAL)
            {
                setComplete(true);
            }
            else if (trapType == TrapModel::TrapType::TRAPDOOR)
            {
                Vec2 v = _reynardController->getCharacter()->getLinearVelocity();
                _reynardController->getCharacter()->setLinearVelocity(Vec2(v.x, -abs(v.y) / 3));
                trap->setTrapState(TrapModel::TrapState::ACTIVATED);
            }
            else if (isThisAReynardWallContact(contact, reynardIsRight))
            {
                resolveReynardWallOnContact();
            }
            else if (isThisAReynardWallContact(contact, !reynardIsRight))
            {
                // the wall reynard's tail is touching lol
            }
            else if (isThisAReynardGroundContact(contact))
            {
                resolveReynardGroundOnContact();
            }
            else
            {
                // CULog("Non-checked contact occured with Reynard");
            }
        }
        // Otherwise it's an enemy-on-object collision, and handle that accordingly
        else
        {
            bool enemyIsRight = enemy->getCharacter()->isFacingRight();
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr)
            {
                trapType = trap->getType();
            }
            // ENEMY-SPIKE COLLISION
            // Only bother if the enemy isn't already dead
            if (trapType == TrapModel::TrapType::SPIKE && !enemy->isDead()) {
                // TODO: Change this because enemy dies instantly on contact with spikes.
                enemy->getCharacter()->setHearts(0);
                // If it's dead and a keyed enemy
                if (enemy->isDead() && enemy->_isKeyed) {
                    // Mark this enemy to get a key spawned here next frame
                    deadKeyEnemyLocs->push_back(enemy->getCharacter()->getPosition());
                }
            }
            else if (trapType == TrapModel::TrapType::SAP)
            {
                // This line of code is sufficient to slow Reynard
                // No helper is used because the abstraction is unnecessary
                enemy->getCharacter()->slowCharacter();
            }
            else if (trapType == TrapModel::TrapType::TRAPDOOR)
            {
                Vec2 v = enemy->getCharacter()->getLinearVelocity();
                enemy->getCharacter()->setLinearVelocity(Vec2(v.x, -abs(v.y) / 3));
            }
            else if (isThisAEnemyWallContact(contact, enemyIsRight, enemy))
            {
                resolveEnemyWallOnContact(enemy);
            }
            else if (isThisAEnemyWallContact(contact, !enemyIsRight, enemy))
            {
                // the wall enemy tail is touching lol
            }
            else if (isThisAEnemyGroundContact(contact, enemy))
            {
                resolveEnemyGroundOnContact(enemy);
            }
            else
            {
                // CULog("Non-checked contact occured with Enemy");
            }
        }
    }
    // Random key collisions
    else if (_keys.size()>0 || _keysCrazy.size()>0) {
        b2Body *body1 = contact->GetFixtureA()->GetBody();
        b2Body *body2 = contact->GetFixtureB()->GetBody();

        for (int i = 0; i < _keys.size(); i++){
            shared_ptr<CheckpointKey> k = _keys.at(i);
            b2Body *kBody = k->getBody();
            if (kBody == body1 || kBody == body2) {
                return;
            };
        }

        for (int i = 0; i < _keysCrazy.size(); i++){
            shared_ptr<CheckpointKeyCrazy> k = _keysCrazy.at(i);
            b2Body *kBody = k->getBody();
            if (kBody == body1 || kBody == body2) {
                return;
            };
        }

    }
    // Reynard-on-enemy collision
    else
    {
        shared_ptr<EnemyController> enemy = getEnemyControllerInCollision(contact);
        if (isReynardCollision(contact) && enemy != nullptr)
        {
            // Collision between Reynard and an enemy
            CULog("Enemy makes contact with Reynard");
            dealReynardDamage();
        }
    }
}

void GameScene::endContact(b2Contact *contact)
{
    b2Body *charInCharOnObject = getCharacterBodyInObjectCollision(contact);
    if (charInCharOnObject != 0)
    {
        // Now try to get if it's an enemy-on-object collision
        shared_ptr<EnemyController> enemy = getEnemyControllerInCollision(contact);
        // If it's nullptr, then it's Reynard, and handle all that accordingly
        if (enemy == nullptr)
        {
            if (_reynardController != nullptr && isReynardCollision(contact))
            {
                if (isThisAReynardGroundContact(contact))
                {
                    resolveReynardGroundOffContact();
                }
            }
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr)
            {
                trapType = trap->getType();
            }
            if (trapType == TrapModel::TrapType::SAP)
            {
                // This line of code is sufficient to slow Reynard
                // No helper is used because the abstraction is unnecessary
                // RESTORE REYNARDS NORMAL RUNNING SPEED THROUGH THIS LINE
                _reynardController->getCharacter()->restoreSpeed();
            }
        }
        else
        {
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr)
            {
                trapType = trap->getType();
            }
            if (trapType == TrapModel::TrapType::SAP)
            {
                // This line of code is sufficient to slow Reynard
                // No helper is used because the abstraction is unnecessary
                // RESTORE ENEMY NORMAL RUNNING SPEED THROUGH THIS LINE
                enemy->getCharacter()->restoreSpeed();
            }
            if (isThisAEnemyGroundContact(contact, enemy))
            {
                resolveEnemyGroundOffContact(enemy);
            }
        }
    }
    //    if (isReynardCollision(contact)&&isTrapDoorCollision(contact)) {
    //
    //        //TODO: Write Code to disable said trapdoor
    //    }
}

/**
 * Handles any modifications necessary before collision resolution
 *
 * This method is called just before Box2D resolves a collision.  We use this method
 * to implement sound on contact, using the algorithms outlined in Ian Parberry's
 * "Introduction to Game Physics with Box2D".
 *
 * @param  contact  	The two bodies that collided
 * @param  oldManifold  	The collision manifold before contact
 */
void GameScene::beforeSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    float speed = 0;

    // Use Ian Parberry's method to compute a speed threshold
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    for (int ii = 0; ii < 2; ii++)
    {
        if (state2[ii] == b2_addState)
        {
            b2Vec2 wp = worldManifold.points[0];
            b2Vec2 v1 = body1->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 v2 = body2->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 dv = v1 - v2;
            speed = b2Dot(dv, worldManifold.normal);
        }
    }
}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const
{
    Size dimen = Application::get()->getDisplaySize();
    //    float ratio1 = dimen.width / dimen.height;
    //    float ratio2 = ((float) SCENE_WIDTH) / ((float) SCENE_HEIGHT);
    //    if (ratio1 < ratio2) {
    //        dimen *= SCENE_WIDTH / dimen.width;
    //    } else {
    //        dimen *= SCENE_HEIGHT / dimen.height;
    //    }
    dimen *= SCENE_HEIGHT / dimen.height;
    return dimen;
}

void GameScene::dealReynardDamage()
{
    std::chrono::duration<float> diff = std::chrono::system_clock::now() - _lastHurt;
    if (diff.count() > 3)
    {
        _reynardController->getCharacter()->setHearts(_reynardController->getCharacter()->getHearts() - SPIKE_DAMAGE);
        _lastHurt = std::chrono::system_clock::now();
        _reynardController->getSceneNode()->setColor(Color4(255, 80, 80, 255));
        keepRedFrames = 5;
    }
}

/**
 * Render the scene. As this class is a scene and have many child node, calling super render will be enough
 * A placeholder for future usage.
 * @param batch The SpriteBatch to draw with.

 */
void GameScene::render(const std::shared_ptr<SpriteBatch> &batch)
{
    Scene2::render(batch);
}

/* Converts input coordinates to coordinates in the game world */
Vec2 GameScene::inputToGameCoords(Vec2 inputCoords)
{
    return inputCoords - Application::get()->getDisplaySize().height / SCENE_HEIGHT * (_worldnode->getPaneTransform().getTranslation() - Vec2(0, _worldnode->getPaneTransform().getTranslation().y) * 2);
}

/**
 * Create a key at the given location in PHYSICS space
 */
void GameScene::createKey(Vec2 pos, bool isPossesed, bool isPathFinding) {
    std::shared_ptr<cugl::scene2::PolygonNode> n = cugl::scene2::SpriteNode::allocWithTexture(_assets->get<Texture>("key"));

    if (isPossesed) {
        std::shared_ptr<CheckpointKeyCrazy> k = CheckpointKeyCrazy::alloc(Vec2(0,0),Size(1.0f, 1.0f));
        k->setSceneNode(n);
        addObstacle(k, n);
        k->setPosition(pos);
        k->setDrawScale(_scale);
        n->setScale(.2);
        k->setIsPathFinding(isPathFinding);
        _keysCrazy.push_back(k);
    }
    else {
        std::shared_ptr<CheckpointKey> k  = CheckpointKey::alloc(Vec2(0,0),Size(1.0f, 1.0f));
        k->setSceneNode(n);
        addObstacle(k, n);
        k->setPosition(pos);
        k->setDrawScale(_scale);
        n->setScale(.2);
        k->setIsPathFinding(isPathFinding);
        _keys.push_back(k);

        //CULog("Position: (%f, %f)", pos.x, pos.y);
    }
}

void GameScene::removeKey(shared_ptr<CheckpointKey> k) {
    // do not attempt to remove a key that has already been removed
    if (_keys.size() <= 0) return;

    auto itr = _keys.begin();
    while (itr != _keys.end()) {
        if ((*itr) == k && (*itr) != nullptr && !((*itr)->isRemoved())) {
            _worldnode->removeChild(k->getSceneNode());
            (*itr)->markRemoved(true);
            itr = _keys.erase(itr);
        }
        else {
            ++itr;
        }
    }
}

void GameScene::removeKeyCrazy(shared_ptr<CheckpointKeyCrazy> k) {
    // do not attempt to remove a key that has already been removed
    if (_keysCrazy.size() <= 0) return;

    auto itr = _keysCrazy.begin();
    while (itr != _keysCrazy.end()) {
        if ((*itr) == k && (*itr) != nullptr && !((*itr)->isRemoved())) {
            _worldnode->removeChild(k->getSceneNode());
            (*itr)->markRemoved(true);
            itr = _keysCrazy.erase(itr);
        }
    }
}

