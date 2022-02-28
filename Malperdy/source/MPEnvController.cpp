//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Authors: Jordan Selin
//  Copyright © 2022 Humblegends. All rights reserved.
//  Version: 2/23/22
//

#include "MPEnvController.h"

/* Creates an envrionment controller and initializes its grid and rooms */
EnvController::EnvController(){
	_grid = std::make_shared<GridModel>();
	_grid->init();
	_toSwap = NULL;
}

bool EnvController::selectRoom(Vec2 coords){
	_toSwap = _grid->screenToNode(coords);
	// if coords don't correspond to a room
		// return false
	return true;
}

bool EnvController::swapWithSelected(Vec2 coords){
	if (_toSwap == NULL) {
		return false
	}
	Vec2 room2 = _grid->screenToNode(coords);
	// if coords don't correspond to a room
		// return false
	//TODO: check if the rooms are equal, not just the coordinates
	if (room2.x == _toSwap.x && room2.y == _toSwap.y) {
		_toSwap = NULL;
		return true; // may make sense to return false, depending on caller usage
	}

	return _grid->swapRooms(_toSwap, room2);
}

void EnvController::deselectRoom(){
	_toSwap = NULL;
}
