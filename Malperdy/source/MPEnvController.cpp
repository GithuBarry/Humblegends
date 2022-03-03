//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Authors: Jordan Selin
//  Copyright © 2022 Humblegends. All rights reserved.
//  Version: 2/28/22
//

#include "MPEnvController.h"

/* Creates an envrionment controller and initializes its grid and rooms */
EnvController::EnvController(){
	_grid = std::make_shared<GridModel>();
	_grid->init(1, true, 10, 10);
	_toSwap = Vec2(-1, -1);
}

bool EnvController::selectRoom(Vec2 coords){
	Vec2 room1 = _grid->worldToRoomCoords(coords);
	if (room1 == NULL) {
		_toSwap = Vec2(-1, -1);
		return false;
	}
	_toSwap = room1;
	return true;
}

bool EnvController::swapWithSelected(Vec2 coords){
	if (!hasSelected()) {
		return false;
	}
	Vec2 room2 = _grid->worldToRoomCoords(coords);
	if (room2.x == -1) {
		return false;
	}
	if (room2.x == _toSwap.x && room2.y == _toSwap.y) {
		_toSwap = NULL;
		return false;
	}

	return _grid->swapRooms(_toSwap, room2);
}

void EnvController::deselectRoom(){
	_toSwap = nullptr;
}
