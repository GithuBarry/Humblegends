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

#ifndef MPRegionModel_h
#define MPRegionModel_h

#include <cugl/cugl.h>
#include <map>

#include "MPRoomModel.h"
#include "MPAudioController.h"

using namespace cugl;

/** Total number of regions */
#define NUM_REGIONS 1
/** Total number of backgrounds each region has */
#define NUM_BACKGROUNDS 7

/** The names of the cleared backgrounds, indexed by region */
#define BG_CLEARED {"bg-r1c-1", "bg-r1c-1", "bg-r1c-1"}
/** All backgrounds for each region */
#define R1_BGS {"bg-r1-1", "bg-r1-2", "bg-r1-3", "", "", "", ""}
#define R2_BGS { "bg-r2-1", "bg-r2-2", "bg-r2-3", "bg-r2-4", "", "", "" }
#define R3_BGS { "bg-r3-1", "bg-r3-2", "bg-r3-3", "bg-r3-4", "bg-r3-5", "bg-r3-6", "bg-r3-7" }

class RegionModel {
private:
	// Names of all the background assets, indexed by region
	// See top of MPRegionModel.cpp for declaration
	static const string BG_REGIONS[NUM_REGIONS][NUM_BACKGROUNDS];

	/**
	 * Nested class representing a sublevel inside a region.
	 */
	class Sublevel {
	private:
		/** Coordinates, in region space, of the lower left corner room of this sublevel */
		int _originX, _originY;

		/** Width and height, in number of rooms, of this sublevel */
		int _width, _height;

		/** Grid of pointers for all the rooms in the sublevel, where _rooms[i][j]
		is the room in the ith row from the bottom and jth column from the left in
		REGION space */
		shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> _rooms;

	public:
		/**
		 * Initialize a sublevel with the given characteristics.
		 * 
		 * @param originX		The x-coordinate of the lower left room of the sublevel, in region space
		 * @param originY		The y-coordinate of the lower left room of the sublevel, in region space
		 * @param width			How many rooms wide this sublevel is
		 * @param height		How many rooms tall this sublevel is
		 * @param rooms			The rooms of the sublevel as a 2D vector array
		 * @return				true if the sublevel is initialized properly, false otherwise.
		 */
		bool init(int originX, int originY, int width, int height,
			shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> rooms) {
			_originX = originX;
			_originY = originY;
			_width = width;
			_height = height;
			_rooms = rooms;
			return true;
		}

		/**
		 * Returns a newly-allocated sublevel with the given characteristics.
		 *
		 * @param originX		The x-coordinate of the lower left room of the sublevel, in region space
		 * @param originY		The y-coordinate of the lower left room of the sublevel, in region space
		 * @param width			How many rooms wide this sublevel is
		 * @param height		How many rooms tall this sublevel is
		 * @param rooms			The rooms of the sublevel as a 2D vector array
		 * @return				A newly-allocated Sublevel
		 */
		static shared_ptr<Sublevel> alloc(int originX, int originY, int width, int height,
			shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> rooms) {
			shared_ptr<Sublevel> result = make_shared<Sublevel>();
			return (result->init(originX, originY, width, height, rooms) ? result : nullptr);
		}

		/**
		 * Returns whether the given coordinates in REGION space are within this sublevel.
		 * 
		 * @param x		An x-coordinate in region space (cols from left)
		 * @param y		A y-coordinate in region space (rows from bottom)
		 * @return		Whether the given coordinate is within this sublevel
		 */
		bool isInSublevel(int x, int y) {
			return (_originX <= x && x < _originX + _width
				&& _originY <= y && y < _originY + _height);
		}

		/**
		 * Returns the room at the given coordinates in REGION space.
		 * 
		 * @param x		An x-coordinate for a room in region space (cols from left)
		 * @param y		A y-coordinate for a room in region space (rows from bottom)
		 * @return		Whether there is a room at the given coordinates, or nullptr otherwise
		 */
		shared_ptr<RoomModel> getRoom(int x, int y) {
			// Return nullptr now if this room is not in this sublevel
			if (!isInSublevel(x, y)) return nullptr;

			// Then return the right room
			return _rooms->at(y)->at(x);
		}

