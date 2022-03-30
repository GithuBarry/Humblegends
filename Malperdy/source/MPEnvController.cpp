//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Barry Wang
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/28/22
//

#include "MPEnvController.h"
#include "MPReynardModel.h"

/* Creates an envrionment controller and initializes its grid and rooms */
EnvController::EnvController() {
    _grid = std::make_shared<GridModel>();
    _toSwap = Vec2(-1, -1);
    _reyRoom = Vec2(-1, -1);
}

/* Updates the environment */
void EnvController::update(const shared_ptr<ReynardController>& reynard) {
    Vec2 newReyRoom = _grid->worldSpaceToRoom(reynard->getPosition());
    if (!_reyRoom.equals(newReyRoom)) {
        _reyRoom = newReyRoom;
        defogSurrounding(_reyRoom);
    }
}

/*
* Selects the room at the given location
* Selection is for the purpose of being swapped with another room in swapWithSelected
* Does not currently check if the room is swappable
*
* @param coords     the coordinates of the selection in screen space
* @param reynard    the controller for reynard
*
* @return true if room was successfully selected, and false otherwise
*/
bool EnvController::selectRoom(Vec2 coords, const shared_ptr<ReynardController> &reynard) {
    Vec2 room1 = _grid->screenSpaceToRoom(coords);
    bool isValidRoom = room1.x != -1 && room1.y != -1;
    isValidRoom = isValidRoom && !_grid->isRoomFogged(room1);
    bool isOccupied = containsReynard(room1, reynard);

    if (!isValidRoom || isOccupied || _grid->getRoom(room1) == nullptr) {
        deselectRoom();
        return false;
    } else {
        lookSelected(room1);
        _toSwap = room1;
        return true;
    }
}

/*
* Swaps the room at the given location with the selected room
* If room at given location is the selected room, deselects the room
* Does not currently check if the room is swappable
*
* @param coords     the coordinates of the selection in screen space
* @param reynard    the controller for reynard
*
* @return	true if rooms were successfully swapped
*			false if room was the same as selected room & is now deselected
*			false if no swap occurred
*/
bool EnvController::swapWithSelected(Vec2 coords, const shared_ptr<ReynardController> &reynard) {
    if (!hasSelected()) {
        return false;
    }

    Vec2 room2 = _grid->screenSpaceToRoom(coords);
    bool isValidRoom = room2.x != -1 && room2.y != -1;
    isValidRoom = isValidRoom && !_grid->isRoomFogged(room2);
    bool isOccupied = containsReynard(room2, reynard) || containsReynard(_toSwap, reynard);

    if (!isValidRoom) return false;
    if (isOccupied) {
        deselectRoom();
        return false;
    }
    if (room2.x == _toSwap.x && room2.y == _toSwap.y) {
        deselectRoom();
        return false;
    }

    bool success = _grid->swapRooms(_toSwap, room2);
    if (success) {
        //Sloppy code, fix when refactoring UI updates
        _toSwap = room2;
        deselectRoom();
        
        // Record the room swapping
        vector<Vec2> l;
        l.push_back(room2);
        l.push_back(_toSwap);
        _swapHistory.push_back(l);
    }
    return success;
}

/* Deselects the currently selected room, if one is selected */
void EnvController::deselectRoom() {
    if (_toSwap.x != -1 && _toSwap.y != -1) {
        lookDeselected(_toSwap);
    }
    _toSwap = Vec2(-1, -1);
}

#pragma mark Helper Functions

/*
* Checks whether Reynard is inside the indicated room
*
* @param room		the row and column of the room to check
* @param reynard    the controller for reynard
* 
* @return true if Reynard is inside the given room
*/
bool EnvController::containsReynard(Vec2 room, const shared_ptr<ReynardController> &reynard) {
    Vec2 pos = reynard->getPosition(); //reynard's center

    // TODO: fix the code below to lock a room when any part of Reynard is in it
    // character->getSize() returns the wrong size
    //float width = reynard->getSize().width;
    //float height = reynard->getSize().height;
    //for (float x = -0.5f; x <= 0.5f; x++) {
        //for (float y = -0.5f; y <= 0.5f; y++) {
            //Vec2 corner = pos + Vec2(width * x, height * y);
            //Vec2 cRoom = _grid->worldSpaceToRoom(corner);
            //CULog("corner: (%f, %f)", corner.x, corner.y);
            //CULog("in room: (%f, %f)", cRoom.x, cRoom.y);
            //if (room.equals(cRoom)) return true;
        //}
    //}

    Vec2 cRoom = _grid->worldSpaceToRoom(pos);
    if (room.equals(cRoom)) return true;
    return false;
}

/*
* Removes fog of war from the eight rooms adjacent to this one, if they exist
* Also defogs the specified room.
*
* @param room   the row and column of the central room
*/
void EnvController::defogSurrounding(Vec2 room) {
    for (float x = -1; x <= 1; x++) {
        for (float y = -1; y <= 1; y++) {
            // Doesn't have to check if room exists, since GridModel does
            Vec2 newRoom = room + Vec2(x, y);
            _grid->setRoomFog(newRoom, false);
            lookUnfogged(newRoom);
        }
    }
}

/* Sets the room to look selected */
void EnvController::lookSelected(Vec2 room) {
    if (_grid->getRoom(room)) {
        _grid->getRoom(room)->setColor(Color4::RED);
    }
}

/* Sets the room to look deselected */
void EnvController::lookDeselected(Vec2 room) {
    if (_grid->getRoom(room)){
        _grid->getRoom(room)->setColor(Color4::WHITE);
    }
}

/* Sets the room to have fog of war */
void EnvController::lookFogged(Vec2 room) {
    if (_grid->getRoom(room)) {
        _grid->getRoom(room)->setColor(Color4::BLUE);
    }
}

/* Sets the room to have no fog of war */
void EnvController::lookUnfogged(Vec2 room) {
    if (_grid->getRoom(room)) {
        _grid->getRoom(room)->setColor(Color4::WHITE);
    }
}
