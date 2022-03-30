//
//  MPEnvController.h
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Barry Wang
//  Version: 3/13/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPEnvController_h
#define MPEnvController_h

#include <cugl/cugl.h>
#include "MPGridModel.h"
#include "MPReynardController.h"

using namespace cugl;

class EnvController {
private:
    /* The grid of rooms constituting the gamespace */
    std::shared_ptr<GridModel> _grid;

    /* The row, column of the room to be swapped */
    Vec2 _toSwap;

    /* Reynard's current room */
    Vec2 _reyRoom;

public:
    /* Creates an envrionment controller and initializes its grid and rooms */
    EnvController();

    /*
    * Sets the GridModel stored in this controller
    *
    * @param grid   the GridModel to store in this controller
    */
    void setGrid(const std::shared_ptr<GridModel> grid) {
        _grid = grid;
    }
    
    shared_ptr<GridModel> getGrid(){ return _grid; }

    /* Updates the environment */
    void update(const shared_ptr<ReynardController>& reynard);

    /*
    * Selects the room at the given location
    * Selection is for the purpose of being swapped with another room in swapWithSelected
    * Will not select a room if it contains Reynard
    * 
    * @param coords     the coordinates of the selection in worldspace
    * @param reynard    the controller for reynard
    * 
    * @return true if room was successfully selected, and false otherwise
    */
    bool selectRoom(Vec2 coords, const shared_ptr<ReynardController> &reynard);

    /* 
    * Returns whether there is currently a room selected 
    * 
    * @return true if there is currently a room selected
    */
    bool hasSelected() {
        return (_toSwap.x != -1 || _toSwap.y != -1);
    }

    /*
    * Swaps the room at the given location with the selected room
    * If room at given location is the selected room, deselects the room
    * Will not swap a room if it contains Reynard
    * 
    * Will deselect the currently selected room on a successful swap,
    * or on an attempt to swap a room with itself
    * 
    * @param coords     the coordinates of the selection in worldspace
    * @param reynard    the controller for reynard
    * 
    * @return   true if rooms were successfully swapped
    *           false if room was the same as selected room
    *           false if no swap occurred
    */
    bool swapWithSelected(Vec2 coords, const shared_ptr<ReynardController> &reynard);

    /* Deselects the currently selected room, if one is selected */
    void deselectRoom();

private:
#pragma mark Helper Functions

    /*
    * Checks whether Reynard is inside the indicated room
    * 
    * @param room       the row and column of the room to check
    * @param reynard    the controller for reynard
    * 
    * @return true if Reynard is inside the given room
    */
    bool containsReynard(Vec2 room, const shared_ptr<ReynardController> &reynard);

    /*
    * Removes fog of war from the eight rooms adjacent to this one, if they exist
    * Also defogs the specified room.
    * 
    * @param room   the row and column of the central room
    */
    void defogSurrounding(Vec2 room);

#pragma mark Appearance Setters

    /* Sets the room to look selected */
    void lookSelected(Vec2 room);

    /* Sets the room to look deselected */
    void lookDeselected(Vec2 room);

    /* Sets the room to have fog of war */
    void lookFogged(Vec2 room);

    /* Sets the room to have no fog of war */
    void lookUnfogged(Vec2 room);
};

#endif /* MPEnvController_h */
