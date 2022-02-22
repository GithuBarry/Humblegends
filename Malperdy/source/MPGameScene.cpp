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
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
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
#define DEFAULT_GRAVITY -4.9f

/** To automate the loading of crate files */
#define NUM_CRATES 2

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
//float WALL1[] = { 0.0f,  0.0f, 16.0f,  0.0f, 16.0f,  1.0f,
//                  3.0f,  1.0f,  3.0f,  5.0f,  2.0f,  7.0f,
//                  1.0f, 17.0f,  8.0f, 15.0f, 16.0f, 17.0f,
//                 16.0f, 18.0f,  0.0f, 18.0f};
//float WALL2[] = {32.0f, 18.0f, 16.0f, 18.0f, 16.0f, 17.0f,
//                 31.0f, 16.0f, 30.0f, 10.0f, 31.0f,  1.0f,
//                 16.0f,  1.0f, 16.0f,  0.0f, 32.0f,  0.0f};
//float WALL3[] = { 4.0f,  9.5f,  8.0f,  9.5f,
//                  8.0f, 10.5f,  4.0f, 10.5f };

/** The positions of the crate pyramid */
//float BOXES[] = { 14.5f, 14.25f,
//                  13.0f, 12.00f, 16.0f, 12.00f,
//                  11.5f,  9.75f, 14.5f,  9.75f, 17.5f, 9.75f,
//                  13.0f,  7.50f, 16.0f,  7.50f,
//                  11.5f,  5.25f, 14.5f,  5.25f, 17.5f, 5.25f,
//                  10.0f,  3.00f, 13.0f,  3.00f, 16.0f, 3.00f, 19.0f, 3.0f};
//
///** The initial rocket position */
//float ROCK_POS[] = {24,  4};
///** The goal door position */
//float GOAL_POS[] = { 6, 12};

#pragma mark Assset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE       "earth"
/** The key for the rocket texture in the asset manager */
#define ROCK_TEXTURE        "rocket"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE        "goal"
/** The key prefix for the multiple crate assets */
#define CRATE_PREFIX        "crate"
/** The key for the fire textures in the asset manager */
#define MAIN_FIRE_TEXTURE   "flames"
#define RGHT_FIRE_TEXTURE   "flames-right"
#define LEFT_FIRE_TEXTURE   "flames-left"

/** Color to outline the physics nodes */
#define STATIC_COLOR    Color4::WHITE
/** Opacity of the physics outlines */
#define DYNAMIC_COLOR   Color4::YELLOW

/** The key for collisions sounds */
#define COLLISION_SOUND     "bump"
/** The key for the main afterburner sound */
#define MAIN_FIRE_SOUND     "burn"
/** The key for the right afterburner sound */
#define RGHT_FIRE_SOUND     "right-burn"
/** The key for the left afterburner sound */
#define LEFT_FIRE_SOUND     "left-burn"

/** The key for the font reference */
#define PRIMARY_FONT        "retro"

#pragma mark Physics Constants

// Physics constants for initialization
/** Density of non-crate objects */
#define BASIC_DENSITY       0.0f
/** Density of the crate objects */
#define CRATE_DENSITY       1.0f
/** Friction of non-crate objects */
#define BASIC_FRICTION      0.1f
/** Friction of the crate objects */
#define CRATE_FRICTION      0.2f
/** Angular damping of the crate objects */
#define CRATE_DAMPING       1.0f
/** Collision restitution for all objects */
#define BASIC_RESTITUTION   0.1f
/** Threshold for generating sound on collision */
#define SOUND_THRESHOLD     3


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
    return init(assets,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
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
    return init(assets,rect,Vec2(0,DEFAULT_GRAVITY));
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
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _input.init();
    
    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect,gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
        beginContact(contact);
    };
    _world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        beforeSolve(contact,oldManifold);
    };
    
    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);

    // Create the scene graph
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);
    
    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);
//
//    _exitnode = scene2::Label::allocWithText("VICTORY!",_assets->get<Font>(PRIMARY_FONT));
//	_exitnode->setAnchor(Vec2::ANCHOR_CENTER);
//    _exitnode->setPosition(dimen/2.0f);
//    _exitnode->setForeground(STATIC_COLOR);
//    _exitnode->setVisible(false);
    
    addChild(_worldnode);
    addChild(_debugnode);
//    addChild(_exitnode);
    
    populate();
    _active = true;
    _complete = false;
    setDebug(false);
    
    // XNA nostalgia
    Application::get()->setClearColor(Color4f::CORNFLOWER);
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
        _exitnode = nullptr;
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

