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

#pragma mark -
#pragma mark Size Constants

using namespace cugl;


class TrapModel : public cugl::scene2::SceneNode {


public:
    
    /** The potential activation states of the trap. */
    enum class TrapState : int{
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
    
    

protected:
#pragma mark -
#pragma mark Constants

    /** The texture for the TRAP   */
    const string TRAP_TEXTURE;

    
#pragma mark Attributes

    /** A uniform value represneting scale between the physics world and the screen */
    float _drawScale;
    
    /** The subtype of the trap */
    TrapType _type;

    /** The current activation state of the trap. */
    TrapState _trapState;
        
    /** The obstacle representing the physical entity for the trap */
    shared_ptr<cugl::physics2::PolygonObstacle> _obstacle;
    
    /** The polynode (alternative for) representing the physical entity for the trap */
    shared_ptr<cugl::scene2::PolygonNode> _polyNode;

    

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
        return _polyNode->getPolygon();
        
    }
    
    shared_ptr<scene2::PolygonNode> getPolyNode(){
        return _polyNode;
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
    virtual void setTrapState(TrapState newState){
        _trapState = newState;
    }
    
    TrapType getType(){
        return _type;
    }
    
        
    
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

//    /**
//     * Updates the object's physics state (NOT GAME LOGIC).
//     *
//     * We use this method to reset cooldowns.
//     *
//     * @param delta Number of seconds since last animation frame
//     */
//    virtual void update(float dt);

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
