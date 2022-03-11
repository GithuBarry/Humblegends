//
//  MPReynardController.cpp
//  Malperdy
//
//  Owner: Spencer Hurst
//  Contributors: Spencer Hurst
//  Version: 2/25/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#define VELOCITY    10
#define DUDE_JUMP   5.5f

#include "MPReynardController.h"

// TODO: construct a ReynardModel within the ReynardController
/**This is the constructor of the Reynard Controller**/
ReynardController::ReynardController(shared_ptr<ReynardModel> r){
    _reynard = r;
}

// TODO: add alloc method

//WARNING: This may lead to a double call of update if reynard himself is having his update called.

/**This function contains a giant switch statement between what should happen given
 * the various character modes of the character. This includes his Jumping, Dashing, Running,
 * and slowed (such as if he is zoomed out looking at the map).
 * The function has little to do with his actual movement and more to do with the animations that will be placed here
 * in addition to sprite batch code to be sent in at a point later. **/
void ReynardController::updateMode(ReynardModel::ReynardState c){
    switch (c){
        case ReynardModel::ReynardState::SPAWN:
            _reynard->setCurrentState(ReynardModel::ReynardState::SPAWN);
            break;
        case ReynardModel::ReynardState::MOVING:
            _reynard->setCurrentState(ReynardModel::ReynardState::MOVING);
            break;
        case ReynardModel::ReynardState::SLOWMOVING:
            _reynard->setCurrentState(ReynardModel::ReynardState::SLOWMOVING);
            break;
        case ReynardModel::ReynardState::DASHING:
            _reynard->setCurrentState(ReynardModel::ReynardState::DASHING);
            break;
        case ReynardModel::ReynardState::JUMPING:
            _reynard->setCurrentState(ReynardModel::ReynardState::JUMPING);
            break;
        case ReynardModel::ReynardState::TRAPPED:
            _reynard->setCurrentState(ReynardModel::ReynardState::TRAPPED);
            break;
    }
}

/**Whenever Reynard hits a horizontal wall, this function is called and turns him around
 * His velocity will be turned to whatever the negative of whatever the Constant his speed
 * should be. **/
void ReynardController::switchDirection(){
    if (!_reynard->getFallingOffWall()) _reynard->setFacingRight(!(_reynard->isFacingRight()));
    
}

/**The beauty of this function is that it will always apply this internal variable which is what is updated directly
 * in the Switch Directions functoin.. **/
void ReynardController::resolveRunning(){
    _reynard->setMovement(_currentMovement);
}


/**This function is what is jused to check if Reynard could jump at a given time and if he
 * can jump it makes him jump and return true (if he cannot it will return false)
 * If Reynard's state is already jumping**/
bool ReynardController::resolveJump(){
//    return true;
    bool r = _reynard->applyJumpForce();
    //cout<<"apply JF"<<endl;
    return r;
}

bool ReynardController::resolveDash(){
//    return true; 
    bool r = _reynard->applyDashForce();
    return r;
}

/**
 * Sets Reynard to be sliding down a wall for a short amount of time.
 *
 * @return  Whether Reynard is successfully stuck to the wall
 */
bool ReynardController::stickToWall() {
    _reynard->setOnWall(true);
    return true;
}

/**
 * If Reynard is sliding down a wall already, unsticks him and drops
 * him to the ground.
 *
 * @return  Whether Reynard is successfully unstuck from the wall
 */
bool ReynardController::unstickFromWall() {
    _reynard->setOnWall(false);
    return true;
}

/**
 * Sets Reynard to be on the ground.
 *
 * @return  Whether Reynard is successfully marked as being on the ground
 */
bool ReynardController::landOnGround() {
    _reynard->setGrounded(true);
    return true;
}

/** [update] This will automatically update Reynard's position and look at other
 * things in the future like health.*/
void ReynardController::update(float delta){
    _reynard->setMovement(_reynard->getMovement() * _reynard->getForce());
    _reynard->applyForce();
    _reynard->update(delta);

    if (_lastPositionValid) {
        _delta = _reynard->getPosition() - _lastPosition;
    } else {
        _delta = Vec2(0, 0);
        _lastPositionValid = true;
    }
    _lastPosition = _reynard->getPosition();
}
