//
//  MPGridModel.h
//  Malperdy
//
//  GridModel system of rooms.
//  Storing all data of rooms grid
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPGridModel_h
#define MPGridModel_h

#include <cugl/cugl.h>
#include "MPRoomModel.h"

using namespace cugl;

class GridModel : public cugl::scene2::SceneNode
{

private:
    /** Horizontal gap between rooms in SCREEN SPACE  */
    float _horizontal_gap;

    /** Verticle gap between rooms in SCREEN SPACE  */
    float _vertical_gap;

    /**
     * Size of the level (in units of number of rooms )
     * _size[0] is the width, _size[1] is the height
     */
    Vec2 _size = Vec2(3, 3);

    float _physics_scale;

    /*
     * The 2D data type for the grid. _grid[i][j] is the ptr to the room at the ith row from the bottom, jth column from the left.
     */
    vector<vector<shared_ptr<RoomModel>>> _grid;

    // /** Rebuilds the geometry for all the rooms.
    //  *
    //  * Propogates the call down to each of the rooms.
    //  **/
    // void buildGeometry();

    // /** Rebuilds the physics assets for all the rooms.
    //  *
    //  * Propogates the call down to each of the rooms.
    //  **/
    // void buildPhysicsGeometry();

public:
#pragma mark Constructors

    /**
     * Creates an empty grid model.
     * @return an empty grid model
     */
    GridModel(){};

    /**
     * @return shared pointer to an empty grid model
     */
    static std::shared_ptr<GridModel> alloc()
    {
        std::shared_ptr<GridModel> result = std::make_shared<GridModel>();
        return (result->init() ? result : nullptr);
    }

    /**
     * Deafult init
     * @return a grid with 3x3 rooms, each room the default
     */
    bool init();

    /**
     * Init given size
     * @param width
     * @param height
     * @return a grid with width x height rooms
     */
    bool init(int width, int height);

    /**
     *  Init given size and a room template
     * @param width
     * @param height
     * @param room
     * @return a grid with width x height rooms, is of the form specified by roomID
     */
    bool init(int width, int height, string roomID);

    /**
     * {@note by Barry feature request}
     * @param width
     * @param height
     * @param jsonPath
     * @param vgap: minimal gaps between rooms
     * @param hgap: minimal hotizal gaps between rooms
     * @return
     */
    bool init(int width, int height, string jsonPath, int vgap, int hgap);

#pragma mark Destructors
    /**
     * Destroys this grid, releasing all resources.
     */
    virtual ~GridModel(void) { dispose(); }

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
    int getWidth() const
    {
        return _size.x;
    }

    /** Getter for grid height */
    int getHeight() const
    {
        return _size.y;
    }

    /** Returns a 1-D vector of all the rooms */
    vector<shared_ptr<RoomModel>> getRooms();

    /** Returns the ptr to the room located at the coordinate.
     *
     * If coord = (i,j), then this returns the room at the ith row from the bottom,
     * jth column from the left */
    shared_ptr<RoomModel> getRoom(Vec2 coord);

    /** returns all the physics geometry in the grid
     */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> getPhysicsObjects();

#pragma mark Setters

    /** Sets the room located at the ith row from the bottom, jth column from the left  */
    void setRoom(Vec2 coord, shared_ptr<RoomModel> room);

    /** Swaps two rooms given two room coordinates.
     * room = (i,j) meaning the room at row i, col j
     * returns true if the swap occurs successfully, returns false if rooms cannot be swapped */
    bool swapRooms(Vec2 room1, Vec2 room2);

    /** Returns whether the rooms can be swapped or not */
    bool canSwap(Vec2 room1, Vec2 room2);
};

#endif /* MPGridModel_h */
