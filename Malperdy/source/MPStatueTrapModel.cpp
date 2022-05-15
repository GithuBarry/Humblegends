//
//  MPStatueTrapModel.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 5/3/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#include "MPStatueTrapModel.hpp"

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPSapTrap.hpp"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

#define ARROW_INTERVAL 20.0f
#define ARROW_DAMAGE 1.0f
#define ARROW_SPEED 10.0f


shared_ptr<physics2::ObstacleWorld> StatueTrap::_world = make_shared<physics2::ObstacleWorld>();
float StatueTrap::_scale = 0.0f;


/**
 * Initializes a new trap at the given position
 *
 * Currently hard coded spawn in across bottom of the room.
 *
 * @return  true if the trap is initialized properly, false otherwise.
 */
bool StatueTrap::init(float roomWidth, float roomHeight){
    _sceneNode = make_shared<scene2::SpriteNode>();
    _sceneNode->initWithFile("textures/StatueTrap.png");
    _type = TrapType::STATUE;
    
    _sceneNode->setAnchor(Vec2::ZERO);
    _sceneNode->setScale(.5,.5);
//    _sceneNode->setScale(10,10);
//    _sceneNode->setScale((roomWidth/2) / _sceneNode->getPolygon().getBounds().getMaxX());
    _sceneNode->setPosition(_sceneNode->getPosition().x +(roomWidth/3), _sceneNode->getPosition().y + (roomHeight/8));
    _sceneNode->setAbsolute(true);

//    _sceneNode->getWorldPosition()/_scale;
    
    return this->TrapModel::init();
}


//TODO: LIST
//1. Make the Arrow nestesd Class in here (Child of box obstacle)
//      -Look at how reynard works
//2. When instantiating the arrow Link the scenenode to physkcs obj
//  Set the physics object's location to be next to the statue's (getWorldPos/_scale)
//  Set the Arrow Position to that value
//  setSceneNode (from Character Model)
//  Seperately create an update function (inside this the setSceneNode)
//  -- LOOK AT WHAT KRISTINA SENT ME
//  -- ISSUES WITH COLLISION
//          LOOK AT THE CHECKPOINT COLLISION CODE 


//  LOOK AT addObs in game scene
//  Already box obscacle
//  CREATE SCENENODE (ALLOC WITH TEXTURE) // INITWITHTFILE
//  Arrow.alloc Create SceneNode (Creates an object and returns a pointer to it)
//      Link the scenenode to Physical obstacle (Look at setSceneNode)
//      MAY NEED AN UPDATE FUNCTION
//      (LOOK AT HOW TO OVERRIDE THIS FUNCTION) (takes in float dt)
//3. Transform world space to physics space
//Get ARROW TEXTURE:
//      -Set physics properties--> setArrow properties
//      -ScaleWorld and Arrow texture
//      -Need to make the world and texture static member variable
//      -USE SETSCENENODE
//      -Go to character model.h and look at its setSceneNode
//Create a internal Class have a Scenenode of the
//ADJUST THE BOX OBSTACLE TO THE SCENENODE POSITION

//Arrow needs to exist and move at a constant speed at its direction
//If it hits something it despawns.
//Will spawn continuously at certain intervals

//Hit Walls, Reynard, Enemies
//Despawning
