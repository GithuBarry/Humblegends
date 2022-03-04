//
// Created by Spencer Hurst on 2/25/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MALPERDY_MPREYNARDCONTROLLER_H
#define MALPERDY_MPREYNARDCONTROLLER_H

#define MOVEMENT_FORCE 5.0F


#include <cugl/cugl.h>
#include "MPCharacterModel.h"
#include "MPReynardModel.h"

class ReynardController {

private:
    shared_ptr<ReynardModel> _reynard;
    float _currentMovement = MOVEMENT_FORCE;

public:
    /**This is the constructor of the Reynard Controller**/
    ReynardController(shared_ptr<ReynardModel> r);

    /**This allows someone to grab the instantiated Reynard from this controller**/
    shared_ptr<ReynardModel> getReynard(){ return _reynard; }

    /** [update] This will automatically update Reynard's position and look at other
     * things in the future like health.*/
    void update(float delta);

    /**This function contains a giant switch statement between what should happen given
     * the various character modes of the character. This includes his Jumping, Dashing, Running,
     * and slowed (such as if he is zoomed out looking at the map).
     * [IF you want to change REYNARD and one of his interactions to something within the world
     * YOU DO THAT HERE]**/
    void updateMode(ReynardModel::ReynardState c);

    /**Whenever Reynard hits a horizontal wall, this function is called and turns him around
     * His velocity will be turned to whatever the negative of whatever the Constant his speed
     * should be. **/
    void switchDirection();
    
    /**Resolve the forces meant to constantly be acted upon him and make sure it automatically
     * automatically sets his velocity every frame.**/
    void resolveRunning();

    /**This function is what is jused to check if Reynard could jump at a given time and if he
     * can jump it makes him jump and return true (if he cannot it will return false)
     * If Reynard's state is already jumping**/
    bool resolveJump();
    
    /**This function is what is jused to check if Reynard could jump at a given time and if he
     * can jump it makes him jump and return true (if he cannot it will return false)
     * If Reynard's state is already jumping**/
    bool resolveDash();

    /**
     * Sets Reynard to be sliding down a wall for a short amount of time.
     * 
     * @return  Whether Reynard is successfully stuck to the wall
     */
    bool stickToWall();

    /**
     * If Reynard is sliding down a wall already, unsticks him and drops
     * him to the ground.
     *
     * @return  Whether Reynard is successfully unstuck from the wall
     */
    bool unstickFromWall();

    /**
     * Sets Reynard to be on the ground.
     * 
     * @return  Whether Reynard is successfully marked as being on the ground
     */
    bool landOnGround();

};


#endif //MALPERDY_MPREYNARDCONTROLLER_H
