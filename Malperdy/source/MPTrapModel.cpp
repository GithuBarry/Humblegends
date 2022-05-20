//
//  MPTrapModel.cpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader
//  Version: 3/10/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPTrapModel.hpp"

using namespace cugl;
using namespace cugl::physics2;

#pragma mark -
#pragma mark Constructors

shared_ptr<AssetManager> TrapModel::ASSETS = make_shared<AssetManager>();


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
bool TrapModel::init()
{

    addChild(_sceneNode);

    _trapState = TrapState::DEACTIVATED;

    return this->scene2::SceneNode::init();
}

/**
 * Creates the physics Body(s) for this object, adding them to the world.
 *
 * This method overrides the base method to keep your ship from spinning.
 *
 * @param world Box2D world to store body
 *
 * @return true if object allocation succeeded
 */
void TrapModel::createFixtures() {
    _obstacle->createFixtures();
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void TrapModel::releaseFixtures() {
    _obstacle->releaseFixtures();
}

void TrapModel::setTrapState(TrapState newState){
    //In case we ever want actions based on current state --> new state.
    switch (_trapState) {
        case TrapState::SPAWN:
            break;
        case TrapState::DEACTIVATED:
            break;
        case TrapState::ACTIVATED:
            break;
    }
    
    //In case we ever want actions based on current state --> new state.
    switch (newState) {
        case TrapState::SPAWN:
            
            break;
        case TrapModel::TrapState::DEACTIVATED:
            
//            _obstacle->setSensor(false);
            break;
        case TrapModel::TrapState::ACTIVATED:

//            _obstacle->setSensor(true);
            break;
    }
    
    _trapState = newState;
}


void TrapModel::update(float dt) {
//    switch(_trapState){
//        case TrapState::SPAWN:
//            setTrapState(TrapState::SPAWN);
//            break;
//        case TrapState::ACTIVATED:
//            setTrapState(TrapState::ACTIVATED);
//            break;
//        case TrapState::DEACTIVATED:
//            setTrapState(TrapState::DEACTIVATED);
//
//            break;
//    }

    // UPDATE THE ANIMATION
    // update time since last frame update
    _elapsed += dt;

    if (_sceneNode->getSize()) {
//        cout << "activated";
    }
    
    if(_type == TrapType::CHECKPOINT){
        if (_trapState == TrapState::ACTIVATED && _sceneNode->getSize()) {
            // update time since last frame update


                // if it is time to update the frame...
                float frame_time = FRAME_TIME;
                if (_elapsed > frame_time ) {

                    // if on the last frame
                    if (_currFrame == 0){
                        // loop the animation if needed
       
                    }
                    // if not on the last frame, then increment
                    else{
                        _currFrame = _currFrame - 1;
                        
                    }
                    _sceneNode->setFrame(_currFrame);
                    // reset time since last frame update
                    _elapsed = 0;
                }
            }
    }
    
    else{
    if (_trapState == TrapState::ACTIVATED && _sceneNode->getSize()) {
        // update time since last frame update


            // if it is time to update the frame...
            float frame_time = FRAME_TIME;
            if (_elapsed > frame_time ) {

                // if on the last frame
                if (_currFrame >= _sceneNode->getSize()-1){
                    // loop the animation if needed
                    _sceneNode->setFrame(0);
                    return;
   
                }
                // if not on the last frame, then increment
                else{
                    _currFrame = _currFrame + 1;
                    
                }
                _sceneNode->setFrame(_currFrame);
                // reset time since last frame update
                _elapsed = 0;
            }
        }
    }
}


