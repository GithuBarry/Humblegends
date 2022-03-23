//
//  MPGameScene.h
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Owner: Barry Wang
//  Contributors: Barry Wang, Jordan Selin
//  Version: 3/13/22
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

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   32.0f

/** Height of the game world in Box2d units */
float DEFAULT_HEIGHT = DEFAULT_WIDTH / SCENE_WIDTH * SCENE_HEIGHT;

/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -22.0f

/** The default value of Spike damage */
#define SPIKE_DAMAGE    100.0f

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
    _envController = make_shared<EnvController>();

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
    //_debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset / _scale);
    setDebug(false);

    addChild(_worldnode);
    addChild(_debugnode);

    populate();
    _active = true;
    _complete = false;

    // Give all enemies a reference to the ObstacleWorld for raycasting
    //EnemyController::setObstacleWorld(_world);

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
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();

    setComplete(false);
    populate();
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


#pragma mark Rooms
    /////////////////////////////////////
    // DEBUG: add room to scene graph
    /////////////////////////////////////
    _grid = make_shared<GridModel>();
    _grid->init(_scale, true, 10, 10, _assets->get<Texture>("overgrowth1"));

    _worldnode->addChild(_grid);
    _grid->setScale(0.4);
    _envController->setGrid(_grid);

    //_grid->setPosition(0,-240);

    // Populate physics obstacles for grid
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> physics_objects = _grid->getPhysicsObjects();
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = physics_objects->begin(); itr != physics_objects->end(); ++itr) {
        _world->addObstacle(*itr);
        (*itr)->setDebugScene(_debugnode);
        (*itr)->setDebugColor(Color4::RED);
        //CULog("populate: %f %f ", (*itr)->getPosition().x);
    }

#pragma mark Reynard
    Vec2 pos = Vec2(4, 3);
    // Create a controller for Reynard based on his image texture
//    _reynardController = ReynardController::alloc(pos, _scale, _assets->get<Texture>("reynard"));

    _reynardController = ReynardController::alloc(pos, _scale, _assets->get<Texture>("reynard"), _assets->get<Texture>("reynard_run"));
    // Add Reynard to physics world
    addObstacle(_reynardController->getCharacter(), _reynardController->getSceneNode()); // Put this at the very front

#pragma mark Enemies
    //pos = Vec2(15, 3);
    //// Create a controller for an enemy based on its image texture
    //_enemies->push_back(EnemyController::alloc(pos, _scale, _assets->get<Texture>("rabbit"), _assets->get<Texture>("rabbit")));
    //// Add enemies to physics world
    //vector<std::shared_ptr<EnemyController>>::iterator itr;
    //for (itr = _enemies->begin(); itr != _enemies->end(); ++itr) {
    //    addObstacle((*itr)->getCharacter(), (*itr)->getSceneNode());
    //}
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

    // Process the toggled key commands
    if (_input.didDebug()) {
        setDebug(!isDebug());
        //_worldnode->setVisible(!_worldnode->isVisible());
    }


    // Reset Process toggled by key command
    if (_input.didReset()) {reset();}
    // Exit Process toggled by key command
    if (_input.didExit()) {
        CULog("Shutting down");
        Application::get()->quit();
    }

    // Room swap initiated
    if (_input.didPress() && !_gamestate.zoomed_in()) {
        // Scale tap/click location by camera pan
        Vec2 pos = _input.getPosition() - Application::get()->getDisplaySize().height / SCENE_HEIGHT * _worldnode->getPaneTransform().getTranslation();
        //CULog("Touch_x: %f Scene_pos_x: %f",_input.getPosition().x ,pos.x);
        bool hasSwapped = false;
        if (_envController->hasSelected()) {
            bool check = _envController->swapWithSelected(pos, _reynardController);
        } else {
            hasSwapped = _envController->selectRoom(pos, _reynardController);
        }
    }

    // Only allow jumping while zoomed in
    if (_input.didJump() && _gamestate.zoomed_in()) {
        _reynardController->jump();
        //cout << "Press Jump Button" << endl;
    }

    if (_input.didZoomIn()) {
        _gamestate.zoom_in();
        // Deselect any selected rooms
        _envController->deselectRoom();
    }

    // When zooming out
    if (_input.didZoomOut()) {
        _gamestate.zoom_out();
    }

    // When dashing right
    if (_input.didDashRight()) {
        //TODO: make dash less buggy and uncomment
        //_reynardController->dashRight();
    }

    // When dashing left
    if (_input.didDashLeft()) {
        //TODO: make dash less buggy and uncomment
        //_reynardController->dashLeft();
    }


    float scaled_dt = _gamestate.getScaledDtForPhysics(dt);
    _reynardController->update(scaled_dt);
    _world->update(scaled_dt);

    // Camera following reynard, with some non-linear smoothing
    Vec2 currentTranslation = _worldnode->getPaneTransform().getTranslation();
    Vec2 reynardScreenPosition = _worldnode->getPaneTransform().transform(_reynardController->getSceneNode()->getPosition());

    bool faceRight = _reynardController->getCharacter()->isFacingRight();

    _worldnode->applyPan(_gamestate.getPan(currentTranslation, reynardScreenPosition, _scale, getSize(), faceRight));
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
}

/**
 * Processes the start of a collision
 *
 * This method is called when we after get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we need to turn around Reynard.
 *
 * To do that , we check if reynard's contact points align vertically
 *
 * @param  contact  The two bodies that collided
 */

bool GameScene::isReynardCollision(b2Contact *contact) {
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    return body1 == _reynardController->getCharacter()->getBody() || body2 == _reynardController->getCharacter()->getBody();
}


