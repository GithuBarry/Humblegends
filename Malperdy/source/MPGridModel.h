//
//  MPGridModel.h
//  Malperdy
//
//  GridModel system of rooms.
//  Storing all data of rooms grid
//
//  Owner: Evan Azari
//  Contributors: Evan Azari, Barry Wang, Jordan Selin
//  Version: 3/28/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPGridModel_h
#define MPGridModel_h

#include <cugl/cugl.h>
#include "MPRoomModel.h"
#include "MPRegionModel.h"
#include "MPCheckpoint.h"

#define DEFAULT_REGION 1

using namespace cugl;

class GridModel : public cugl::scene2::SceneNode {

private:
    /** Reference to asset manager */
    shared_ptr<cugl::AssetManager> _assets;

    // GRID LOADING
    /** Dimensions of a room in tiles */
    int _roomWidth, _roomHeight;

    /** Size of each tile in pixels */
    int _tileSize;

    /** Rooms tileset */
    shared_ptr<JsonValue> _roomsTileset;

    /**
     * Size of the entire level, spanning all regions (in units of number of rooms )
     * _size[0] is the width, _size[1] is the height
     */
    Vec2 _size = Vec2(0, 0);

    /** The origin of the grid, so location of the lower left corner */
    int _originX = 0;
    int _originY = 0;

    Rect _bounds = Rect();

    float _physics_scale;

    /*
     * The 2D data type for the grid. _grid[i][j] is the ptr to the room at the ith row from the bottom, jth column from the left.
     */
    //shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> _grid;

    /*
     * Holds all the physics objects of the grid
     */
    shared_ptr<vector<shared_ptr<vector<shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>>>>>> _physicsGeometry;

    // REGIONS

    /** The regions that form the entire level */
    shared_ptr<vector<shared_ptr<RegionModel>>> _regions = make_shared<vector<shared_ptr<RegionModel>>>();

    /** The regions that Reynard has already unlocked and therefore has access to */
    shared_ptr<vector<shared_ptr<RegionModel>>> _activeRegions = make_shared<vector<shared_ptr<RegionModel>>>();

    /** The filler rooms, so solid rooms placed in empty areas */
    shared_ptr<vector<shared_ptr<RoomModel>>> _filler = make_shared<vector<shared_ptr<RoomModel>>>();

public:
#pragma mark Constructors

    /**
     * Creates an empty grid model.
     * @return an empty grid model
     */
    GridModel() {
        //CULog("child Offset: %i", this->_childOffset);
    };

    /**
     * @return shared pointer to an empty grid model
     */
    static std::shared_ptr<GridModel> alloc(shared_ptr<AssetManager> assets) {
        std::shared_ptr<GridModel> result = std::make_shared<GridModel>();
        return (result->init(assets) ? result : nullptr);
    }

private:
    /**
     * Initializes and returns a grid of rooms with the given dimensions.
     *
     * @param width     The width/number of columns of rooms that the grid should have
     * @param height    The height/number of rows of rooms that the grid should have
     * @return          The initialized grid with the given dimensions
     */
    shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> initGrid(int width, int height);

    /**
     * Initializes a region for the game. The region is placed such that its
     * lower left corner, its region origin, is at the given coordinates in the
     * overall grid space.
     *
     * Note that everything here uses REGION space, not grid space. The only coordinates
     * in grid space are the region's origin, which will be used to organize the regions,
     * but otherwise regions only care about where things are relative to their origin.
     *
     * @param region    The JSONValue for the metadata of the region to be initialized

     */
    void GridModel::initRegion(shared_ptr<JsonValue> region);
    
public:
    /**
     * Deafult init
     * @param json - whether to use json loader or not
     * @return a grid with 3x3 rooms, each room the default
     */
    bool init(shared_ptr<AssetManager> assets, float scale = 1);

#pragma mark Destructors
    /**
     * Destroys this grid, releasing all resources.
     */
    // RoomModel[] getRooms();

    /**
     * Disposes all resources and assets of this grid.
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a room may not be used until it is initialized again.
     */
    void dispose();

#pragma mark -
#pragma mark Accessors

    /** Getter for grid width */
    int getWidth() const {
        return _size.x;
    }

    /** Getter for grid height */
    int getHeight() const {
        return _size.y;
    }

    /**
     * Returns the x-origin of the grid.
     */
    int getOriginX() const { return _originX; }

    /**
     * Returns the y-origin of the grid.
     */
    int getOriginY() const { return _originY; }

    /**
     * Returns the ptr to the room located at the given coordinate,
     * where the x-coordinate is the column from the left and the
     * y-coordinate is the row from the bottom.
     *
     * If the coordinate is out of bounds, then a null pointer is
     * returned.
     *
     * @param coord The coordinates of the desired room in (column, row) form
     * @return      The room located at the given coordinates
     */
    shared_ptr<RoomModel> getRoom(Vec2 coord) {
        return getRoom(coord.x, coord.y);
    };

    /**
     * Returns the pointer to the room located in the xth column
     * from the left and the yth row from the bottom.
     *
     * If the coordinate is out of bounds, then a null pointer is
     * returned.
     *
     * @param x The column from the left that the desired room is located in
     * @param y The row from the bottom that the desired room is located in
     * @return  The room located at the given coordinates
     */
    shared_ptr<RoomModel> getRoom(int x, int y);

    /**
     * Returns all the active regions, so the ones that Reynard has
     * already unlocked and therefore has access to.
     * 
     * @return  The list of active regions
     */
    shared_ptr<vector<shared_ptr<RegionModel>>> getActiveRegions() { return _activeRegions; }

