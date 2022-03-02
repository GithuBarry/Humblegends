//
// Created by Spencer Hurst on 2/25/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#define VELOCITY    10
#define DUDE_JUMP   5.5f

#include "MPReynardController.h"

/**This is the constructor of the Reynard Controller**/
ReynardController::ReynardController(ReynardModel r){
    _reynard = r;
}


/** [update] This will automatically update Reynard's position and look at other
 * things in the future like health.*/
void ReynardController::update(float delta){
    _reynard.update(delta);
}
//WARNING: This may lead to a double call of update if reynard himself is having his update called.

/**This function contains a giant switch statement between what should happen given
 * the various character modes of the character. This includes his Jumping, Dashing, Running,
 * and slowed (such as if he is zoomed out looking at the map).
 * The function has little to do with his actual movement and more to do with the animations that will be placed here
 * in addition to sprite batch code to be sent in at a point later. **/
void ReynardController::updateMode(ReynardModel::ReynardState c){
    switch (c){
        case ReynardModel::ReynardState::SPAWN:
            _reynard.setCurrentState(ReynardModel::ReynardState::SPAWN);
            break;
        case ReynardModel::ReynardState::MOVING:
            _reynard.setCurrentState(ReynardModel::ReynardState::MOVING);
            break;
        case ReynardModel::ReynardState::SLOWMOVING:
            _reynard.setCurrentState(ReynardModel::ReynardState::SLOWMOVING);
            break;
        case ReynardModel::ReynardState::DASHING:
            _reynard.setCurrentState(ReynardModel::ReynardState::DASHING);
            break;
        case ReynardModel::ReynardState::JUMPING:
            _reynard.setCurrentState(ReynardModel::ReynardState::JUMPING);
            break;
        case ReynardModel::ReynardState::TRAPPED:
            _reynard.setCurrentState(ReynardModel::ReynardState::TRAPPED);
            break;
    }
}

/**Whenever Reynard hits a horizontal wall, this function is called and turns him around
 * His velocity will be turned to whatever the negative of whatever the Constant his speed
 * should be. **/
void ReynardController::switchDirection(){
    _currentMovement = -_reynard.getMovement();
    _reynard.setMovement(-_currentMovement);
}

/**The beauty of this function is that it will always apply this internal variable which is what is updated directly
 * in the Switch Directions functoin.. **/
void ReynardController::resolveRunning(){
//    The beauty of this is
    _reynard.setMovement(_currentMovement);
}


/**This function is what is jused to check if Reynard could jump at a given time and if he
 * can jump it makes him jump and return true (if he cannot it will return false)
 * If Reynard's state is already jumping**/
bool ReynardController::resolveJump(){
    bool r = _reynard.applyJumpForce();
    return r;
}

bool ReynardController::resolveDash(){
    bool r = _reynard.applyDashForce();
    return r;
}