		/**
		 * Sets the given room to be located in the xth column from the
		 * left and the yth row from the bottom in REGION space coordinates.
		 * 
		 * Returns whether or not the room was set successfully.
		 *
		 * @param x     The x-coordinate to place the room at in REGION space
		 * @param y     The y-coordinate to place the room at in REGION space
		 * @param room  The room to be placed at the given coordinates
		 * @return      Whether the room was set successfully
		 */
		bool setRoom(int x, int y, shared_ptr<RoomModel> room) {
			// Return false now if this location is not in this sublevel
			if (!isInSublevel(x, y)) return false;

			// Set the room and then return true
			_rooms->at(y)->at(x) = room;
			return true;
		}

		/**
		 * Clears all the rooms in this sublevel by changing the backgrounds to the
		 * given cleared background.
		 * 
		 * @param bgCleared	The background texture that marks a room as cleared
		 */
		void clearSublevel(shared_ptr<Texture> bgCleared) {
			// For each room in the sublevel
			for (int x = _originX; x < _originX + _width; x++) {
				for (int y = _originY; y < _originY + _height; y++) {
					_rooms->at(y)->at(x)->_bgNode->setTexture(bgCleared);
				}
			}
		}
	};

	// PROPERTIES

	/** The number of this region, from 1-3 */
	int _regionNumber;

	/** The width and height of this region in number of rooms */
	int _width, _height;

	/** The location of the lower left corner of this region in grid space */
	int _originX, _originY;

	// CHECKPOINTS + SUBLEVELS

	/** Vector of all sublevels in this region */
	shared_ptr<vector<shared_ptr<Sublevel>>> _sublevels = make_shared<vector<shared_ptr<Sublevel>>>();

	/**
	 * Map of each checkpoint to its associated sublevel. Each checkpoint has a unique ID
	 * number which is used as the key, then the values is the index at which the
	 * corresponding sublevel is stored in [_sublevels].
	 * 
	 * A checkpoint is delinked from its sublevel (it's been cleared) when its value is -1
	 * in this map.
	 * 
	 * Map is in the (key, value) format (checkpoint ID, index in _sublevels).
	 */
	shared_ptr<map<int, int>> _checkpointMap;

	/** Number of uncleared checkpoints remaining in this region (initialize at 1 so that it
	doesn't register as no checkpoints remaining before checkpoints have been added) */
	int _checkpointsToClear = 1;

	// BACKGROUNDS
	/** Static reference to the background textures for all the regions */
	static shared_ptr<vector<shared_ptr<vector<shared_ptr<Texture>>>>> _backgrounds;

	/** Static reference to all the cleared background textures for each region */
	static shared_ptr<vector<shared_ptr<Texture>>> _bgsCleared;

public:
#pragma mark Constructors
	/**
	 * Initializes a region with the given width, height, and number.
	 *
	 * @param regNumber	The number of this region, from 1-3
	 * @param originX       The x-coordinate of the region origin in grid space
	 * @param originY       The y-coordinate of the region origin in grid space
	 * @return          true if the region is initialized properly, false otherwise.
	 */
	bool init(int regNumber, int originX, int originY) {
		_regionNumber = regNumber;
		_originX = originX;
		_originY = originY;
		return true;
	}

#pragma mark Static Constructors
	/**
	 * Returns a newly-allocated region with the given width, height, and number.
	 *
	 * @param regNumber	The number of this region, from 1-3
	 * @param originX       The x-coordinate of the region origin in grid space
	 * @param originY       The y-coordinate of the region origin in grid space
	 * @return          A newly-allocated RegionModel
	 */
	static shared_ptr<RegionModel> alloc(int regNumber, int originX, int originY) {
		shared_ptr<RegionModel> result = make_shared<RegionModel>();
		return (result->init(regNumber, originX, originY) ? result : nullptr);
	}

#pragma mark Getters
	/**
	 * Returns the number of sublevels in this region.
	 * 
	 * @return	The number of sublevels in this region
	 */
	int getNumSublevels() { return _sublevels->size(); }

	/**
	 * Returns the width of this region in rooms, which is
	 * equivalent to the number of columns in this region.
	 * 
	 * @return	Width of the region in rooms (columns)
	 */
	int getWidth() { return _width; }

	/**
	 * Returns the height of this region in rooms, which is
	 * equivalent to the number of rows in this region.
	 *
	 * @return	Height of the region in rooms (rows)
	 */
	int getHeight() { return _height; }

	/**
	 * Returns the x-coordinate of the origin of this region in grid
	 * space, so the column in which the lower left room of this region
	 * is located in the overall grid.
	 *
	 * @return	The x-coordinate of the region origin in grid space
	 */
	int getGridOriginX() {
		return _originX;
	}

