//
//  MPRegionModel.h
//  Malperdy
//
//  This class is a pure data class that handles regions and their sublevels.
//  In practice, the environment controller only ever interacts with GridModel,
//  but this is how GridModel is tracking things like regions and sublevels in
//  the backend.
// 
//  GridModel is the only class that should be interacting with this class.
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 5/13/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPRegionModel.h"

using namespace cugl;

shared_ptr<vector<shared_ptr<Texture>>> RegionModel::_bgsCleared = make_shared<vector<shared_ptr<Texture>>>();
shared_ptr<vector<shared_ptr<vector<shared_ptr<Texture>>>>> RegionModel::_backgrounds = make_shared<vector<shared_ptr<vector<shared_ptr<Texture>>>>>();
//const string RegionModel::BG_REGIONS[NUM_REGIONS][NUM_BACKGROUNDS] = {R1_BGS, R2_BGS, R3_BGS};
// TODO: replace with a full size-3 array when the regions are all in
const string RegionModel::BG_REGIONS[NUM_REGIONS][NUM_BACKGROUNDS] = { R1_BGS };

#pragma mark Backgrounds
/**
 * Stores static references to all the background options for all regions.
 *
 * @param assets	Asset manager where all the backgrounds are stored
 */
void RegionModel::setBackgrounds(shared_ptr<cugl::AssetManager> assets) {
	string bgClearName;

	// Store all the possible background textures, indexed by region
	_backgrounds = make_shared<vector<shared_ptr<vector<shared_ptr<Texture>>>>>();
	for (int k = 0; k < NUM_REGIONS; k++) {
		_backgrounds->push_back(make_shared<vector<shared_ptr<Texture>>>());
		for (string name : BG_REGIONS[k]) {
			if (name != "") _backgrounds->at(k)->push_back(assets->get<Texture>(name));
		}
	}
	// Store all the cleared backgrounds for each region
	for (string name : BG_CLEARED) {
		_bgsCleared->push_back(assets->get<Texture>(name));
	}
}

#pragma mark Getters
/**
 * Returns whether the given GRID space coordinates are within this
 * region.
 *
 * @param x		An x-coordinate in GRID space
 * @param y		A y-coordinate in GRID space
 * @return		Whether the given coordinate is in this region
 */
bool RegionModel::isInRegion(int x, int y) {
	return (_originX <= x && x < _originX + _width
		&& _originY <= y && y < _originY + _height);
}

/**
 * Returns the sublevel that the room at the given GRID space coordinates is located in.
 *
 * @param x		An x-coordinate of a room in GRID space
 * @param y		A y-coordinate of a room in GRID space
 * @return		The Sublevel containing the given coordinates, or nullptr if there is none
 */
shared_ptr<RegionModel::Sublevel> RegionModel::getSublevel(int x, int y) {
	// Convert coordinates from grid to region space
	x -= _originX;
	y -= _originY;

	// Check each sublevel for if it contains this room
	for (shared_ptr<Sublevel> sublevel : *_sublevels) {
		if (sublevel->isInSublevel(x, y)) return sublevel;
	}

	return nullptr;
}

/**
 * Returns the room at the given GRID space coordinates.
 *
 * @param x		An x-coordinate in GRID space
 * @param y		A y-coordinate in GRID space
 * @return		The RoomModel at the given coordinates, or nullptr if there is none
 */
shared_ptr<RoomModel> RegionModel::getRoom(int x, int y) {
	shared_ptr<Sublevel> sublevel = getSublevel(x, y);
	return (sublevel == nullptr) ? nullptr : sublevel->getRoom(x - _originX, y - _originY);
}

#pragma mark Setters
/**
 * Sets the given room to be located in the xth column from the
 * left and the yth row from the bottom in GRID space coordinates.
 *
 * Returns whether or not the room was set successfully.
 *
 * @param x     The x-coordinate to place the room at in GRID space
 * @param y     The y-coordinate to place the room at in GRID space
 * @param room  The room to be placed at the given coordinates
 * @return      Whether the room was set successfully
 */
bool RegionModel::setRoom(int x, int y, shared_ptr<RoomModel> room) {
	shared_ptr<Sublevel> sublevel = getSublevel(x, y);
	return (sublevel == nullptr) ? false : sublevel->setRoom(x - _originX, y - _originY, room);
}

#pragma mark Sublevels
/**
 * Adds a sublevel with the given characteristics to this region.
 * 
 * This DOES NOT handle checkpoint linking; that must be done separately.
 *
 * @param originX		The x-coordinate of the lower left room of the sublevel, in region space
 * @param originY		The y-coordinate of the lower left room of the sublevel, in region space
 * @param width			How many rooms wide this sublevel is
 * @param height		How many rooms tall this sublevel is
 * @param rooms			The rooms of the sublevel as a 2D vector array
 */
void RegionModel::addSublevel(int originX, int originY, int width, int height,
	shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> rooms) {

	// Create a new sublevel with the given characteristics and store
	shared_ptr<Sublevel> sublevel = Sublevel::alloc(originX, originY, width, height, rooms);
	_sublevels->push_back(sublevel);
}

#pragma mark Checkpoints

/**
 * Adds a checkpoint to the region and links it to whichever sublevel it is inside.
 * 
 * This should ONLY be called after all sublevels have already been added to the region.
 *
 * @param cID	The ID of the checkpoint to add to this region
 * @param cX	The x-coordinate in region space of this checkpoint's location
 * @param cY	The y-coordinate in region space of this checkpoint's location
 * @return		Whether the checkpoint was successfully added to the region
 */
bool RegionModel::addCheckpoint(int cID, int cX, int cY) {
	// If this is the first time that this was called, then all sublevels have been added
	// Thus, initialize the level map to be the same length as there are sublevels
	if (!_checkpointMap) _checkpointMap = make_shared<map<int, int>>();
	// If not, then increment number of uncleared checkpoints in the level
	else _checkpointsToClear++;

	// Now check each sublevel to find the one this checkpoint belongs to
	for (int k = 0; k < _sublevels->size(); k++) {
		// When that sublevel is found
		if (_sublevels->at(k)->isInSublevel(cX, cY)) {
			// Add the checkpoint to the map
			_checkpointMap->emplace(cID, k);
			return true;
		}
	}

	// If no such sublevel was found, return false
	return false;
}

/**
 * Clears all the rooms associated with the checkpoint with the given ID (backgrounds
 * are swapped to the "cleared" option for the associated region).
 *
 * @param cID	The unique ID number for a specific checkpoint
 * @return		Whether the checkpoint's associated rooms were cleared successfully
 */
bool RegionModel::clearCheckpoint(int cID) {
	// Fail if this checkpoint is already cleared (not in the checkpoint map, so value is -1)
	// or if checkpoint isn't in this region
	if (_checkpointMap->count(cID) == 0 || _checkpointMap->at(cID) < 0) return false;

	// Clear all the rooms in the sublevel
	_sublevels->at(_checkpointMap->at(cID))->clearSublevel(_bgsCleared->at(_regionNumber));

	// Now unlink this checkpoint from the checkpoint map
	_checkpointMap->operator[](cID) = -1;
	// And reduce number of checkpoints left to clear in this region
	_checkpointsToClear--;

	return true;
}