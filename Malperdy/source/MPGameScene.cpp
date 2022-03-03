//
//  RDGameScene.h
//  Rocket Demo
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It also handles collision detection. There is not
//  much to do for collisions; our ObstacleWorld class takes care of all
//  of that for us.  This controller mainly transforms input into gameplay.
//
//  You will notice that we do not use a Scene asset this time.  While we could
//  have done this, we wanted to highlight the issues of connecting physics
//  objects to scene graph objects.  Hence we include all of the API calls.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Humblegends
//  Contributors: Barry Wang, Jordan Selin
//  Version: 3/01/2022
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
#define DEFAULT_HEIGHT  18.0f
/** The default value of gravity (going down) */
#define DEFAULT_GRAVITY -1000.0f

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

float REYNARD_POS[] = { 30, 10 };

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets) {
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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect rect) {
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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect rect, const Vec2 gravity) {
    Size dimen = computeActiveSize();

    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    // Start up the input handler
    _assets = assets;
    _input.init();

    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect, gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
      beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact* contact) {
        endContact(contact);
    };

    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        beforeSolve(contact, oldManifold);
    };

    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world. Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);

    // Create the scene graph
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);


    addChild(_worldnode);
    addChild(_debugnode);

    populate();
    _active = true;
    _complete = false;

    // XNA nostalg_debugnodeia
    Application::get()->setClearColor(Color4f::WHITE);
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

    //TODO waiting for Reynard Controller and ReynardModel

    //_reynard = ReynardModel::alloc(Vec2(50,50));
    //addObstacle((const shared_ptr<physics2::Obstacle> &)  _reynard,(const shared_ptr<scene2::SceneNode> &) _reynard->getCharacterNode());
    //_reynardController = ReynardController(_reynard)


#pragma mark Rooms
    /////////////////////////////////////
    // DEBUG: add room to scene graph
    /////////////////////////////////////
    shared_ptr<GridModel> _grid = make_shared<GridModel>();
    _grid->init(_scale, true, 10, 10);

    _worldnode->addChild(_grid);
    _grid->setScale(0.5);
    //_grid->setPosition(0,-240);
    
    // Populate physics obstacles for grid
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> physics_objects = _grid->getPhysicsObjects();
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = physics_objects->begin(); itr != physics_objects->end(); ++itr) {
        _world->addObstacle(*itr);
        (*itr)->setDebugScene(_debugnode);
        (*itr)->setDebugColor(Color4::RED);
    }    

#pragma mark Reynard
    Vec2 reyPos = Vec2(5, 4);
    // Create image for Reynard
    std::shared_ptr<Texture> image;
    image = _assets->get<Texture>("reynard");
    // Create sprite for Reynard from texture
    std::shared_ptr<scene2::SpriteNode> sprite;
    sprite = scene2::SpriteNode::alloc(image, 1, 1);
    // Create a model for Reynard based on his image texture
    shared_ptr<ReynardModel> reynard = ReynardModel::alloc(reyPos, image->getSize() / _scale, _scale);
    reynard->setSceneNode(sprite);
    addObstacle(reynard, sprite); // Put this at the very front

    // Create controller for Reynard and assign model to that controller
    _reynardController = make_shared<ReynardController>(reynard);
    
    /*PolyFactory pf;
    shared_ptr<physics2::PolygonObstacle> po = make_shared<physics2::PolygonObstacle>();
    po->init(pf.makeNgon(Vec2(3,3), 2, 4));
    _world->addObstacle(po);*/
    
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
void GameScene::addObstacle(const std::shared_ptr<physics2::Obstacle>& obj,
    const std::shared_ptr<scene2::SceneNode>& node) {
    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);
    obj->setDebugColor(Color4::RED);

    // Position the scene graph node (enough for static objects)
    node->setPosition(obj->getPosition() * _scale);
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs) {
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
        _worldnode->setVisible(! _worldnode->isVisible());
    }


    // Reset Process toggled by key command
    if (_input.didReset()) { reset(); }
    // Exit Process toggled by key command
    if (_input.didExit()) {
        CULog("Shutting down");
        Application::get()->quit();
    }
    if (_input.didJump()) {
        _reynardController->resolveJump();
        cout<<"Press Jump Button"<<endl;
    }

    // Swipe command toggled by key command
    if (_input.didEndSwipe()) {
        Vec2 start;
        Vec2 end;
        start = _input.getSwipeStartEnd()[0];
        end = _input.getSwipeStartEnd()[1];
        _envController->selectRoom(start);
        _envController->swapWithSelected(end);
        _world->clear();

        shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> physics_objects;
        physics_objects = _grid->getPhysicsObjects();
        vector<shared_ptr<physics2::PolygonObstacle>>::iterator ptr;
        vector<shared_ptr<physics2::PolygonObstacle>> physics_vec;
        physics_vec = *physics_objects;
        for (ptr = physics_vec.begin(); ptr < physics_vec.end(); ptr++) {
            _world->addObstacle(*ptr);
        }
    }
