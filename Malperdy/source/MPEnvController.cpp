//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Barry Wang
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 5/12/22
//

#include "MPEnvController.h"
#include "MPReynardModel.h"
#include "MPEnemyController.h"
#include "MPTrapModel.hpp"
#include <cugl/cugl.h>

/* Creates an envrionment controller and initializes its grid and rooms */
EnvController::EnvController() {
    _grid = std::make_shared<GridModel>();
    _toSwap = Vec2(-1, -1);
    _reyPrev = Vec2(-1, -1);
    _prevZoomOut = false;
}

/* Updates the environment */
void EnvController::update(Vec2 dragCoords, bool zoomedOut, const shared_ptr<ReynardController>& reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies) {
    Vec2 newReyRoom = _grid->worldSpaceToRoom(reynard->getScenePosition());
    if (!isSwappable(_toSwap, reynard, enemies)) {
        deselectRoom();
    }
    // Apply fog to external objects
    vector<Vec2> newEnemyPrevs = vector<Vec2>();
    for (auto i = enemies->begin(); i != enemies->end(); i++) {
        Vec2 enemyRoom = _grid->worldSpaceToRoom((*i)->getScenePosition());
        bool isFogged = _grid->isRoomFogged(enemyRoom);
        if (isFogged) (*i)->getSceneNode()->setColor(Color4(Vec4(0.2, 0.2, 0.2, 1)));
        else (*i)->getSceneNode()->setColor(Color4::WHITE);

        newEnemyPrevs.push_back(enemyRoom);
    }
    if (swapIndex <_swapHistory.size()){
        for (int i = swapIndex; i < _swapHistory.size(); ++i) {
            bool a = _grid->getRoom(_swapHistory[i][0])->update();
            bool b = _grid->getRoom(_swapHistory[i][1])->update();
            if (a && b){
                swapIndex = i+1;
            }
        }
    }
    // Update lock icons
    if (zoomedOut && !_prevZoomOut) setLockVisibility(true, reynard, enemies);
    else if (!zoomedOut && _prevZoomOut) setLockVisibility(false, reynard, enemies);
    else if (zoomedOut && _prevZoomOut) {
        // Check if prevs should still be locked
        bool isVisible = !isSwappable(_reyPrev, reynard, enemies);
        _grid->getRoom(_reyPrev)->setLockIcon(isVisible);
        for (auto i = _enemyPrevs.begin(); i != _enemyPrevs.end(); i++) {
            isVisible = !isSwappable((*i), reynard, enemies);
            _grid->getRoom(*i)->setLockIcon(isVisible);
        }
        // Lock currents
        _grid->getRoom(newReyRoom)->setLockIcon(true);
        for (auto i = newEnemyPrevs.begin(); i != newEnemyPrevs.end(); i++) {
            _grid->getRoom(*i)->setLockIcon(true);
        }
    }

    // Update previous variables & defog rooms
    if (!_reyPrev.equals(newReyRoom)) {
        _reyPrev = newReyRoom;
        defogSurrounding(_reyPrev);
    }
    _prevZoomOut = zoomedOut;
    _enemyPrevs = newEnemyPrevs;
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
bool EnvController::selectRoom(Vec2 coords, const shared_ptr<ReynardController> &reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies) {
    Vec2 room = _grid->screenSpaceToRoom(coords);

    if (isSwappable(room, reynard, enemies)) {
        lookSelected(room);
        _toSwap = room;
        return true;
    } 
    else {
        deselectRoom();
        return false;
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
bool EnvController::swapWithSelected(Vec2 coords, const shared_ptr<ReynardController> &reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies) {
    if (!hasSelected()) {
        return false;
    }

    Vec2 room2 = _grid->screenSpaceToRoom(coords);
    bool invalid = !isSwappable(room2, reynard, enemies);
    invalid = invalid || !isSwappable(_toSwap, reynard, enemies);
    invalid = invalid || (room2.x == _toSwap.x && room2.y == _toSwap.y); // can't be the sam room

    if (invalid) {
        deselectRoom();
        return false;
    }

    bool success = _grid->swapRooms(_toSwap, room2,false);
    if (success) {
        // Record the room swapping
        vector<Vec2> l;
        l.push_back(room2);
        l.push_back(_toSwap);
        _swapHistory.push_back(l);

        //Sloppy code, fix when refactoring UI updates
        _toSwap = room2;
        deselectRoom();
        

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
* Shows or hides the lock icons on locked rooms
* To be called when zooming out or in
*
* @param isVisible  true if the locks should be visible
*/
void EnvController::setLockVisibility(bool isVisible, const shared_ptr<ReynardController>& reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies) {
    for (int x = 0; x < _grid->getWidth(); x++) {
        for (int y = 0; y < _grid->getHeight(); y++) {
            _grid->getRoom(Vec2(x, y))->setLockIcon(isVisible && !isSwappable(Vec2(x, y), reynard, enemies));
        }
    }
}

/*
* Checks if the room satisfies the conditions to be swappable
*
* @param room       the row and column of the room to check
* @param reynard    the controller for reynard
* @param enemies    the controllers for the enemies
*
* @ return true if room doesn't contain Reynard, enemies or a checkpoint
*/
bool EnvController::isSwappable(Vec2 room, const shared_ptr<ReynardController>& reynard, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies) {
    // check if the room number is valid
    if (room.x == -1 || room.y == -1) return false;
    if (_grid->getRoom(room) == nullptr) return false;
    // check if the room is permalocked
    if (_grid->getRoom(room)->permlocked) return false;
    // check if the room is fogged
    if (_grid->isRoomFogged(room)) return false;
    // check if the room is occupied
    if (containsEnemies(room, enemies)) return false;
    if (containsReynard(room, reynard)) return false;
    // check if the room contains a checkpoint
    shared_ptr<TrapModel> trap = _grid->getRoom(room)->getTrap();
    if ((trap != nullptr) && (trap->getType() == TrapModel::TrapType::CHECKPOINT)) {
        return false;
    }
    return true;
}

/*
* Checks whether Reynard is inside the indicated room
*
* @param room		the row and column of the room to check
* @param reynard    the controller for reynard
* 
* @return true if Reynard is inside the given room
*/
bool EnvController::containsReynard(Vec2 room, const shared_ptr<ReynardController> &reynard) {
    Vec2 pos = reynard->getScenePosition(); //reynard's center
    Vec2 cRoom = _grid->worldSpaceToRoom(pos);
    return room.equals(cRoom);
}

/*
* Checks whether any enemies are inside the indicated room
*
* @param room       the row and column of the room to check
* @param reynard    the controllers for the enemies
*
* @return true if at least one enemy is inside the given room
*/
bool EnvController::containsEnemies(Vec2 room, const shared_ptr<vector<shared_ptr<EnemyController>>>& enemies) {
    bool isTrue = false;
    for (auto i = enemies->begin(); i != enemies->end(); i++) {
        Vec2 pos = (*i)->getScenePosition();
        Vec2 cRoom = _grid->worldSpaceToRoom(pos);
        isTrue = isTrue || room.equals(cRoom);
    }
    
    return isTrue;
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

#pragma mark Appearance Setters

/* Animates a room swap */
void EnvController::animateSwap(Vec2 vec1, Vec2 vec2) {
    //shared_ptr<Scene2::MoveTo> move1;
    //move1.alloc();
    //shared_ptr<RoomModel> room1 = _grid->getRoom(vec1);
    //shared_ptr<RoomModel> room2 = _grid->getRoom(vec2);
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

EnvController::~EnvController() {
    _grid = nullptr;
}

const vector<vector<Vec2>> &EnvController::getSwapHistory() const {
    return _swapHistory;
}
