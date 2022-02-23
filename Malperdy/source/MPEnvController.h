//
//  MPEnvController.h
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Authors: Barry Wang, Jordan Selin
//  Copyright © 2022 Humblegends. All rights reserved.
//  Version: 2/23/22
//

#ifndef MPEnvController_h
#define MPEnvController_h

#include <cugl/cugl.h>
#include "MPGridModel.h"
#include "MPRoomModel.h"

class EnvController: scene2::SceneNode {
private:
    /* The grid of rooms constituting the gamespace */
    std::shared_ptr<GridModel> _grid;

    /* The row, column of the room to be swapped */
    Vec2 _toSwap;

public:
    /* Creates an envrionment controller and initializes its grid and rooms */
    EnvController();

    /*
    * Selects the room at the given location
    * Selection is for the purpose of being swapped with another room in swapWithSelected
    * Does not currently check if the room is swappable
    * 
    * @param coords     the coordinates of the selection in worldspace
    * 
    * @return true if room was successfully selected, and false otherwise
    */
    bool selectRoom(Vec2 coords);

    /*
    * Swaps the room at the given location with the selected room
    * If room at given location is the selected room, deselects the room
    * Does not currently check if the room is swappable
    * 
    * @param coords     the coordinates of the selection in worldspace
    * 
    * @return true if rooms were successfully swapped
    * @return true if room was the same as selected room & is now deselected
    * @return false if no swap occurred
    */
    bool swapWithSelected(Vec2 coords);

    /* Deselects the currently selected room, if one is selected */
    void deselectRoom();
};

#endif /* MPEnvController_h */
