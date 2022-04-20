//
//  MPTrapModel.hpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader, Jordan Selin
//  Version: 4/16/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPTrapModel_hpp
#define MPTrapModel_hpp

#include <stdio.h>
#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <map>

using namespace cugl;


class TrapModel : public cugl::scene2::SceneNode {


public:
    
    /** The potential activation states of the trap. */
    enum class TrapState : int{
        SPAWN,
        ACTIVATED,
        DEACTIVATED
    };

    enum class TrapType : int {
        UNTYPED,
        SPIKE,
        TRAPDOOR,
        SAP,
        BRAZIER
    };
    
    /** Class representing an animation */
    class Animation{
    public:
        // The sprite sheet
        shared_ptr<Texture> _frames;
        
        // Frame data
        int _size;
        int _cols;
        int _rows;
        bool _loop = false;
        
        // Empty constructor, must initialize to usse
        Animation(){};
        
        // Sets all attributes
        bool init(shared_ptr<Texture> frames, int size, int cols, string loop){
            // Frame data
            _frames = frames;
            _size = size;
            _cols = cols;
            // Calculate the number of rows from size & cols
            _rows = (_size-1) / _cols + 1;
            if (loop == "true") _loop = true;
            
            // return false if spritesheet is null or the size is nonpositive
            return frames && size > 0;
        }
    };

protected:
#pragma mark -
#pragma mark Constants

    /** The texture for the TRAP   */
    const string TRAP_TEXTURE;
    
    /** The amount of time in between each frame update */
    const float FRAME_TIME = 0.03;


    
    
#pragma mark Attributes

    /** A uniform value represneting scale between the physics world and the screen */
    float _drawScale;
    
    /** The subtype of the trap */
    TrapType _type;

    /** The current activation state of the trap. */
    TrapState _trapState = TrapState::SPAWN;
        
    /** The obstacle representing the physical entity for the trap */
    shared_ptr<cugl::physics2::PolygonObstacle> _obstacle;
    
    /** The polynode (alternative for) representing the physical entity for the trap */
    shared_ptr<cugl::scene2::SpriteNode> _sceneNode;
    
    // ANIMATION RELATED ATTRIBUTES
    /** The amount of time since last frame update */
    float _elapsed = 0;

    /** represents the actual frame of animation, invariant to texture flips */
    int _currFrame = 0;

    /** The dictionary of all character animations */
    shared_ptr<map<string, Animation>> _animations;
    
    /** The frame data the current animation */
    Animation _currAnimation;

    

//    /**
//    * Redraws the outline of the physics fixtures to the debug node
//    *
//    * The debug node is use to outline the fixtures attached to this object.
//    * This is very useful when the fixtures have a very different shape than
//    * the texture (e.g. a circular shape attached to a square texture).
//    */
//    virtual void resetDebug();

public:
#pragma mark -
#pragma mark Hidden Constructors
    
    /**
     * Initializes a trap with the given characteristics in a given location in Room Space.
     *
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x         The x-coordinate of the trap in room space
     * @param y         The y-coordinate of the room in room space
     *
     * @return     Returns True if the space is initialized properly.
     */
    bool init();
    //TODO: This needs to be verified to be in the room space coords and not world space coords
    
    bool initObstacle(shared_ptr<physics2::PolygonObstacle> bo){
        _obstacle = bo;
        _obstacle->createFixtures();
        return true;
    }
    
    
    
#pragma mark -
#pragma mark Attribute Properties

    // PolygonObstacle Section:
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @return      Whether the change happened successfully
     */
    shared_ptr<cugl::physics2::PolygonObstacle> getObstacle(){
        return _obstacle;
    }
    
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @param x           Float representing x Position
     * @param y           Float representing y Position
     */
    void setPhysicalBodyPos(float x, float y){
        _obstacle->setPosition(x, y);
    }
    
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @param pos           Vec2 with pos.x representing x location and pos.y representing y
     */
    void setPhysicalBodyPos(Vec2 pos){
        _obstacle->setPosition(pos);
    }
    
    // Polygon Node Section:
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @return      Whether the change happened successfully
     */
    Poly2 getImageBody(){
        return _sceneNode->getPolygon();
        
    }
    
    shared_ptr<scene2::PolygonNode> getPolyNode(){
        return _sceneNode;
    }
    
    // Trap State Section
    /**
     * Returns true if the trap is currently activated.
     *
     * @return true if the trap is currently active and can kill enemies and Reynard.
     */
    bool isActivated() const {
        return (_trapState == TrapState::ACTIVATED);
    }
    
    /**
     * Sets the traps's state, changing physical attributes of the trap.
     *
     * @param state The new state the trap should be in
     * @return      Whether the change happened successfully
     */
    virtual void setTrapState(TrapState newState);
    
    
    TrapType getType(){
        return _type;
    }
    
#pragma mark -
#pragma mark ANIMATION Methods

    /**
     * Replaces the node with the specified animation
     *
     * @param tex is the key representing an animation in _animations
     *
     * @returns whether or not the animation was uplaoded
     */
    bool uploadTrapTexture(string tex);
    
    
#pragma mark -
#pragma mark Physics Methods

    
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    virtual void createFixtures();

    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    virtual void releaseFixtures();

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    void update(float dt);

    bool uploadTexture(string tex); 
    
#pragma mark Destructors
    /**
     * Destroys this room, releasing all resources.
     */
    virtual ~TrapModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this room.
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a room may not be used until it is initialized again.
     */
    void dispose(){
        removeAllChildren();
        _obstacle = nullptr;
    };
};




#endif /* MPTrapModel_hpp */
