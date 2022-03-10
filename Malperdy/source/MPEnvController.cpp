//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Barry Wang
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22
//

#include "MPEnvController.h"

/* Creates an envrionment controller and initializes its grid and rooms */
EnvController::EnvController(){
	_grid = std::make_shared<GridModel>();
	_grid->init(1, true, 10, 10);
	_toSwap = Vec2(-1, -1);
}

/*
* Selects the room at the given location
* Selection is for the purpose of being swapped with another room in swapWithSelected
* Does not currently check if the room is swappable
*
* @param coords     the coordinates of the selection in worldspace
*
* @return true if room was successfully selected, and false otherwise
*/
bool EnvController::selectRoom(Vec2 coords){
	Vec2 room1 = _grid->worldSpaceToRoom(coords);
	bool isValidRoom = room1.x != -1 && room1.y != -1;
	bool isOccupied = containsReynard(room1);

	if (!isValidRoom || isOccupied) {
		deselectRoom();
		return false;
	}
	else{
		_grid->getRoom(room1)->setColor(Color4::RED);
		_toSwap = room1;
		return true;
	}
}

/*
* Swaps the room at the given location with the selected room
* If room at given location is the selected room, deselects the room
* Does not currently check if the room is swappable
*
* @param coords     the coordinates of the selection in worldspace
*
* @return true if rooms were successfully swapped
* @return false if room was the same as selected room & is now deselected
* @return false if no swap occurred
*/
bool EnvController::swapWithSelected(Vec2 coords){
	if (!hasSelected()) {
		return false;
	}

	Vec2 room2 = _grid->worldSpaceToRoom(coords);
	bool isValidRoom = room2.x != -1 && room2.y != -1;
	bool isOccupied = containsReynard(room2) || containsReynard(_toSwap);

	if (!isValidRoom || isOccupied) {
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
	}
	return success;
}

/* Deselects the currently selected room, if one is selected */
void EnvController::deselectRoom(){
	if (_toSwap.x != -1 && _toSwap.y != -1) {
		_grid->getRoom(_toSwap)->setColor(Color4::WHITE);
	}
	_toSwap = Vec2(-1, -1);
}

#pragma mark Helper Functions
/*
* Checks whether Reynard is inside the indicated room
*
* @param pos    the position Reynard is currently at
* @param size   Reynard's size
* @param room   the row and column of the room to check
* 
* @return true if Reynard is inside the given room
*/
bool EnvController::containsReynard(Vec2 room) {
	//TODO: write actual implementation
	//calculate Reynard's corners by his position & size
	//for each corner
		//call GridModel to see the room that corner is in
		//if that room is the room we're checking
			//return true
	return false;
}
