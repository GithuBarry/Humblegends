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
#include "MPRocketModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Animation and Physics Constants

/** This is adjusted by screen aspect ratio to get the height */
#define GAME_WIDTH 1024

/** The number of frames for the afterburner */
#define FIRE_FRAMES     4

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.1f
/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new rocket with the given position and size.
 *
 * The rocket size is specified in world coordinates.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size   	The dimensions of the box.
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool RocketModel::init(const Vec2 pos, const Size size) {
    physics2::BoxObstacle::init(pos,size);
    std::string name("rocket");
    setName(name);
    
    _shipNode = nullptr;
    
    _mainBurner = nullptr;
    _mainSound  = "";
    _mainCycle  = true;
    
    _leftBurner = nullptr;
    _leftSound  = "";
    _leftCycle  = true;

    _rghtBurner = nullptr;
    _rghtSound  = "";
    _rghtCycle  = true;

    setDensity(DEFAULT_DENSITY);
    setFriction(DEFAULT_FRICTION);
    setRestitution(DEFAULT_RESTITUTION);
    setFixedRotation(true);
    
    return true;
}


/**
 * Disposes all resources and assets of this rocket
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a rocket may not be used until it is initialized again.
 */
void RocketModel::dispose() {
    _shipNode = nullptr;
    _mainBurner = nullptr;
    _leftBurner = nullptr;
    _rghtBurner = nullptr;
}


#pragma mark -
#pragma mark Physics
/**
 * Applies the force to the body of this ship
 *
 * This method should be called after the force attribute is set.
 */
