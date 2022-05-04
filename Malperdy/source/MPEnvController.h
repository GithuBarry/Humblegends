//
//  MPEnvController.h
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Barry Wang
//  Version: 4/23/22
//
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPEnvController_h
#define MPEnvController_h

#include <cugl/cugl.h>
#include "MPGridModel.h"
#include "MPReynardController.h"
#include "MPEnemyController.h"

using namespace cugl;

class EnvController {
private:
    /* The grid of rooms constituting the gamespace */
    std::shared_ptr<GridModel> _grid;

    /* The row, column of the room to be swapped */
    Vec2 _toSwap;

    /* Reynard's current room */
    Vec2 _reyRoom;

    /* History of room swaps as a list of pairs of rooms that have been swapped */
    vector<vector<Vec2>> _swapHistory;

    /**
     * how many pairs of rooms finished swapping
     */
     int swapIndex = 0;
public:
    const vector<vector<Vec2>> &getSwapHistory() const;

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

    /*  
    * Updates the environment
    * 
    * Defogs rooms, applies fog to moving (non-Reynard) objects entering fogged rooms
    * Updates the animations for an in-progress drag
    * 
    * @param dragCoords     the coordinates of the in-progress drag, or (-1, -1) if there is none
    * @param reynard        Reynard's controller
    * @param enemies        the enemies' controllers
    */
    void update(Vec2 dragCoords, const shared_ptr<ReynardController>& reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies);

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
    bool selectRoom(Vec2 coords, const shared_ptr<ReynardController> &reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies);

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
    bool swapWithSelected(Vec2 coords, const shared_ptr<ReynardController> &reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies);

    /* Deselects the currently selected room, if one is selected */
    void deselectRoom();

    /*
    * Return swap history up to last call of this function (since last checkpoint) 
    * and start a new session (session as between checkpoints)
    * 
    * @return   the swap history between last checkpoint and this checkpoint
    *           History is returned as a list of room pairs that have been swapped
    *           (i.e. [[room1, room2], [room2, room3] ... ])
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
            _grid->swapRooms(room1, room2,true);
        }
        _swapHistory = vector<vector<Vec2>>();
    }
    
    void swapRoomOnGrid (Vec2 room1,Vec2  room2, bool forced){
        _grid->swapRooms(room1, room2, forced);
    }

    virtual ~EnvController();


private:
#pragma mark Helper Functions

    /*
    * Checks if the room satisfies the conditions to be swappable
    * 
    * @param room       the row and column of the room to check
    * @param reynard    the controller for reynard
    * @param enemies    the controllers for the enemies
    * 
    * @ return true if room doesn't contain Reynard, enemies or a checkpoint
    */
    bool isSwappable(Vec2 room, const shared_ptr<ReynardController>& reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies);

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
    * Checks whether any enemies are inside the indicated room
    *
    * @param room       the row and column of the room to check
    * @param enemies    the controllers for the enemies
    *
    * @return true if at least one enemy is inside the given room
    */
    bool containsEnemies(Vec2 room, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies);

    /*
    * Removes fog of war from the eight rooms adjacent to this one, if they exist
    * Also defogs the specified room.
    *
    * @param room   the row and column of the central room
    */
    void defogSurrounding(Vec2 room);

#pragma mark Appearance Setters

    /* Animates a room swap */
    void animateSwap(Vec2 room1, Vec2 room2);
    
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
