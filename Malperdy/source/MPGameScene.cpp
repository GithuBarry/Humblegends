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


#pragma mark -
#pragma mark Level Geography

/** This is the size of the active portion of the screen */
#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 576

#define LEVEL_MUSIC "level_music"

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f

/** Height of the game world in Box2d units */
float DEFAULT_HEIGHT = DEFAULT_WIDTH / SCENE_WIDTH * SCENE_HEIGHT;

/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -22.0f

/** The default value of Spike damage */
#define SPIKE_DAMAGE    1.0f

/** To automate the loading of crate files */
#define NUM_CRATES 2


#pragma mark Assset Constants
/** The key for the some texture in the asset manager {@example}*/
#define SOME_TEXTURE       "sometexturename"

/** Color to outline the physics nodes */
#define STATIC_COLOR    Color4::WHITE
/** Opacity of the physics outlines {@example} */
#define SOME_COLOR   Color4::YELLOW

/** The key for collisions sounds {@example} */

#define SOME_SOUND     "somesoundname"

/** The key for the font reference */
#define PRIMARY_FONT        "retro"


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
                         _debug(false) {
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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets) {
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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets, const Rect rect) {
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
bool GameScene::init(const std::shared_ptr<AssetManager> &assets, const Rect rect, const Vec2 gravity) {
    Size dimen = computeActiveSize();

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }

    // Start up the input handler
    _assets = assets;
    _input.init();

    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect, gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact *contact) {
        beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact *contact) {
        endContact(contact);
    };

    _world->beforeSolve = [this](b2Contact *contact, const b2Manifold *oldManifold) {
        beforeSolve(contact, oldManifold);
    };

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world. Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    //Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f); //BUGGY
    Vec2 offset;




    //CULog("Size: %f %f", getSize().width, getSize().height);
    // Create the scene graph
    _worldnode = scene2::ScrollPane::allocWithBounds(10, 10); // Number does not matter when constraint is false
    _worldnode->setPosition(offset);

    _debugnode = scene2::ScrollPane::allocWithBounds(10, 10); // Number does not matter when constraint is false
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setPosition(offset / _scale);
    setDebug(false);

    _winNode = scene2::Label::allocWithText("VICTORY!", _assets->get<Font>(PRIMARY_FONT));
    _winNode->setAnchor(Vec2::ANCHOR_CENTER);
    _winNode->setPosition(offset);
    _winNode->setBackground(Color4::BLACK);
    _winNode->setForeground(STATIC_COLOR);
    _winNode->setPadding(dimen.width / 2, dimen.height / 2, dimen.width / 2, dimen.height / 2);
    setComplete(false);

    _health = scene2::PolygonNode::allocWithFile("textures/Health_Bar_Full.png");
    _health->setAnchor(Vec2::ANCHOR_TOP_LEFT);
    Vec2 padding = Vec2(30, -20);
    _health->setPosition(offset + Vec2(0, getSize().height) + padding);
    _health->setScale(1.5);

    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_health);
    addChild(_winNode);

    // Give all enemies a reference to the ObstacleWorld for raycasting
    EnemyController::setObstacleWorld(_world);

    populate();
    _active = true;
    _complete = false;
    
    
    _trapList = _grid->getTrapList();
    

////    INITARROW CODE
//    for(shared_ptr<TrapModel> trap : *_trapList){
//        if(trap->getType()==TrapModel::TrapType::STATUE){
//            bool right = true;
//            if(_reynardController->getCharacter()->getPosition().x < trap->getPosition().x){
//                right = false;
//            }
//            createArrow(trap->getPosition(), right);
//        }
//    }


    // XNA nostalgia
    Application::get()->setClearColor(Color4f::BLACK);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _world = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winNode = nullptr;
        _health = nullptr;
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
void GameScene::reset() {
    revert(true);
}