	/**
	 * Returns the y-coordinate of the origin of this region in grid
	 * space, so the row in which the lower left room of this region
	 * is located in the overall grid.
	 *
	 * @return	The y-coordinate of the region origin in grid space
	 */
	int getGridOriginY() {
		return _originY;
	}

	/**
	 * Returns whether the given GRID space coordinates are within this
	 * region.
	 *
	 * @param x		An x-coordinate in GRID space
	 * @param y		A y-coordinate in GRID space
	 * @return		Whether the given coordinate is in this region
	 */
	bool isInRegion(int x, int y);

	/**
	 * Returns the room at the given GRID space coordinates.
	 * 
	 * @param x		An x-coordinate in GRID space
	 * @param y		A y-coordinate in GRID space
	 * @return		The RoomModel at the given coordinates, or nullptr if there is none
	 */
	shared_ptr<RoomModel> getRoom(int x, int y);

	/**
	 * Returns whether the given pairs of coordinates are both within the same sublevel.
	 * 
	 * @param x1	First x-coordinate in GRID space
	 * @param y1	First y-coordinate in GRID space
	 * @param x2	Second x-coordinate in GRID space
	 * @param y2	Second y-coordinate in GRID space
	 * @return		Whether both coordinates are within the same sublevel
	 */
	bool areInSameSublevel(int x1, int y1, int x2, int y2) {
		shared_ptr<Sublevel> sublevel1 = getSublevel(x1, y1);
		shared_ptr<Sublevel> sublevel2 = getSublevel(x2, y2);
		return (sublevel1 == sublevel2 && sublevel1 != nullptr && sublevel2 != nullptr);
	}

private:
	/**
	 * Returns the sublevel that the room at the given GRID space coordinates is located in.
	 *
	 * @param x		An x-coordinate of a room in GRID space
	 * @param y		A y-coordinate of a room in GRID space
	 * @return		The Sublevel at the given coordinates, or nullptr if there is none
	 */
	shared_ptr<Sublevel> getSublevel(int x, int y);

public:
#pragma mark Setters
	/**
	 * Sets the dimensions of this region.
	 *
	 * @param width		The width of this region in rooms
	 * @param height	The height of this region in rooms
	 */
	void setDims(int width, int height) {
		_width = width;
		_height = height;
	}

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
	bool setRoom(int x, int y, shared_ptr<RoomModel> room);

#pragma mark Backgrounds
	/**
	 * Stores static references to all the background options for all regions.
	 *
	 * @param assets	Asset manager where all the backgrounds are stored
	 */
	static void setBackgrounds(shared_ptr<cugl::AssetManager> assets);

	/**
	 * Returns a random background in the given region.
	 *
	 * @param regNum    Number of the region the background should be for
	 * @return          Shared pointer to the texture for a background in the given region
	 */
	static shared_ptr<Texture> getRandBG(int regNum) {
		// Get index from 0
		regNum--;
		// Return random background from the ones available
		return _backgrounds->at(regNum)->at(rand() % _backgrounds->at(regNum)->size());
	}

#pragma mark Sublevels
	/**
	 * Adds a sublevel with the given characteristics to this region.
	 * 
	 * This DOES NOT handle checkpoint linking; that msut be done separately.
	 * 
	 * @param originX		The x-coordinate of the lower left room of the sublevel, in region space
	 * @param originY		The y-coordinate of the lower left room of the sublevel, in region space
	 * @param width			How many rooms wide this sublevel is
	 * @param height		How many rooms tall this sublevel is
	 * @param rooms			The rooms of the sublevel as a 2D vector array
	 */
	void addSublevel(int originX, int originY, int width, int height,
		shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> rooms);

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
	bool addCheckpoint(int cID, int cX, int cY);

	/**
	 * Clears all the rooms associated with the checkpoint with the given ID (backgrounds
	 * are swapped to the "cleared" option for the associated region).
	 *
	 * @param cID	The unique ID number for a specific checkpoint
	 * @return		Whether the checkpoint's associated rooms were cleared successfully
	 */
	bool clearCheckpoint(int cID);

private:
#pragma mark Debug
	/**
	 * Prints out the ID of each checkpoint and how many rooms are linked to it.
	 */
	void printCheckpointMap() {
		CULog("[RegionModel.h] CHECKPOINT MAP");
		for (map<int, int>::iterator itr = _checkpointMap->begin(); itr != _checkpointMap->end(); ++itr) {
			CULog("[RegionModel.h] Checkpoint ID #%d: %d rooms", itr->first, itr->second);
		}
	}
};


#endif /* MPRegionModel_h */