#pragma mark : Goal door
    std::shared_ptr<Texture> image = _assets->get<Texture>(GOAL_TEXTURE);
    
    // Create obstacle
    Vec2 goalPos = ((Vec2)GOAL_POS);
    Size goalSize = image->getSize()/_scale;
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    
    
    _goalDoor = physics2::BoxObstacle::alloc(goalPos,goalSize);
    _goalDoor->setName("door");
    _goalDoor->setDebugColor(STATIC_COLOR);
    
    // Set the physics attributes
    _goalDoor->setBodyType(b2_staticBody);
    _goalDoor->setDensity(0.0f);
    _goalDoor->setFriction(0.0f);
    _goalDoor->setRestitution(0.0f);
    _goalDoor->setSensor(true);
    
    addObstacle(_goalDoor,sprite); // Put this at the very back
    
#pragma mark : Wall polygon 1
    // Create ground pieces
    // All walls share the same texture
    image  = _assets->get<Texture>(EARTH_TEXTURE);
    std::string wname = "wall";

    // Create the polygon outline
    Poly2 wall1(reinterpret_cast<Vec2*>(WALL1),11);
    EarclipTriangulator triangulator;
    triangulator.set(wall1.vertices);
    triangulator.calculate();
    wall1.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    std::shared_ptr<physics2::PolygonObstacle> wallobj;
    wallobj = physics2::PolygonObstacle::allocWithAnchor(wall1,Vec2::ANCHOR_CENTER);
    wallobj->setDebugColor(STATIC_COLOR);
    wallobj->setName(wname);

    // Set the physics attributes
    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);

    // Add the scene graph nodes to this object
    wall1 *= _scale;
    sprite = scene2::PolygonNode::allocWithTexture(image,wall1);
    addObstacle(wallobj,sprite);  // All walls share the same texture
    
#pragma mark : Wall polygon 2
    Poly2 wall2(reinterpret_cast<Vec2*>(WALL2),9);
    triangulator.set(wall2.vertices);
    triangulator.calculate();
    wall2.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    wallobj = physics2::PolygonObstacle::allocWithAnchor(wall2,Vec2::ANCHOR_CENTER);
    wallobj->setDebugColor(STATIC_COLOR);
    wallobj->setName(wname);
    
    // Set the physics attributes
    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);

    // Add the scene graph nodes to this object
    wall2 *= _scale;
    sprite = scene2::PolygonNode::allocWithTexture(image,wall2);
    addObstacle(wallobj,sprite);  // All walls share the same texture

    
#pragma mark : Wall polygon 3
    Poly2 wall3(reinterpret_cast<Vec2*>(WALL3),4);
    triangulator.set(wall3.vertices);
    triangulator.calculate();
    wall3.setIndices(triangulator.getTriangulation());
    triangulator.clear();

    wallobj = physics2::PolygonObstacle::allocWithAnchor(wall3,Vec2::ANCHOR_CENTER);
    wallobj->setDebugColor(STATIC_COLOR);
    wallobj->setName(wname);

    // Set the physics attributes
    wallobj->setBodyType(b2_staticBody);
    wallobj->setDensity(BASIC_DENSITY);
    wallobj->setFriction(BASIC_FRICTION);
    wallobj->setRestitution(BASIC_RESTITUTION);

    // Add the scene graph nodes to this object
    wall3 *= _scale;
    sprite = scene2::PolygonNode::allocWithTexture(image,wall3);
    addObstacle(wallobj,sprite);  // All walls share the same texture
    
#pragma mark : Crates
    std::srand((int)std::time(0));
    for (int ii = 0; ii < 15; ii++) {
        // Pick a crate and random and generate the key
        int indx = (std::rand() % 2 == 0 ? 2 : 1);
        std::stringstream ss;
        ss << CRATE_PREFIX << (indx < 10 ? "0" : "" ) << indx;

        // Create the sprite for this crate
        image  = _assets->get<Texture>(ss.str());

        Vec2 boxPos(BOXES[2*ii], BOXES[2*ii+1]);
        Size boxSize(image->getSize()/_scale);
        auto crate = physics2::BoxObstacle::alloc(boxPos,boxSize);
        crate->setDebugColor(DYNAMIC_COLOR);
        crate->setName(ss.str());
        crate->setAngleSnap(0);             // Snap to the nearest degree

        // Set the physics attributes
        crate->setDensity(CRATE_DENSITY);
        crate->setFriction(CRATE_FRICTION);
        crate->setAngularDamping(CRATE_DAMPING);
        crate->setRestitution(BASIC_RESTITUTION);

        sprite = scene2::PolygonNode::allocWithTexture(image);
		sprite->setAnchor(Vec2::ANCHOR_CENTER);
        addObstacle(crate,sprite);   // PUT SAME TEXTURES IN SAME LAYER!!!

    }