b2Fixture *GameScene::getReynardFixture(b2Contact *contact) {
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody()) {
        return contact->GetFixtureA();
    } else {
        return contact->GetFixtureB();
    }
}

// TODO: there's gotta be a better way to do this
b2Fixture *GameScene::getNotReynardFixture(b2Contact *contact) {
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();
    if (body1 == _reynardController->getCharacter()->getBody()) {
        return contact->GetFixtureB();
    } else {
        return contact->GetFixtureA();
    }
}

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

bool GameScene::isSpikeTrapCollision(b2Contact *contact) {
    b2Body *body1 = contact->GetFixtureA()->GetBody();
    b2Body *body2 = contact->GetFixtureB()->GetBody();

    for (int row = 0; row < _grid->getWidth(); row++) {
        for (int col = 0; col < _grid->getHeight(); col++) {
            if (_grid->getRoom(row, col)->getTrap() != nullptr) {
                shared_ptr<TrapModel> _trap = _grid->getRoom(row, col)->getTrap();
                if ((_trap->getType() == "spike" && _trap->getObstacle()->getBody() == body1)
                        || (_trap->getType() == "spike" && _trap->getObstacle()->getBody() == body2)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * The primary purpose of this function obfuscate the code
 * behind what happens during the interaction between Reynard
 * and a trap upon collision
 *
 * The current Implementation only sees Reynard's health decremented by 1.
 */

void GameScene::resolveTrapCollision() {
    if (_reynardController->canBeHit()) {
        _reynardController->getCharacter()->setHearts(_reynardController->getCharacter()->getHearts() - SPIKE_DAMAGE);
        CULog("Reynard's Current Health: %d", (int) _reynardController->getCharacter()->getHearts());
    }
    //TODO: Determine how else we want the game to deal with Reynard hitting a trap
    //(do we want the trap to be turned off)?
}


bool isCharacterGroundFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 4);
}

bool isCharacterLeftFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 6);
}

bool isCharacterRightFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 5);
}

// Whether the fixture is an enemy detection radius
bool isEnemyDetectFixture(b2Fixture *fixture) {
    return (fixture->GetUserData().pointer == 10);
}

void GameScene::beginContact(b2Contact *contact) {
    // If Reynard is one of the collidees
    if (isReynardCollision(contact)) {
        // CULog("Is this a Reynard collision? %d", isReynardCollision(contact));
        // CULog("Is Reyanrd facing right? %d", ReynardIsRight);
        // CULog("Is Reyanrd grounded? %d", ReynardIsGrounded);
        // CULog("Is this right fixture? %d", isCharacterRightFixture(reynardFixture));
        // CULog("Is this left fixture? %d", isCharacterRightFixture(reynardFixture));
        // CULog("Fixture ID %i", reynardFixture->GetUserData().pointer);

        // if statement check to see if contact contains a trap
        // Call Helper resolveTrapCollision
        //
        if (true && isSpikeTrapCollision(contact)) {
            resolveTrapCollision();
        }

        bool reynardIsRight = _reynardController->getCharacter()->isFacingRight();
        b2Fixture *reynardFixture = getReynardFixture(contact);
        // First, if Reynard has hit an enemy detection radius
        if (isEnemyDetectFixture(getNotReynardFixture(contact))) {
            //CULog("Reynard spotted");
            //_enemies->at(0)->detectTarget(_reynardController->getCharacter());
        }
            // Otherwise if Reynard has hit the enemy's body
            //else if (_enemies->at(0)->isMyBody(getNotReynardFixture(contact)->GetBody())) {
            //    CULog("Body contact");
            //    // Knock back enemy
            //    b2Vec2 normal = contact->GetManifold()->localNormal;
            //    normal *= -1;
            //    _enemies->at(0)->knockback(normal);
            //    // Knock back Reynard
            //    _reynardController->knockback(contact->GetManifold()->localNormal);
            //}
            // Reynard hitting right or left wall
        else if (reynardIsRight && isCharacterRightFixture(reynardFixture)) {
            _reynardController->hitWall();
        } else if (!reynardIsRight && isCharacterLeftFixture(reynardFixture)) {
            _reynardController->hitWall();
        }
            // Reynard hitting ground
        else if (isCharacterGroundFixture(reynardFixture)) {
            _reynardController->hitGround();
        } else {
            //CULog("Switching C");
            // _reynardController->hitGround();
        }
    }

}

void GameScene::endContact(b2Contact *contact) {
    // CULog("Is this a Reynard collision END? %d", isReynardCollision(contact));
    // CULog("rey is off da ground");
    if (isReynardCollision(contact)) {
        b2Fixture *reynardFixture = getReynardFixture(contact);
        // If Reynard leaves the ground
        if (isCharacterGroundFixture(reynardFixture)) {
            _reynardController->offGround();
        }
        //if (isCharacterGroundFixture(reynardFixture) && !_enemies->at(0)->isMyBody(getNotReynardFixture(contact)->GetBody())) {
        //    _reynardController->offGround();
        //}
        //// Otherwise if Reynard is leaving the enemy sensor
        //else if (isEnemyDetectFixture(getNotReynardFixture(contact))) {
        //    _enemies->at(0)->loseTarget(_reynardController->getCharacter());
        //}
    }
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

/**
 * Render the scene. As this class is a scene and have many child node, calling super render will be enough
 * A placeholder for future usage.
 * @param batch The SpriteBatch to draw with.

 */
void GameScene::render(const std::shared_ptr<SpriteBatch> &batch) {
    Scene2::render(batch);
}