void GameScene::revert(bool totalReset){
    vector<vector<Vec2>> swapHistory = _envController->getSwapHistory();

    _reynardController = nullptr;
    _grid = nullptr;
    _envController = nullptr;
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    _gamestate.reset();
    _enemies = nullptr;
    setComplete(false);

    if (totalReset){
        _checkpointReynardPos = reynardDefault;
        populate();
    }else{
        populateEnv();
        populateChars();
        for (int i = 0; i<_checkpointSwapLen; i++) {
            _envController->swapRoomOnGrid(swapHistory[i][0],swapHistory[i][1],true);
        }
        _reynardController->getCharacter()->setPosition(_checkpointReynardPos);
        for (int i = 0; i < _enemies->size(); i++){
            (*_enemies)[i]->getCharacter()->setPosition(_checkpointEnemyPos[i]);
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
void GameScene::populate() {
    populateEnv();
    populateChars();
}

void GameScene::populateEnv() {
    MPAudioController::playAudio(_assets, LEVEL_MUSIC, true, 1, true);
    _envController = make_shared<EnvController>();
#pragma mark Rooms
    /////////////////////////////////////
    // DEBUG: add room to scene graph
    /////////////////////////////////////
    _grid = _envController->getGrid();
    _grid->init(_assets, _scale, _assets->get<Texture>("overgrowth1"));

    _worldnode->addChild(_grid);
    _grid->setScale(0.4);

    //_grid->setPosition(0,-240);

    // Populate physics obstacles for grid
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> physics_objects = _grid->getPhysicsObjects();
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = physics_objects->begin(); itr != physics_objects->end(); ++itr) {
        _world->addObstacle(*itr);
        (*itr)->setDebugScene(_debugnode);
        (*itr)->setDebugColor(Color4::RED);
        //CULog("populate: %f %f ", (*itr)->getPosition().x);
    }
}

void GameScene::populateChars(){
#pragma mark Reynard
    Vec2 pos = _checkpointReynardPos;

    shared_ptr<Animation> reynard_animations = make_shared<Animation>(_assets->get<Texture>("reynard_all"), _assets->get<JsonValue>("framedata2")->get("reynard"));
    // initialize reynardController with the final animation map
    _reynardController = ReynardController::alloc(pos, _scale, reynard_animations);

    // Add Reynard to physics world
    Vec2 pos_temp = _reynardController->getCharacter()->getPosition();
    _reynardController->getCharacter()->setPosition(Vec2(4,3));
    addObstacle(_reynardController->getCharacter(), _reynardController->getSceneNode()); // Put this at the very front
    _reynardController->getCharacter()->setPosition(pos_temp);

#pragma mark ArrowTest
    
    //Arrows instantiation
    //Populate requries it to literally exist in screen area when called
    _arrows = make_shared<vector<std::shared_ptr<Arrow>>>();
    shared_ptr<Arrow> arrow = make_shared<Arrow>();
    Vec2 arrowPos = Vec2(30, pos.y);
    arrow->init(arrowPos, 5, false);
    
    //Node Textures
    std::shared_ptr<Texture> arrowImage = _assets->get<Texture>("Arrow");
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(arrowImage);
    arrow->setSceneNode(sprite);
    (arrow->getRight()) ? arrow->getSceneNode()->setScale(.25) : arrow->getSceneNode()->setScale(-.25);
    arrow->getRight();
//    cout<<endl;
//    cout<<"MY ARROW NOW EXISTS"<<endl;
    _arrows->push_back(arrow);
    addObstacle(arrow, arrow->getSceneNode());
    
    
    
//    Remove From OBtacleWorld
//    Remove from DebugWorld
//    Remove from SceneNode
//    Remove from List
    
    
    
    
    
    
#pragma mark Enemies

    shared_ptr<Animation> rabbit_animations = make_shared<Animation>(_assets->get<Texture>("rabbit_all"), _assets->get<JsonValue>("framedata2")->get("rabbit"));

    // Give all enemies a reference to Reynard's controller to handle detection
    _enemies = make_shared<vector<std::shared_ptr<EnemyController>>>();

    // get Level data from the JSON
    shared_ptr<JsonValue> levelJSON = _assets->get<JsonValue>("level");
    // get the layer containing entities
    shared_ptr<JsonValue> entityLayer;
    for(int i = 0; i < levelJSON->get("layers")->size(); i++){
        if(levelJSON->get("layers")->get(i)->get("name")->asString() == "entities"){
            entityLayer =levelJSON->get("layers")->get(i);
        }
    }
    // get the entity tileset offset from levelJSON
    int entity_offset;
    for(int i=0; i< levelJSON->get("tilesets")->size(); i++){
        if( levelJSON->get("tilesets")->get(i)->get("source")->asString().find("entities") != string::npos){
            entity_offset = levelJSON->get("tilesets")->get(i)->get("firstgid")->asInt();
        }
    }

    // For each tile in the entity layer,
    for(int i = 0; i < entityLayer->get("data")->size(); i++){

        // if there is something in the tile
        if(entityLayer->get("data")->get(i)->asInt() != 0){
            int tile = entityLayer->get("data")->get(i)->asInt() - entity_offset;

            // determine if the entity corresponds to an enemy
            shared_ptr<JsonValue> temp1 =_assets->get<JsonValue>("tileset_entities")->get("tiles");
            string temp =temp1->get(tile - temp1->get(0)->get("id")->asInt())->get("image")->asString();

            // if it is an enemy...
            if(temp.find("enemy") != string::npos){

                //HARDCODED
                int room = i / 8/ 12;
                int x = room % (levelJSON->get("width")->asInt() / 12);
                int y = room / (levelJSON->get("width")->asInt() / 12);
                Vec2 enemypos = Vec2(x, levelJSON->get("height")->asInt() /8 -1 -y);

                // initialize it
//                _enemies->push_back(EnemyController::alloc(enemypos * Vec2(12,8), _scale, rabbit_animations));
                _enemies->push_back(EnemyController::alloc(Vec2::ZERO, _scale, rabbit_animations));

                _enemies->back()->setObstacleWorld(_world);
                _enemies->back()->setReynardController(_reynardController);
                addObstacle(_enemies->back()->getCharacter(), _enemies->back()->getSceneNode());


                _enemies->back()->getCharacter()->setPosition((enemypos + Vec2(1,1)) * Vec2(5,5));
            }
        }
    }
    //INITIALIZE AN ARROR TO TEST IT

    // Initialize EnemyController with the final animation map and store in vector of enemies
    //_enemies->push_back(EnemyController::alloc(Vec2(3, 3), _scale, rabbit_animations));

//    for(shared_ptr<EnemyController> enemy : *_enemies){
//        enemy->setObstacleWorld(_world);
//        enemy->setReynardController(_reynardController);
//        addObstacle(enemy->getCharacter(), enemy->getSceneNode()); // Put
//    }

    _checkpointEnemyPos = vector<Vec2>();
    _checkpointReynardPos = _reynardController->getCharacter()->getPosition();
    for (auto enemy: *_enemies){
        _checkpointEnemyPos.push_back(enemy->getCharacter()->getPosition());
    }


}

void GameScene::createArrow(Vec2 pos, bool right){
    //Arrows instantiation
    _arrows = make_shared<vector<std::shared_ptr<Arrow>>>();
    shared_ptr<Arrow> arrow = make_shared<Arrow>();
    arrow->init(pos, 5, right);
    
    //Node Textures
    std::shared_ptr<Texture> arrowImage = _assets->get<Texture>("Arrow");
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(arrowImage);
    arrow->setSceneNode(sprite);
    (arrow->getRight()) ? arrow->getSceneNode()->setScale(.25) : arrow->getSceneNode()->setScale(-.25);
    arrow->getRight();
//    cout<<endl;
//    cout<<"MY ARROW NOW EXISTS"<<endl;
    _arrows->push_back(arrow);
    addObstacle(arrow, arrow->getSceneNode());

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
        const std::shared_ptr<scene2::SceneNode> &node) {
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);
    obj->setDebugColor(Color4::RED);

    // Position the scene graph node (enough for static objects)
    node->setPosition(obj->getPosition() * _scale);
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode *weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle *obs) {
            weak->setPosition(obs->getPosition() * _scale);
            weak->setAngle(obs->getAngle());
        });
    }
}


