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

    vector<vector<Vec2>> _swapHistory;

public:
    /* Creates an envrionment controller and initializes its grid and rooms */
    EnvController();

    /*
    * Sets the GridModel stored in this controller
    *
    * @param grid   the GridModel to store in this controller
    */
    void setGrid(std::shared_ptr<GridModel> grid) {
        _grid = grid;
    }

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

    /**
     Return swap history up to last call of this function
     in format of [[room11, room12],[room21,room22],...],
     and start a new session (session as between checkpoints)
     @return the swap history between last checkpoint and this checkpoint
     */
    vector<vector<Vec2>> checkPoint(){
        vector<vector<Vec2>> result =_swapHistory;
        _swapHistory = vector<vector<Vec2>>();
        return result;
    }

    /** Restore the room order according to the current history (since last checkpoint) */
    void revertHistory(){
        for(int i = _swapHistory.size() - 1; i >= 0; i--){
            vector<Vec2> roomPair = _swapHistory[i];
            Vec2 room1 =roomPair[0];
            Vec2 room2 =roomPair[1];
            _grid->swapRooms(room1, room2);
        }
        _swapHistory = vector<vector<Vec2>>();
    }


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
};

#endif /* MPEnvController_h */