    /**
     * Returns the physics objects of the given room in GRID coordinates.
     * 
     * This handles the issue with adding extra solid rooms to surround the
     * whole level.
     * 
     * @param row   Row of the room in GRID coordinates
     * @param col   Column of the room in GRID coordinates
     * @return      Physics objects in the given room
     */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> getPhysicsGeometryAt(int row, int col) {
        return (_physicsGeometry->at(row)->at(col));
    }

private:
    /**
     * Helper function that returns the number of the region that the given
     * GRID space coordinates are located within.
     * 
     * @param x     An x-coordinate in GRID space
     * @param y     A y-coordinate in GRID space
     * @return      The region # (1-3) that these coordinates are in, or 0 if not found
     */
    int getRegion(int x, int y);

public:
    /**
     * Returns a shared pointer to the vector of physics objects that compose
     * the geometry of all rooms in the grid.
     *
     * @return  Shared pointer to vector of physics objects for room geometry
     */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> getPhysicsObjects();

    Vec2 gridSpaceToRoom(Vec2 coord) {
        int x = (static_cast<int>(coord.x) / DEFAULT_ROOM_WIDTH) - _originX;
        int y = (static_cast<int>(coord.y) / DEFAULT_ROOM_HEIGHT) - _originY;
        // y-coordinate is in the wrong direction, so flip to be from bottom instead of top
        //y = getHeight() - y - 1;
        return Vec2(x, y);
    }

    Vec2 worldSpaceToRoom(Vec2 coord) {
        //Vec2 gridcoords = this->screenToNodeCoords(coord);
        Vec2 gridcoords = this->worldToNodeCoords(coord);
        //CULog("Grid Model: (%f, %f)", gridcoords.x, gridcoords.y);
        return gridSpaceToRoom(gridcoords);
    }

    Vec2 screenSpaceToRoom(Vec2 coord) {
        Vec2 gridcoords = this->screenToNodeCoords(coord);
        //CULog("Grid Model: (%f, %f)", gridcoords.x, gridcoords.y);
        return gridSpaceToRoom(gridcoords);
    }

    /** Returns whether the rooms can be swapped or not */
    bool canSwap(Vec2 room1, Vec2 room2);

    /*
    * Returns whether the room at the given coordinates has fog of war
    * 
    * @return whether the room at the given coordinates has fog of war
    */
    bool isRoomFogged(Vec2 coord) {
        return getRoom(coord) != nullptr && getRoom(coord)->isFogged();
    }
    
    void update(float dt){
        for(int i = 0; i < _regions->size(); i++){
            shared_ptr<RegionModel> rm = _regions->at(i);
            
            int width = rm->getWidth();
            int height = rm->getHeight();
            
            for(int row = 0; row < height; row++){
                for(int col = 0; col < width; col++){
                    shared_ptr<RoomModel> room = rm->getRoom(col, row);
                    if(room){
                        room->update(dt);
                    }
                }
            }
        }
    }

#pragma mark Setters

    /**
     * Sets the room at the given coordinate to be the given room,
     * where the x-coordinate is the column from the left and the
     * y-coordinate is the row from the bottom.
     *
     * Returns whether or not the room was set successfully.
     *
     * @param coord The coordinates of the desired room in (column, row) form
     * @param room  The room to be placed at the given coordinates
     * @return      Whether the room was set successfully
     */
    bool setRoom(Vec2 coord, shared_ptr<RoomModel> room) {
        return setRoom(coord.x, coord.y, room);
    }

    /**
     * Sets the given room to be located in the xth column from the
     * left and the yth row from the bottom.
     *
     * Returns whether or not the room was set successfully.
     *
     * @param x     The column from the left that the desired room is located in
     * @param y     The row from the bottom that the desired room is located in
     * @param room  The room to be placed at the given coordinates
     * @return      Whether the room was set successfully
     */
    bool setRoom(int x, int y, shared_ptr<RoomModel> room);

    /**
     * Swaps the two rooms at the given coordinates, where the x-coordinate is
     * the column from the left and the y-coordinate is the row from the bottom.
     *
     * Returns true if the swap was successfully performed.
     *
     * @param pos1  The coordinates of the first room to swap in (column, row) form
     * @param pos2  The coordinates of the second room to swap in (column, row) form
     * @return      Whether the swap was performed successfully
     */
    bool swapRooms(Vec2 pos1, Vec2 pos2, bool forced);

    /*
    * Sets the fog of war for the room at the given coordinates.
    * True means contents are hidden. False means they are visible.
    * 
    * Does nothing if the coordinates are out of bounds.
    */
    void setRoomFog(Vec2 coord, bool hasFog) {
        if (getRoom(coord)) getRoom(coord)->setFogged(hasFog);
    }

    /**
     * Sets the physics objects of the given room in GRID coordinates.
     *
     * @param row   Row of the room in GRID coordinates
     * @param col   Column of the room in GRID coordinates
     * @param phys  Physics objects to put in the given room
     */
    void setPhysicsGeometryAt(int row, int col, shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> phys) {
        _physicsGeometry->at(row)->at(col) = phys;
    }

#pragma mark Helpers

    Poly2 convertToScreen(Poly2 poly);

    void calculatePhysicsGeometry();

    shared_ptr<physics2::PolygonObstacle> makeStaticFromPath(Path2 path);

#pragma mark Checkpoints
    /**
     * Clears all the rooms associated with the checkpoint with the given ID (backgrounds
     * are swapped to the "cleared" option for the associated region).
     *
     * @param cID	The unique ID number for a specific checkpoint
     * @return		Whether the checkpoint's associated rooms were cleared successfully
     */
    bool clearCheckpoint(int cID);
};


#endif /* MPGridModel_h */