#pragma mark -
#pragma mark Physics Handling

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
void GameScene::update(float dt) {
    _input.update(dt);
    Vec2 inputPos = inputToGameCoords(_input.getPosition());

    // Process the toggled key commands
    if (_input.didDebug()) {
        setDebug(!isDebug());
        //_worldnode->setVisible(!_worldnode->isVisible());
        vector<std::shared_ptr<EnemyController>>::iterator itr;
        for (itr = _enemies->begin(); itr != _enemies->end(); ++itr) {
            (*itr)->setDebug(true);
        }
    }


    // Reset Process toggled by key command
    if (_input.didReset()) {reset();}
    // Exit Process toggled by key command
    if (_input.didExit()) {
        CULog("Shutting down");
        Application::get()->quit();
    }


    // reynard red when hurt/dealt damage
    if(keepRedFrames>0){
        //keep time (frame)
        keepRedFrames-=1;
    }else{
        //restore
        _reynardController->getSceneNode()->setColor(Color4::WHITE);
    }



    // Variables to indicate which forms of room swap are being used
    bool usingClick = true;
    bool usingDrag = true;

    bool triedSwap = false;
    Vec2 progressCoords = Vec2(-1, -1);
    // Room swap by click
    if (usingClick && !_gamestate.zoomed_in() && _input.didPress()) {
        if (_envController->hasSelected()) {
            triedSwap = true;
            _envController->swapWithSelected(inputPos, _reynardController, _enemies);
        } else {
            _envController->selectRoom(inputPos, _reynardController, _enemies);
        }
    }
    // Room swap by drag
    if (usingDrag && !_gamestate.zoomed_in()) {
        if (_input.didPress() && !triedSwap) {
            _envController->selectRoom(inputPos, _reynardController, _enemies);
        }
        else if (_input.didEndDrag() && _envController->hasSelected()) {
            _envController->swapWithSelected(inputPos, _reynardController, _enemies);
        }
        if (_input.isDragging() && _envController->hasSelected()) {
            progressCoords = inputPos;
        }
    }

    // Only allow jumping while zoomed in
    if (_input.didJump() && _gamestate.zoomed_in()) {
        _reynardController->jump();
        corner_num_frames_workaround = 0;
        //cout << "Press Jump Button" << endl;
        //CULog("jumpin");
    }
    // When dashing right
    else if (_input.didDashRight() && _gamestate.zoomed_in()) {
        _reynardController->dashRight();
    }

    // When dashing left
    else if (_input.didDashLeft() && _gamestate.zoomed_in()) {
        _reynardController->dashLeft();
    }

    if (_input.didZoomIn()) {
        _gamestate.zoom_in();
        if (!_gamestate.zoomed_in()&& _gamestate.finishedZooming(_worldnode->getZoom())){
            //_gamestate.pauseSwitch();
        }
        // Deselect any selected rooms
        _envController->deselectRoom();
    }

    if (_reynardController->getCharacter()->getHearts()<=0 ){
        revert(false);
        return;
    }


    // When zooming out
    else if (_input.didZoomOut()) {
        if (!_gamestate.zoomed_in() && _gamestate.finishedZooming(_worldnode->getZoom())){
            revert(true);
        }
        _gamestate.zoom_out();
        _envController->deselectRoom();
    }

    //if (_gamestate.isPaused()){
    //    return;
    //}




    float scaled_dt = _gamestate.getScaledDtForPhysics(dt);
    //TODO: Why does both these updates exist you only need the _world one
    _reynardController->update(scaled_dt);
    _world->update(scaled_dt);

    //TODO debugging area. Disable for releases
    if ((!_reynardController->getCharacter()->isOnWall() ) && abs(_reynardController->getCharacter()->getLinearVelocity().x) <= 0.5 ){
        CULog("likely Error 01: Reynard stuck. See MPGameScene.c update() and breakpoint here");
        _reynardController->getCharacter()->setLinearVelocity((3.7*(_reynardController->getCharacter()->isFacingRight()?1:-1)),0.1);

    }
    else if(abs(_reynardController->getCharacter()->getLinearVelocity().x) + abs(_reynardController->getCharacter()->getLinearVelocity().y)==0){
        corner_num_frames_workaround +=1;
        if (corner_num_frames_workaround>2){
            _reynardController->getCharacter()->setLinearVelocity((3.7*(_reynardController->getCharacter()->isFacingRight()?1:-1)),0);
            corner_num_frames_workaround = 0;
        }

    }
    if ( _reynardController->getCharacter()->isJumping()  && abs(_reynardController->getCharacter()->getLinearVelocity().x)<7){
        //CULog("likely Error 02: Reynard jumping slow. See MPGameScene.c update() and breakpoint here");
    }


    // Camera following reynard, with some non-linear smoothing
    Vec2 currentTranslation = _worldnode->getPaneTransform().getTranslation();
    Vec2 reynardScreenPosition = _worldnode->getPaneTransform().transform(_reynardController->getSceneNode()->getPosition());

    bool faceRight = _reynardController->getCharacter()->isFacingRight();
    Vec2 reynardVelocity = _reynardController->getCharacter()->getLinearVelocity();

    _worldnode->applyPan(_gamestate.getPan(currentTranslation, reynardScreenPosition, _scale, getSize(), faceRight,reynardVelocity));
    _worldnode->applyZoom(_gamestate.getZoom(_worldnode->getZoom()));

    // Copy World's zoom and transform
    _debugnode->applyPan(-_debugnode->getPaneTransform().transform(Vec2()));
    _debugnode->applyPan(_worldnode->getPaneTransform().transform(Vec2()) / _scale);
    _debugnode->applyZoom(1 / _debugnode->getZoom());
    _debugnode->applyZoom(_worldnode->getZoom());

    // Update all enemies
    vector<std::shared_ptr<EnemyController>>::iterator itr;
    for (itr = _enemies->begin(); itr != _enemies->end(); ++itr) {
        (*itr)->update(dt);
    }

    // Update the environment
    _envController->update(progressCoords, _reynardController, _enemies);

    // Update the UI
    if (_reynardController->getCharacter()->getHearts() >= 3) {
        _health->setTexture("textures/Health_Bar_Full.png");
    }else if (_reynardController->getCharacter()->getHearts() == 2) {
        _health->setTexture("textures/Health_Bar_Two_Third.png");
    }else if (_reynardController->getCharacter()->getHearts() == 1) {
        _health->setTexture("textures/Health_Bar_One_Third.png");
    }else if (_reynardController->getCharacter()->getHearts() <= 0) {
        _health->setTexture("textures/Health_Bar_None.png");
    }
}

