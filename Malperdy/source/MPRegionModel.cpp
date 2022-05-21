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

#pragma mark Backgrounds
/**
 * Stores static references to all the background options for all regions.
 *
 * @param assets	Asset manager where all the backgrounds are stored
 * @param world		The JSON that contains all the world metadata
 */
void RegionModel::setBackgrounds(shared_ptr<cugl::AssetManager> assets, shared_ptr<JsonValue> world) {
	// Get the list of lists of backgrounds for each region type
	vector<shared_ptr<JsonValue>> bgs = world->get("backgrounds")->children();

	// Initialize vector cache for each region type's background options
	vector<string> regionBgs;

	// Store all the possible background textures, indexed by region
	for (int k = 0; k < NUM_BG_TYPES; k++) {
		regionBgs = bgs[k]->asStringArray();

		// For each background array, first is the cleared background for that region
		_bgsCleared->push_back(assets->get<Texture>(regionBgs[0]));

		// The rest are the possible options for uncleared backgrounds for that region
		_backgrounds->push_back(make_shared<vector<shared_ptr<Texture>>>());
		for (int n = 1; n < regionBgs.size(); n++) {
			_backgrounds->at(k)->push_back(assets->get<Texture>(regionBgs[n]));
		}
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
	// Transform these GRID coordinates to REGION space
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

	// Transform these GRID coordinates to REGION space
	x -= _originX;
	y -= _originY;

	return (sublevel == nullptr) ? nullptr : sublevel->getRoom(x, y);
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

	// Transform these GRID coordinates to REGION space
	x -= _originX;
	y -= _originY;

	return (sublevel == nullptr) ? false : sublevel->setRoom(x, y, room);
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

/**
 * Sets the room located in the xth column from the left and the yth
 * row from the bottom in GRID space coordinates to be an exit room.
 * This will apply a texture to hide it and make it impassable until
 * the region is cleared.
 *
 * @param x     The x-coordinate to place the room at in GRID space
 * @param y     The y-coordinate to place the room at in GRID space
 * @param tex	The texture to apply to an exit room
 * @return      Whether the room was set successfully
 */
bool RegionModel::setExitRoom(int x, int y, shared_ptr<Texture> tex) {
	shared_ptr<RoomModel> exitRoom = getRoom(x, y);
	if (exitRoom == nullptr) return false;

	_exitRooms->push_back(exitRoom);

	// Make SceneNode for the blocked texture
	shared_ptr<scene2::PolygonNode> blockedNode = scene2::PolygonNode::allocWithTexture(tex);
	blockedNode->setPolygon(Rect(0, 0, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT));
	exitRoom->addChild(blockedNode);
	_blockades->push_back(blockedNode);

	// The physics are handled in GridModel's calculatePhysicsGeometry

	return true;
}

#pragma mark Checkpoints

/**
 * Adds a checkpoint to the region and links it to whichever sublevel it is inside.
 * 
 * This should ONLY be called after all sublevels have already been added to the region.
 *
 * @param cID	The ID of the checkpoint to add to this region
 * @param cX	The x-coordinate in GRID space of this checkpoint's location
 * @param cY	The y-coordinate in GRID space of this checkpoint's location
 * @return		Whether the checkpoint was successfully added to the region
 */
bool RegionModel::addCheckpoint(int cID, int cX, int cY) {
	// Convert to REGION space
	cX -= _originX;
	cY -= _originY;

	// Increment number of uncleared checkpoints in the level
	_checkpointsToClear++;

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
 * Returns a 0 if the checkpoint failed to clear, a 1 if the checkpoint cleared successfully
 * but the region is still not cleared, or a 2 if the checkpoint cleared and it was the
 * last checkpoint in the region so the region cleared too.
 *
 * @param cID	The unique ID number for a specific checkpoint
 * @return		An integer from 0-2 representing the outcome of the attempt
 */
int RegionModel::clearCheckpoint(int cID) {
	// Fail if this checkpoint is already cleared (not in the checkpoint map, so value is -1)
	// or if checkpoint isn't in this region
	if (_checkpointMap->count(cID) == 0 || _checkpointMap->at(cID) < 0) return 0;

	// Play checkpoint clear sound effect
	AudioController::playSFX(CHECKPOINT_SOUND);

	// Clear all the rooms in the sublevel
	_sublevels->at(_checkpointMap->at(cID))->clearSublevel(_bgsCleared->at(_bgType - 1));

	// Now unlink this checkpoint from the checkpoint map
	_checkpointMap->operator[](cID) = -1;
	// And reduce number of checkpoints left to clear in this region
	_checkpointsToClear--;

	// If there are no more checkpoints in the region, clear the region
	if (_checkpointsToClear <= 0) {
		// Cleared checkpoint and region
		return 2;
	}

	// Cleared checkpoint only
	return 1;
}

/**
 * Clears the region, meaning the blockades now disappear and the
 * player can move on to the next region.
 */
void RegionModel::clearRegion() {
	// Clear physics for blockades
	for (auto itr = _blockadesObs->begin(); itr != _blockadesObs->end(); ++itr) {
		(*itr)->markRemoved(true);
	}

	// Clear visuals for blockades
	_exitRooms = nullptr;
	for (auto itr = _blockades->begin(); itr != _blockades->end(); ++itr) {
		(*itr)->setVisible(false);
	}
	_blockades = nullptr;
}