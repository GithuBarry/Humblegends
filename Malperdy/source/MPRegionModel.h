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

#define NUM_BG_TYPES 3

class RegionModel {
private:
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
                    _rooms->at(y)->at(x)->clear(bgCleared);
                }
            }
        }
    };

    // PROPERTIES

    /** The name of this region */
    string _name;

    /** Which background type the region uses (corresponds to 1-3 based on region) */
    int _bgType = 0;

    /** The width and height of this region in number of rooms */
    int _width, _height;

    /** The location of the lower left corner of this region in grid space */
    int _originX, _originY;

    /** The bounds of the region in GRID space as a rect */
    Rect _bounds;

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
    shared_ptr<map<int, int>> _checkpointMap = make_shared<map<int, int>>();

    /** Number of uncleared checkpoints remaining in this region */
    int _checkpointsToClear = 0;

    /** Pointers to the SceneNodes that have the blocked exit textures */
    shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> _blockades =
            make_shared<vector<shared_ptr<scene2::PolygonNode>>>();

    /** Pointers to the physics obstacles that block the exits */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> _blockadesObs =
            make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();

    /** The exit rooms, which unblock when the region is cleared */
    shared_ptr<vector<shared_ptr<RoomModel>>> _exitRooms =
            make_shared<vector<shared_ptr<RoomModel>>>();

    // BACKGROUNDS
    /** Static reference to the background textures for all the regions */
    static shared_ptr<vector<shared_ptr<vector<shared_ptr<Texture>>>>> _backgrounds;

    /** Static reference to all the cleared background textures for each region */
    static shared_ptr<vector<shared_ptr<Texture>>> _bgsCleared;

public:
#pragma mark Constructors

    /**
     * Initializes a region with the given width, height, number, and type.
     *
     * Type is used to determine the background and should be from 1-3.
     *
     * @param name		The name of this region
     * @param type		The type of this region, from 1-3
     * @param width		The width of this region in rooms
     * @param height	The height of this region in rooms
     * @param originX   The x-coordinate of the region origin in grid space
     * @param originY   The y-coordinate of the region origin in grid space
     * @return          true if the region is initialized properly, false otherwise.
     */
    bool init(string name, int type, int width, int height, int originX, int originY) {
        _name = name;
        _bgType = type;
        _width = width;
        _height = height;
        _originX = originX;
        _originY = originY;
        _bounds = Rect(_originX, _originY, width, height);
        return true;
    }

#pragma mark Static Constructors

    /**
     * Returns a newly-allocated region with the given width, height, number, and type.
     *
     * Type is used to determine the background and should be from 1-3.
     *
     * @param name		The name of this region
     * @param type		The type of this region, from 1-3
     * @param width		The width of this region in rooms
     * @param height	The height of this region in rooms
     * @param originX   The x-coordinate of the region origin in grid space
     * @param originY   The y-coordinate of the region origin in grid space
     * @return          true if the region is initialized properly, false otherwise.
     */
    static shared_ptr<RegionModel> alloc(string name, int type, int width, int height, int originX, int originY) {
        shared_ptr<RegionModel> result = make_shared<RegionModel>();
        return (result->init(name, type, width, height, originX, originY) ? result : nullptr);
    }

#pragma mark Getters

    /**
     * Returns the name of this region, which can be used to pull its corresponding
     * JSON file from assets.
     *
     * @return	Name of this region
     */
    string getName() {
        return _name;
    }

    /**
     * Returns the "type" of this region, which is the number from 1-3 indicating
     * which background the region should use.
     *
     * @return	The type of this region
     */
    int getType() {
        return _bgType;
    }

    /**
     * Returns the number of sublevels in this region.
     *
     * @return	The number of sublevels in this region
     */
    int getNumSublevels() {
        return _sublevels->size();
    }

    /**
     * Returns the width of this region in rooms, which is
     * equivalent to the number of columns in this region.
     *
     * @return	Width of the region in rooms (columns)
     */
    int getWidth() {
        return _width;
    }

    /**
     * Returns the height of this region in rooms, which is
     * equivalent to the number of rows in this region.
     *
     * @return	Height of the region in rooms (rows)
     */
    int getHeight() {
        return _height;
    }

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
     * Returns the bounds of this region as a Rect.
     *
     * @return	The bounds of this region
     */
    Rect getBounds() {
        return _bounds;
    }

    /**
     * Returns the blockade textures, which block off the exit rooms until
     * this region is cleared.
     *
     * @return	Blockades in this region
     */
    shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> getBlockades() {
        return _blockades;
    }

    /**
     * Returns the exit rooms, which are rooms that are closed off until the
     * region is cleared.
     *
     * @return	Exit rooms in this region
     */
    shared_ptr<vector<shared_ptr<RoomModel>>> getExitRooms() {
        return _exitRooms;
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

    /**
     * Sets the exit room to be the onelocated in the xth column from the
     * left and the yth row from the bottom in GRID space coordinates.
     *
     * @param x     The x-coordinate to place the room at in GRID space
     * @param y     The y-coordinate to place the room at in GRID space
     * @param tex	The texture to apply to an exit room
     * @return      Whether the room was set successfully
     */
    bool setExitRoom(int x, int y, shared_ptr<Texture> tex);

    /**
     * Adds the given obstacle to the list of obstacles for this region's
     * exit blockades.
     *
     * @param obs	A physics obstacle for an exit room blockade, created in GridModel
     */
    void addBlockadeObs(shared_ptr<physics2::PolygonObstacle> obs) {
        _blockadesObs->push_back(obs);
    }

#pragma mark Backgrounds

    /**
     * Stores static references to all the background options for all regions.
     *
     * @param assets	Asset manager where all the backgrounds are stored
     * @param world		The JSON that contains all the world metadata
     */
    static void setBackgrounds(shared_ptr<cugl::AssetManager> assets, shared_ptr<JsonValue> world);

    /**
     * Returns a random background in the given region.
     *
     * @param regType   Type of the region the background should be for
     * @return          Shared pointer to the texture for a background in the given region
     */
    static shared_ptr<Texture> getRandBG(int regType) {
        // Get index from 0
        regType--;
        // Return random background from the ones available
        return _backgrounds->at(regType)->at(rand() % _backgrounds->at(regType)->size());
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
     * Returns a 0 if the checkpoint failed to clear, a 1 if the checkpoint cleared successfully
     * but the region is still not cleared, or a 2 if the checkpoint cleared and it was the
     * last checkpoint in the region so the region cleared too.
     *
     * @param cID	The unique ID number for a specific checkpoint
     * @return		An integer from 0-2 representing the outcome of the attempt
     */
    int clearCheckpoint(int cID);

    void clearAllCheckpoints() {
        for (auto itr = _checkpointMap->begin(); itr != _checkpointMap->end();
             ++itr) {
            clearCheckpoint((*itr).first);
        }
    }

    /**
     * Clears the region, meaning the blockades now disappear and the
     * player can move on to the next region.
     */
    void clearRegion();

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