#pragma mark -
#pragma mark Collision Handling

bool GameScene::isCharacterGroundFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 4);
}

bool GameScene::isCharacterRightFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 5);
}

bool GameScene::isCharacterLeftFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 6);
}

bool GameScene::isEnemyDetectFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 10);
}

b2Fixture *GameScene::getReynardFixture(b2Contact *contact) {
    // We assume that you have checked to see that at least
    // one of the bodies in the contact event have been
    // verified to be a Reynard object
    //assert(isReynardCollision(contact))
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody() ||
        body2 ==_reynardController->getCharacter()->getBody()) {
        return contact->GetFixtureA();
    } else {
        return contact->GetFixtureB();
    }
}

b2Fixture *GameScene::getEnemyFixture(b2Contact *contact) {
    // TODO this function is not right. Also what if both are enemies?
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody() ||
        body2 == _reynardController->getCharacter()->getBody()) {
        return contact->GetFixtureA();
    } else {
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
shared_ptr<TrapModel> GameScene::isTrapCollision(b2Contact* contact) {
    if (_grid == nullptr) return nullptr;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    for (int row = 0; row < _grid->getWidth(); row++) {
        for (int col = 0; col < _grid->getHeight(); col++) {
            if (_grid->getRoom(row, col)->getTrap() != nullptr) {
                shared_ptr<TrapModel> _trap = _grid->getRoom(row, col)->getTrap();
                b2Body* body = _trap->getObstacle()->getBody();
                bool isCollision = body == body1 || body == body2;
                if (isCollision) return _trap;
            }
        }
    }
    return nullptr;
}

bool GameScene::isReynardCollision(b2Contact *contact) {
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
bool GameScene::isCharacterBody(b2Body* body) {
    if (_reynardController != nullptr) {
        // Return true if it's Reynard
        if (body == _reynardController->getCharacter()->getBody()) return true;
    }
    // Otherwise check against the enemies and return true if it's one of them
    for (auto itr = _enemies->begin(); itr != _enemies->end(); ++itr) {
        if ((*itr)->getCharacter()->getBody() == body) return true;
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
b2Body* GameScene::getCharacterBodyInObjectCollision(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();

    //// Only characters have their body user data set to something nonzero
    //// Thus, it's a character-on-object collision if only one of these bodies has nonzero user data
    //// Return nullptr if it's not character-on-object
    //if (!((body1->GetUserData().pointer == 0 && body2->GetUserData().pointer != 0) ||
    //    (body1->GetUserData().pointer != 0 && body2->GetUserData().pointer == 0))) return nullptr;

    //// Return whichever body is the character involved in the collision
    //return body1->GetUserData().pointer == 0 ? body2 : body1;

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
shared_ptr<EnemyController> GameScene::getEnemyControllerInCollision(b2Contact *contact) {
    //// Get body user data and convert to BodyData
    //CharacterController<EnemyModel, EnemyController>::BodyData* bodyData =
    //    static_cast<CharacterController<EnemyModel, EnemyController>::BodyData*>
    //    ((void*)body->GetUserData().pointer);

    //// Return nullptr now if it's not for an enemy
    //if (bodyData->_type != CharacterController<EnemyModel, EnemyController>::CharacterType::ENEMY)
    //    return nullptr;

    //// Otherwise, cast the given pointer to an EnemyController pointer and return
    //EnemyController* enemyPtr = static_cast<EnemyController*>(bodyData->_controller);

    //return make_shared<EnemyController>(*enemyPtr);

    // Check body against all enemies in level
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    for (auto itr = _enemies->begin(); itr != _enemies->end(); ++itr) {
        if (((*itr)->getCharacter()->getBody() == body1) || ((*itr)->getCharacter()->getBody() == body2))return (*itr);
    }

    // If not an enemy, return nullptr
    return nullptr;
}


bool GameScene::isThisAReynardWallContact(b2Contact *contact, bool reynardIsRight) {
    b2Fixture *reynardFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody()) {
        reynardFixture = contact->GetFixtureA();
    } else {
        reynardFixture = contact->GetFixtureB();
    }
    if (reynardIsRight && isCharacterRightFixture(reynardFixture)) {
        return true;
    }
    else return !reynardIsRight && isCharacterLeftFixture(reynardFixture);
}

bool GameScene::isThisAEnemyWallContact(b2Contact *contact, bool enemyIsRight, shared_ptr<EnemyController> enemy) {
    b2Fixture *enemyFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == enemy->getCharacter()->getBody()) {
        enemyFixture = contact->GetFixtureA();
    } else {
        enemyFixture = contact->GetFixtureB();
    }
    if (enemyIsRight && isCharacterRightFixture(enemyFixture)) {
        return true;
    }
    else return !enemyIsRight && isCharacterLeftFixture(enemyFixture);
}

bool GameScene::isThisAEnemyGroundContact(b2Contact *contact, shared_ptr<EnemyController> enemy) {
    b2Fixture *enemyFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == enemy->getCharacter()->getBody()) {
        enemyFixture = contact->GetFixtureA();
    } else {
        enemyFixture = contact->GetFixtureB();
    }
    return isCharacterGroundFixture(enemyFixture);
}

bool GameScene::isThisAReynardGroundContact(b2Contact *contact) {
    b2Fixture *reynardFixture;
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody()) {
        reynardFixture = contact->GetFixtureA();
    } else {
        reynardFixture = contact->GetFixtureB();
    }
    return isCharacterGroundFixture(reynardFixture);
}

void GameScene::resolveReynardWallOnContact() {
    _reynardController->hitWall();
}

void GameScene::resolveEnemyWallOnContact(shared_ptr<EnemyController> enemy) {
    enemy->hitWall();
}

void GameScene::resolveReynardGroundOnContact() {
    _reynardController->hitGround();
}

void GameScene::resolveEnemyGroundOffContact(shared_ptr<EnemyController> enemy) {
    enemy->offGround();
}

void GameScene::resolveEnemyGroundOnContact(shared_ptr<EnemyController> enemy) {
    enemy->hitGround();
}

void GameScene::resolveReynardGroundOffContact() {
    _reynardController->offGround();
}

void GameScene::resolveTrapOnContact() {
    if (_reynardController->canBeHit()) {
        dealReynardDamage();
    }
}

void GameScene::resolveWallJumpOntoTrap(float reynardVY) {
    // We expect reynardVY to be a negative value
    // KNOCKBACK
    _reynardController->getCharacter()->setVY(5);
//    _reynardController->getCharacter()->setVY(-1 * reynardVY);
}

void GameScene::resolveEnemyTrapOnContact(shared_ptr<EnemyController> enemy) {
    enemy->getCharacter()->setHearts(enemy->getCharacter()->getHearts() - SPIKE_DAMAGE);
    enemy->jump();
    //enemy->getCharacter()->setMoveState(CharacterModel::MovementState::DEAD);
}

void GameScene::resolveEnemyWallJumpOntoTrap(float enemyVY, shared_ptr<EnemyController> enemy) {
    // We expect reynardVY to be a negative value
    //TODO: Spencer--thats me--changed this for game feel to make spikes no longer feel like trampoleens
    enemy->getCharacter()->setVY(5);
//    enemy->getCharacter()->setVY(-1 * enemyVY);
}


void GameScene::beginContact(b2Contact *contact) {
    // TODO: all of these collisions need to apply for every character, not just Reynard
    // Try to get the character, assuming it's a character-on-object collision
    b2Body* charInCharOnObject = getCharacterBodyInObjectCollision(contact);
    // If it is a character-on-object collision
    if (charInCharOnObject != 0) {
        // Now try to get if it's an enemy-on-object collision
        shared_ptr<EnemyController> enemy = getEnemyControllerInCollision(contact);
        // If it's nullptr, then it's Reynard, and handle all that accordingly
        if (enemy == nullptr) {
            bool reynardIsRight = _reynardController->getCharacter()->isFacingRight();
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr){
                trapType = trap->getType();
            }
            // Spike trap collision code
            if (trapType == TrapModel::TrapType::SPIKE) {
                float reynardVY = _reynardController->getCharacter()->getVY();
                if (reynardVY < 0) {
                    resolveTrapOnContact();
                    resolveWallJumpOntoTrap(reynardVY);
                }
            }
            // Sap Trap Collision Code
            else if (trapType == TrapModel::TrapType::SAP) {
                //This line of code is sufficient to slow Reynard
                //No helper is used because the abstraction is unnecessary
                _reynardController->getCharacter()->slowCharacter();
            }
            else if (trapType == TrapModel::TrapType::CHECKPOINT) {
                _checkpointSwapLen = static_cast<int>(_envController->getSwapHistory().size());
                _checkpointEnemyPos = vector<Vec2>();
                _checkpointReynardPos = _reynardController->getCharacter()->getPosition();
                for (auto thisEnemy: *_enemies){
                    _checkpointEnemyPos.push_back(thisEnemy->getCharacter()->getPosition());
                }
                trap->getPolyNode()->setColor(Color4::GREEN);
            }
            else if (trapType == TrapModel::TrapType::STATUE){
                //Nothing needs be written in here I just need to pre-empt some later logic
            }
            else if (trapType == TrapModel::TrapType::GOAL) {
                setComplete(true);
            }
            // Trapdoor collision code
            else if (trapType == TrapModel::TrapType::TRAPDOOR) {
                Vec2 v = _reynardController->getCharacter()->getLinearVelocity();
                _reynardController->getCharacter()->setLinearVelocity(Vec2(v.x,-abs(v.y)/3));
            }
            else if (isThisAReynardWallContact(contact, reynardIsRight)) {
                resolveReynardWallOnContact();
            }
            else if (isThisAReynardWallContact(contact, !reynardIsRight)) {
                // the wall reynard's tail is touching lol
            }
            else if (isThisAReynardGroundContact(contact)) {
                resolveReynardGroundOnContact();
            }
            else {
                //CULog("Non-checked contact occured with Reynard");
            }
        }
        // Otherwise it's an enemy-on-object collision, and handle that accordingly
        else {
            bool enemyIsRight = enemy->getCharacter()->isFacingRight();
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr){
                trapType = trap->getType();
            }
            if (trapType == TrapModel::TrapType::SPIKE) {
                float enemyVY = enemy->getCharacter()->getVY();
                if (enemyVY < 0) {
                    resolveEnemyTrapOnContact(enemy);
                    if(!enemy->getCharacter()->isDead()){
                        resolveEnemyWallJumpOntoTrap(enemyVY, enemy);
                    }
                }
            }
            else if (trapType == TrapModel::TrapType::SAP) {
                    //This line of code is sufficient to slow Reynard
                    //No helper is used because the abstraction is unnecessary
                enemy->getCharacter()->slowCharacter();
            }
            else if (trapType == TrapModel::TrapType::TRAPDOOR) {
                Vec2 v = enemy->getCharacter()->getLinearVelocity();
                enemy->getCharacter()->setLinearVelocity(Vec2(v.x,-abs(v.y)/3));
            }
            else if (isThisAEnemyWallContact(contact, enemyIsRight, enemy)) {
                resolveEnemyWallOnContact(enemy);
            }
            else if (isThisAEnemyWallContact(contact, !enemyIsRight, enemy)) {
                // the wall enemy tail is touching lol
            }
            else if (isThisAEnemyGroundContact(contact, enemy)) {
                resolveEnemyGroundOnContact(enemy);
            }
            else {
                //CULog("Non-checked contact occured with Enemy");
            }
        }
    }
    // Reynard-on-enemy collision
    else {
        shared_ptr<EnemyController> enemy = getEnemyControllerInCollision(contact);
        if (isReynardCollision(contact) && enemy != nullptr) {
            // Collision between Reynard and an enemy
            CULog("Enemy makes contact with Reynard");
            dealReynardDamage();

        }
    }
}