void RocketModel::applyForce() {
    if (!isEnabled()) {
        return;
    }
    
    // Orient the force with rotation.
    Vec4 netforce(_force.x,_force.y,0.0f,1.0f);
    Mat4::createRotationZ(getAngle(),&_affine);
    netforce *= _affine;
    
    // Apply force to the rocket BODY, not the rocket
    _body->ApplyForceToCenter(b2Vec2(netforce.x,netforce.y), true);
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
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
void RocketModel::update(float delta) {
    Obstacle::update(delta);
    if (_shipNode != nullptr) {
        _shipNode->setPosition(getPosition()*_drawscale);
        _shipNode->setAngle(getAngle());
    }
}


#pragma mark -
#pragma mark Animation
/**
 * Sets the scene graph node representing this rocket.
 *
 * By storing a reference to the scene graph node, the model can update
 * the node to be in sync with the physics info. It does this via the
 * {@link Obstacle#update(float)} method.
 *
 * If the animation nodes are not null, this method will remove them from
 * the previous scene and add them to the new one.
 *
 * @param node  The scene graph node representing this rocket.
 */
void RocketModel::setShipNode(const std::shared_ptr<scene2::SceneNode>& node) {
    if (_shipNode != nullptr) {
        if (_mainBurner != nullptr) {
            _shipNode->removeChild(_mainBurner);
        }
        if (_leftBurner != nullptr) {
            _shipNode->removeChild(_leftBurner);
        }
        if (_rghtBurner != nullptr) {
            _shipNode->removeChild(_rghtBurner);
        }
    }
    _shipNode = node;
    if (_mainBurner != nullptr) {
        _mainBurner->setPosition(_shipNode->getContentSize().width/2.0f,
                                 _shipNode->getContentSize().height-
                                 _mainBurner->getContentSize().height/2.0f);
        _shipNode->addChild(_mainBurner);
    }
    if (_leftBurner != nullptr) {
        _leftBurner->setPosition(_shipNode->getContentSize()/2.0f);
        _shipNode->addChild(_leftBurner);
    }
    if (_rghtBurner != nullptr) {
        _rghtBurner->setPosition(_shipNode->getContentSize()/2.0f);
        _shipNode->addChild(_rghtBurner);
    }
}


/**
 * Returns the animation node for the given afterburner
 *
 * The model tracks the animation nodes separately from the main scene
 * graph node (even though they are childing of this node).  That is so
 * we can encapsulate the animation cycle.
 *
 * @param  burner   The enumeration to identify the afterburner
 *
 * @return the animation node for the given afterburner
 */
const std::shared_ptr<scene2::SpriteNode>& RocketModel::getBurnerStrip(Burner burner) const {
    switch (burner) {
        case Burner::MAIN:
            return _mainBurner;
        case Burner::LEFT:
            return _leftBurner;
        case Burner::RIGHT:
            return _rghtBurner;
    }
    CUAssertLog(false, "Invalid burner enumeration");
	return _mainBurner;
}

/**
 * Sets the animation node for the given afterburner
 *
 * The model tracks the animation nodes separately from the main scene
 * graph node (even though they are childing of this node).  That is so
 * we can encapsulate the animation cycle.
 * @param burner    The enumeration to identify the afterburner
 * @param strip     The animation node for the given afterburner
 */
void RocketModel::setBurnerStrip(Burner burner, const std::shared_ptr<cugl::Texture>& strip) {
    switch (burner) {
        case Burner::MAIN:
            _mainBurner = scene2::SpriteNode::alloc(strip, 1, FIRE_FRAMES, FIRE_FRAMES);
            if (_shipNode != nullptr) {
                _mainBurner->setPosition(_shipNode->getContentSize().width/2.0f,
                                         _shipNode->getContentSize().height-
                                         _mainBurner->getContentSize().height/2.0f);
                _shipNode->addChild(_mainBurner);
            }
            break;
        case Burner::LEFT:
            _leftBurner = scene2::SpriteNode::alloc(strip, 1, FIRE_FRAMES, FIRE_FRAMES);
            if (_shipNode != nullptr) {
                _leftBurner->setPosition(_shipNode->getContentSize()/2.0f);
                _shipNode->addChild(_leftBurner);
            }
            break;
        case Burner::RIGHT:
            _rghtBurner = scene2::SpriteNode::alloc(strip, 1, FIRE_FRAMES, FIRE_FRAMES);
            if (_shipNode != nullptr) {
                _rghtBurner->setPosition(_shipNode->getContentSize()/2.0f);
                _shipNode->addChild(_rghtBurner);
            }
            break;
        default:
            CUAssertLog(false, "Invalid burner enumeration");
    }
}

/**
 * Returns the key for the sound to accompany the given afterburner
 *
 * The key should either refer to a valid sound loaded in the AssetManager or
 * be empty ("").  If the key is "", then no sound will play.
 *
 * @param burner    The enumeration to identify the afterburner
 *
 * @return the key for the sound to accompany the given afterburner
 */
const std::string& RocketModel::getBurnerSound(Burner burner) const {
    switch (burner) {
        case Burner::MAIN:
            return _mainSound;
        case Burner::LEFT:
            return _leftSound;
        case Burner::RIGHT:
            return _rghtSound;
    }
    CUAssertLog(false, "Invalid burner enumeration");
	return _mainSound;
}

/**
 * Sets the key for the sound to accompany the given afterburner
 *
 * The key should either refer to a valid sound loaded in the AssetManager or
 * be empty ("").  If the key is "", then no sound will play.
 *
 * @param burner    The enumeration to identify the afterburner
 * @param key       The key for the sound to accompany the main afterburner
 */
void RocketModel::setBurnerSound(Burner burner, const std::string& key) {
    switch (burner) {
        case Burner::MAIN:
            _mainSound = key;
            break;
        case Burner::LEFT:
            _leftSound = key;
            break;
        case Burner::RIGHT:
            _rghtSound = key;
            break;
        default:
            CUAssertLog(false, "Invalid burner enumeration");
    }
}

/**
 * Animates the given burner.
 *
 * If the animation is not active, it will reset to the initial animation frame.
 *
 * @param burner    The reference to the rocket burner
 * @param on        Whether the animation is active
 */
void RocketModel::animateBurner(Burner burner, bool on) {
    scene2::SpriteNode* node;
    bool*  cycle;
    
    switch (burner) {
        case Burner::MAIN:
            node  = _mainBurner.get();
            cycle = &_mainCycle;
            break;
        case Burner::LEFT:
            node  = _leftBurner.get();
            cycle = &_leftCycle;
            break;
        case Burner::RIGHT:
            node  = _rghtBurner.get();
            cycle = &_rghtCycle;
            break;
    }
    
    if (on) {
        // Turn on the flames and go back and forth
        if (node->getFrame() == 0 || node->getFrame() == 1) {
            *cycle = true;
        } else if (node->getFrame() == node->getSize()-1) {
            *cycle = false;
        }
        
        // Increment
        if (*cycle) {
            node->setFrame(node->getFrame()+1);
        } else {
            node->setFrame(node->getFrame()-1);
        }
    } else {
        node->setFrame(0);
    }
}

/**
 * Sets the ratio of the ship sprite to the physics body
 *
 * The rocket needs this value to convert correctly between the physics
 * coordinates and the drawing screen coordinates.  Otherwise it will
 * interpret one Box2D unit as one pixel.
 *
 * All physics scaling must be uniform.  Rotation does weird things when
 * attempting to scale physics by a non-uniform factor.
 *
 * @param scale The ratio of the ship sprite to the physics body
 */
void RocketModel::setDrawScale(float scale) {
    _drawscale = scale;
    if (_shipNode != nullptr) {
        _shipNode->setPosition(getPosition()*_drawscale);
    }
}

