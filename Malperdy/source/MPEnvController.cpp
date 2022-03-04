//
//  MPEnvController.cpp
//  Malperdy
//
//  Controller for the house of Malperdy
//
//  Authors: Jordan Selin
//  Copyright � 2022 Humblegends. All rights reserved.
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
	Vec2 room1 = _grid->worldSpaceToRoom(coords);
	if (room1.x==-1 || room1.y == -1) {
		_toSwap = Vec2(-1, -1);
		return false;
	}else{
		_grid->getRoom(Vec2(room1.y,room1.x))->setColor(Color4::RED);
	}
	_toSwap = room1;
	CULog("Room selected: (%f, %f)", _toSwap.x, _toSwap.y);

	return true;
}

bool EnvController::swapWithSelected(Vec2 coords){
	if (!hasSelected()) {
		return false;
	}
	Vec2 room2 = _grid->worldSpaceToRoom(coords);
	if (room2.x == -1 || room2.y == -1) {
		return false;
	}
	_grid->getRoom(Vec2(_toSwap.y,_toSwap.x))->setColor(Color4::WHITE);
	if (room2.x == _toSwap.x && room2.y == _toSwap.y) {
		CULog("Tried to swap with itself");
		_toSwap = Vec2(-1, -1);
		return false;
	}

	bool success = _grid->swapRooms(Vec2(_toSwap.y,_toSwap.x), Vec2(room2.y, room2.x));
	if (success) {
		CULog("Swapping with: (%f, %f)", room2.x, room2.y);
		_toSwap = Vec2(-1, -1);
	}
	return success;
}

void EnvController::deselectRoom(){
	_toSwap = nullptr;
}
