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

#include "MPEnvController.h";

/* Creates an envrionment controller and initializes its grid and rooms */
EnvController::EnvController(){
	// initialize a GridModel and store in _grid;
	_toSwap = Vec2(-1, -1);
}

bool EnvController::selectRoom(Vec2 coords){
	// calculate row and column from coords
	// if coords don't correspond to a room
		// return false
	// store the row and column in _toSwap
	// return true

	return false;
}

bool EnvController::swapWithSelected(Vec2 coords){
	// if _toSwap is (-1, -1)
		// return false
	// calculate row2 and col2 from coords
	// if coords don't correspond to a room
		// return false
	// if the new row & column equal those in _toSwap
		// set _toSwap to (-1, -1)
		// may make sense to return false, depending on caller usage
		// return true
	// create a new variable tempRoom to hold the room at row2 and col2
	// get the room at _toSelect and put it into row2, col2
	// put the room in tempRoom into _toSelect.x, _toSelect.y
	// return true

	return false;
}

void EnvController::deselectRoom(){
	_toSwap = Vec2(-1, -1);
}
