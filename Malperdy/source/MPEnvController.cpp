//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Barry Wang
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/13/22
//

#include "MPEnvController.h"
#include "MPReynardModel.h"

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
* @param reynard    the controller for reynard
*
* @return true if room was successfully selected, and false otherwise
*/
bool EnvController::selectRoom(Vec2 coords, const shared_ptr<ReynardController>& reynard){
	CULog("Mouse coords: (%f, %f)", coords.x, coords.y);
	Vec2 room1 = _grid->worldSpaceToRoom(coords);
	bool isValidRoom = room1.x != -1 && room1.y != -1;
	bool isOccupied = containsReynard(room1, reynard);

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
* @param reynard    the controller for reynard
*
* @return	true if rooms were successfully swapped
*			false if room was the same as selected room & is now deselected
*			false if no swap occurred
*/
bool EnvController::swapWithSelected(Vec2 coords, const shared_ptr<ReynardController>& reynard){
	if (!hasSelected()) {
		return false;
	}

	Vec2 room2 = _grid->worldSpaceToRoom(coords);
	bool isValidRoom = room2.x != -1 && room2.y != -1;
	bool isOccupied = containsReynard(room2, reynard) || containsReynard(_toSwap, reynard);

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
* @param room		the row and column of the room to check
* @param reynard    the controller for reynard
* 
* @return true if Reynard is inside the given room
*/
bool EnvController::containsReynard(Vec2 room, const shared_ptr<ReynardController>& reynard) {
	/*CULog("room: (%f, %f)", room.x, room.y);
	Vec2 pos = reynard->getPosition(); //reynard's center
	CULog("position: (%f, %f)", pos.x, pos.y);
	/*int width = reynard->getSize().width;
	int height = reynard->getSize().height;
	for (float x = -0.5; x <= 0.5; x++) {
		for (float y = -0.5; y <= 0.5; y++) {
			Vec2 corner = pos + Vec2(width * x, height * y);
			Vec2 cRoom = _grid->worldSpaceToRoom(corner);
			CULog("corner: (%f, %f)", corner.x, corner.y);
			CULog("in room: (%f, %f)", cRoom.x, cRoom.y);
			if (room.equals(cRoom)) return true;
		}
	}*/
	/*Vec2 cRoom = _grid->worldSpaceToRoom(pos);
	CULog("in room: (%f, %f)", cRoom.x, cRoom.y);
	if (room.equals(cRoom)) return true;*/
	return false;
}