void GameScene::endContact(b2Contact *contact) {
    b2Body* charInCharOnObject = getCharacterBodyInObjectCollision(contact);
    if (charInCharOnObject != 0) {
        // Now try to get if it's an enemy-on-object collision
        shared_ptr<EnemyController> enemy = getEnemyControllerInCollision(contact);
        // If it's nullptr, then it's Reynard, and handle all that accordingly
        if (enemy == nullptr) {
            if (_reynardController != nullptr && isReynardCollision(contact)) {
                if (isThisAReynardGroundContact(contact)) {
                    resolveReynardGroundOffContact();
                }
            }
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr){
                trapType = trap->getType();
            }
            if (trapType == TrapModel::TrapType::SAP) {
                //This line of code is sufficient to slow Reynard
                //No helper is used because the abstraction is unnecessary
                //RESTORE REYNARDS NORMAL RUNNING SPEED THROUGH THIS LINE
                cout<<"I AM CALLED"<<endl;
                _reynardController->getCharacter()->restoreSpeed();
            }
        }
        else {
            shared_ptr<TrapModel> trap = isTrapCollision(contact);
            TrapModel::TrapType trapType = TrapModel::TrapType::UNTYPED;
            if (trap != nullptr){
                trapType = trap->getType();
            }
            if (trapType == TrapModel::TrapType::SAP) {
                //This line of code is sufficient to slow Reynard
                //No helper is used because the abstraction is unnecessary
                //RESTORE ENEMY NORMAL RUNNING SPEED THROUGH THIS LINE
                enemy->getCharacter()->restoreSpeed();
            }
            if (isThisAEnemyGroundContact(contact, enemy)) {
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
void GameScene::beforeSolve(b2Contact *contact, const b2Manifold *oldManifold) {
    float speed = 0;

    // Use Ian Parberry's method to compute a speed threshold
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    for (int ii = 0; ii < 2; ii++) {
        if (state2[ii] == b2_addState) {
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
Size GameScene::computeActiveSize() const {
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

void GameScene::dealReynardDamage(){
    std::chrono::duration<float> diff = std::chrono::system_clock::now()-_lastHurt;
    if (diff.count()>3){
        _reynardController->getCharacter()->setHearts(_reynardController->getCharacter()->getHearts() - SPIKE_DAMAGE);
        _lastHurt = std::chrono::system_clock::now();
        _reynardController->getSceneNode()->setColor(Color4(255,80,80,255));
        keepRedFrames = 5;
    }
}

/**
 * Render the scene. As this class is a scene and have many child node, calling super render will be enough
 * A placeholder for future usage.
 * @param batch The SpriteBatch to draw with.

 */
void GameScene::render(const std::shared_ptr<SpriteBatch> &batch) {
    Scene2::render(batch);
}

/* Converts input coordinates to coordinates in the game world */
Vec2 GameScene::inputToGameCoords(Vec2 inputCoords) {
    return inputCoords - Application::get()->getDisplaySize().height / SCENE_HEIGHT * (_worldnode->getPaneTransform().getTranslation() - Vec2(0, _worldnode->getPaneTransform().getTranslation().y) * 2);
}