#pragma mark : Rocket
    Vec2 rockPos = ((Vec2)ROCK_POS);
    image  = _assets->get<Texture>(ROCK_TEXTURE);
    Size rockSize(image->getSize()/_scale);
    
    _rocket = RocketModel::alloc(rockPos,rockSize);
    _rocket->setDrawScale(_scale);
    _rocket->setDebugColor(DYNAMIC_COLOR);
    
    auto rocketNode = scene2::PolygonNode::allocWithTexture(image);
	rocketNode->setAnchor(Vec2::ANCHOR_CENTER);
	_rocket->setShipNode(rocketNode);
    
    // These will attach them to the ship node
    _rocket->setBurnerStrip(RocketModel::Burner::MAIN, _assets->get<Texture>(MAIN_FIRE_TEXTURE));
    _rocket->setBurnerStrip(RocketModel::Burner::LEFT, _assets->get<Texture>(LEFT_FIRE_TEXTURE));
    _rocket->setBurnerStrip(RocketModel::Burner::RIGHT,_assets->get<Texture>(RGHT_FIRE_TEXTURE));
    
    // This just stores the keys
    _rocket->setBurnerSound(RocketModel::Burner::MAIN,  MAIN_FIRE_SOUND);
    _rocket->setBurnerSound(RocketModel::Burner::LEFT,  LEFT_FIRE_SOUND);
    _rocket->setBurnerSound(RocketModel::Burner::RIGHT, RGHT_FIRE_SOUND);

    // Create the polygon node (empty, as the model will initialize)
    _worldnode->addChild(rocketNode);
    _rocket->setDebugScene(_debugnode);
    _world->addObstacle(_rocket);
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
    
    // Position the scene graph node (enough for static objects)
    node->setPosition(obj->getPosition()*_scale);
    _worldnode->addChild(node);
    
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
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
        
    }
    if (_input.didReset()) { reset(); }
    if (_input.didExit())  {
        CULog("Shutting down");
        Application::get()->quit();
    }
    if (_input.didEndSwipe()){
        Vec2 start();
        Vec end();
        start = _input.getSwipeStartEnd()[0];
        end = _input.getSwipeStartEnd()[1];

    }

    // Apply the force to the rocket
    _rocket->setFX(_input.getHorizontal() * _rocket->getThrust());
    _rocket->setFY(_input.getVertical() * _rocket->getThrust());
    _rocket->applyForce();

    // Animate the three burners
    updateBurner(RocketModel::Burner::MAIN,  _rocket->getFY() >  1);
    updateBurner(RocketModel::Burner::LEFT,  _rocket->getFX() >  1);
    updateBurner(RocketModel::Burner::RIGHT, _rocket->getFX() <  -1);

    // Turn the physics engine crank.
    // TODO: Implement https://gafferongames.com/post/fix_your_timestep/
    _world->update(dt);
}

/**
 * Updates that animation for a single burner
 *
 * This method is here instead of the the rocket model because of our philosophy
 * that models should always be lightweight.  Animation includes sounds and other
 * assets that we do not want to process in the model
 *
 * @param  burner   The rocket burner to animate
 * @param  on       Whether to turn the animation on or off
 */
void GameScene::updateBurner(RocketModel::Burner burner, bool on) {
    std::string sound = _rocket->getBurnerSound(burner);
    if (on) {
        _rocket->animateBurner(burner, true);
        if (!AudioEngine::get()->isActive(sound) && sound.size() > 0) {
            auto source = _assets->get<Sound>(sound);
            AudioEngine::get()->play(sound,source,true,source->getVolume());
        }
    } else {
        _rocket->animateBurner(burner, false);
        if (AudioEngine::get()->isActive(sound)) {
            AudioEngine::get()->clear(sound);
        }
    }
    
}

/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */
void GameScene::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    
    // If we hit the "win" door, we are done
    intptr_t rptr = reinterpret_cast<intptr_t>(_rocket.get());
    intptr_t dptr = reinterpret_cast<intptr_t>(_goalDoor.get());

    if((body1->GetUserData().pointer == rptr && body2->GetUserData().pointer == dptr) ||
       (body1->GetUserData().pointer == dptr && body2->GetUserData().pointer == rptr)) {
        setComplete(true);
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
 * @param  oldManfold  	The collision manifold before contact
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
    for(int ii =0; ii < 2; ii++) {
        if (state2[ii] == b2_addState) {
            b2Vec2 wp = worldManifold.points[0];
            b2Vec2 v1 = body1->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 v2 = body2->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 dv = v1-v2;
            speed = b2Dot(dv,worldManifold.normal);
        }
    }
    
    // Play a sound if above threshold
    if (speed > SOUND_THRESHOLD) {
        // These keys result in a low number of sounds.  Too many == distortion.
        physics2::Obstacle* data1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
        physics2::Obstacle* data2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

        if (data1 != nullptr && data2 != nullptr) {
            std::string key = (data1->getName()+data2->getName());
            auto source = _assets->get<Sound>(COLLISION_SOUND);
            if (!AudioEngine::get()->isActive(key)) {
                AudioEngine::get()->play(key, source, false, source->getVolume());
            }
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
    float ratio1 = dimen.width/dimen.height;
    float ratio2 = ((float)SCENE_WIDTH)/((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH/dimen.width;
    } else {
        dimen *= SCENE_HEIGHT/dimen.height;
    }
    return dimen;
}