//    _reynard->setJumping(_input.didJump());
//    _reynard->applyForce();
    // Update Reynard
    _reynardController->update(dt);
   
    
//    if (_input.didDashLeft()) {
//
//    }
//    if (_input.didDashRight()) {
//
//    }
//    if (_input.didZoomIn()) {
//
//    }
//    if (_input.didZoomOut()) {
//
//    }


    _world->update(_stateController->getScaledDtForPhysics(dt));
}


/**
 * Processes the end of a collision
 *
 * This method is called when we after get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we need to turn around Reynard.
 *
 * To do that , we check if reynard's contact points align vertically
 *
 * @param  contact  The two bodies that collided
 */
void GameScene::endContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    b2Body* wall;
    // If we hit the "win" door, we are done
    intptr_t rptr = reinterpret_cast<intptr_t>(_reynard.get());

    if(body1->GetUserData().pointer == rptr || body2->GetUserData().pointer == rptr) {
        if (body1->GetUserData().pointer == rptr){
            wall = body2;
        }else{
            wall = body1;
        }
        b2Vec2 first_collision =contact->GetManifold()->points[0].localPoint;
        int last_idx = contact->GetManifold()->pointCount-1;
        b2Vec2 last_collision =contact->GetManifold()->points[last_idx].localPoint;
        if (first_collision.x == last_collision.x){
            _reynardController->switchDirection();
        }

    }
    b2Vec2 first_collision = contact->GetManifold()->points[0].localPoint;
    int last_idx = contact->GetManifold()->pointCount - 1;
    b2Vec2 last_collision = contact->GetManifold()->points[last_idx].localPoint;
    if (first_collision.x == last_collision.x) {
        _reynardController->switchDirection();
    }
}

///**
// * Processes the start of a collision
// *
// * This method is called when we first get a collision between two objects.  We use
// * this method to test if it is the "right" kind of collision.  In particular, we
// * use it to test if we make it to the win door.
// *
// * @param  contact  The two bodies that collided
// */
void GameScene::beginContact(b2Contact* contact) {
    CULog("CONTACT BEGUN");
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    // See if we have landed on the ground.
    if ((_reynard->getSensorName() == fd2 && _reynard.get() != bd1) ||
        (_reynard->getSensorName() == fd1 && _reynard.get() != bd2)) {
        _reynard->setGrounded(true);
        // Could have more than one ground
        _sensorFixtures.emplace(_reynard.get() == bd1 ? fix2 : fix1);
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
void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    float speed = 0;

    // Use Ian Parberry's method to compute a speed threshold
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
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
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    }
    else {
        dimen *= SCENE_HEIGHT / dimen.height;
    }
    return dimen;
}

/**
 * Render the scene. As this class is a scene and have many child node, calling super render will be enough
 * A placeholder for future usage.
 * @param batch The SpriteBatch to draw with.

 */
void GameScene::render(const std::shared_ptr<SpriteBatch>& batch) {
    Scene2::render(batch);
}
