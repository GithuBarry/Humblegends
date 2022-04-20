//
//  MPTrapModel.cpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Evan Azari, Abu Qader, Jordan Selin
//  Version: 4/16/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPTrapModel.hpp"

using namespace cugl;
using namespace cugl::physics2;

#pragma mark -
#pragma mark Constructors


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

    addChild(_polyNode);

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
            uploadTexture("spawn");
            break;
        case TrapModel::TrapState::DEACTIVATED:
            uploadTexture("deactivated");
//            _obstacle->setSensor(false);
            break;
        case TrapModel::TrapState::ACTIVATED:
            uploadTexture("activated");
//            _obstacle->setSensor(true);
            break;
    }
    
    _trapState = newState;
}

bool TrapModel::uploadTexture(string tex) {
    
    // If the animation doesn't exist, return false
    if (!(*_animations).count(tex)) return false;
    
    // Jump to the beginning of the animation
    _currFrame = 0;
    _elapsed = 0;
    
    // Make a new node for the animation
    shared_ptr<scene2::SpriteNode> newNode = make_shared<scene2::SpriteNode>();
    newNode->initWithSprite((*_animations)[tex]._frames, (*_animations)[tex]._rows, (*_animations)[tex]._cols, (*_animations)[tex]._size);
    
    // Add the node to the scenegraph, and then delete the old node
    scene2::SceneNode* p = _polyNode->getParent();
    p->addChild(newNode);
    _polyNode->SceneNode::dispose();
    _polyNode = newNode;
    _polyNode->setPosition(getPosition() * _drawScale);
    
    // Update the current Animation class
    _currAnimation = (*_animations)[tex];
//    _polyNode->setAnchor(0.5, 0.5);
    
    _polyNode->setScale(_polyNode->getScale()*Vec2(1,1));
    
    return true;
}

//void TrapModel::update(float dt) {
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
//    // UPDATE THE ANIMATION
//    
//    // update time since last frame update
//    _elapsed += dt;
//    
//    if (_trapState == TrapState::ACTIVATED || _trapState == TrapState::DEACTIVATED) {
//        // update time since last frame update
//            
//            // if it is time to update the frame...
//            float frame_time = FRAME_TIME * ((_trapState == TrapState::ACTIVATED) ? 2.0 : 1.0);
//            if (_elapsed > frame_time ) {
//
//                // if on the last frame
//                if (_currFrame >= _polyNode->getSize()-1){
//                    // loop the animation if needed
//                    if(_currAnimation._loop){
//                        _currFrame = 0;
//                    }
//                }
//                // if not on the last frame, then increment
//                else{
//                    _currFrame = _currFrame + 1;
//                }
//                _polyNode->setFrame(_currFrame);
//                // reset time since last frame update
//                _elapsed = 0;
//            }
//        }
//
//        // if it is time to update the frame...
////        float frame_time = FRAME_TIME * ((_moveState == MovementState::JUMPING) ? 2.0 : 1.0);
////        if (_elapsed > frame_time ) {
////
////            // if on the last frame
////            if (_currFrame >= _node->getSize()-1){
////                // loop the animation if needed
////                if(_currAnimation._loop){
////                    _currFrame = 0;
////                }
////            }
////            // if not on the last frame, then increment
////            else{
////                _currFrame = _currFrame + 1;
////            }
////            _node->setFrame(_currFrame);
////            // reset time since last frame update
////            _elapsed = 0;
////        }
//    }
//    
//}
//
//
