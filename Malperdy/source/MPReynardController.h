//
// Created by Spencer Hurst on 2/25/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MALPERDY_MPREYNARDCONTROLLER_H
#define MALPERDY_MPREYNARDCONTROLLER_H

#include <cugl/cugl.h>
#include "MPCharacterModel.h"
#include "MPReynardModel.h"

class MPReynardController {

private:
    ReynardModel _reynard;

public:
    /**This is the constructor of the Reynard Controller**/
    MPReynardController(ReynardModel r);

    /**This allows someone to grab the instantiated Reynard from this controller**/
    ReynardModel getReynard(){
        return _reynard;
    }

    /** [update] This will automatically update Reynard's position and look at other
     * things in the future like health.*/
    void update();

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

    /**This function is what is jused to check if Reynard could jump at a given time and if he
     * can jump it makes him jump and return true (if he cannot it will return false)
     * If Reynard's state is already jumping**/
    bool resolveJump();
    
    /**This function is what is jused to check if Reynard could jump at a given time and if he
     * can jump it makes him jump and return true (if he cannot it will return false)
     * If Reynard's state is already jumping**/
//    bool resolveDash();

};


#endif //MALPERDY_MPREYNARDCONTROLLER_H
