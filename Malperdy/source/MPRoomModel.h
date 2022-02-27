//
//  MPRoomModel.h
//  Malperdy
//
//  This class encapsulates all of the information for a single room, including
//  its geometry for both drawing and physics. Locations of everything within a
//  a room are stored relative to the room's origin, which is the lower left corner.
// 
//  ALWAYS call loadRooms() first before creating or initializing any rooms. This
//  will read in all the predefined room geometries from the JSON and store those
//  as a static lookup table that all instances of RoomModel can access. Failing
//  to call this first means that rooms won't know what geometries to draw for
//  themselves.
// 
//  Geometry within a room is defined as a percentage of the room's actual width/
//  height. Changing the macros for default room width/height in the headerfile will
//  scale the rooms accordingly. Rooms are not locked, meaning they can be swapped,
//  by default upon creation.
// 
//  Room is a subclass of SceneNode, and so all of SceneNode's methods can be used
//  with it. This allows individual rooms and their contents to be scaled properly
//  when zooming in and out. Each element of room geometry is then stored as a child
//  scene graph node of the room.
// 
//  In theory, all the rooms in a level can be made children of a SceneNode for the
//  whole level grid. Transforming the grid should then allow for easy transformation
//  of all the room, and calling render() on the level grid SceneNode should draw all
//  of the rooms accordingly.
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPRoomModel_h
#define MPRoomModel_h

#include <cugl/cugl.h>
#include <stdlib.h>
#include <vector>
#include <map>

#include "MPRoomLoader.h"

using namespace cugl;

/** The default width of a room in pixels */
#define DEFAULT_ROOM_WIDTH 720
/** The default height of a room in pixels */
#define DEFAULT_ROOM_HEIGHT 480

class RoomModel : public cugl::scene2::SceneNode {
private:
    // ROOM LOADING
    /** Loads in room formats from a JSON and is used to look up geometries for rooms */
    static shared_ptr<RoomLoader> _roomLoader;

    // STATUS
    /** Whether this room is currently locked/unable to be swapped. False by default */
    bool locked = false;

    // GEOMETRY
    /** Vector of physics objects forming the room's geometry */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> _physicsGeometry;
    /** Vector constant representing by how much the room geometry needs to be scaled */
    static const Vec2 ROOM_SCALE;

    /**
     * Creates all the polygons for any geometry for the room type with the given ID.
     * If no room ID is given, then it defaults to a room with only floor.
     * 
     * This is a private helper function that is only used within the class.
     *
     * @param roomID	ID of room type with the desired geometry
     */
    void buildGeometry(string roomID);

public:
#pragma mark Constructors
    /**
     * Creates a new, empty room.
     */
    RoomModel() {};

    //{@note by Barry on Feb 26: feature request: alloc() which returns shared ptr so I dont gave to make_shared in Gamescene}

    /**
     * Initializes an empty room at the world origin.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @return  true if the room is initialized properly, false otherwise.
     */
    bool init() { return init(0, 0, ""); }

    /**
     * Initializes an empty room at the given location.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param pos   The origin of the room in parent space
     * @return      true if the room is initialized properly, false otherwise.
     */
    bool init(const Vec2 pos) { return init(pos.x, pos.y, ""); }

    /**
     * Initializes an empty room at the given location.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x The x-coordinate of the room in parent space
     * @param y The y-coordinate of the room in parent space
     * @return  true if the room is initialized properly, false otherwise.
     */
    bool init(float x, float y) { return init(x, y, ""); }

    /**
     * Initializes a room with the type of the given ID at the world origin.
     * 
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param roomID    ID of room type with the desired geometry
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(string roomID) { return init(0, 0, roomID); }

    /**
     * Initializes a room with the type of the given ID at the given location.
     * 
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param pos       The origin of the room in parent space
     * @param roomID    ID of room type with the desired geometry
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(Vec2 pos, string roomID) { return init(pos.x, pos.y, roomID); }

    /**
     * Initializes a room with the given geometry at the given location.
     * 
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x         The x-coordinate of the room in parent space
     * @param y         The y-coordinate of the room in parent space
     * @param roomID    ID of room type with the desired geometry
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(float x, float y, string roomID);

#pragma mark Destructors
    /**
     * Destroys this room, releasing all resources.
     */
    virtual ~RoomModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this room.
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a room may not be used until it is initialized again.
     */
    void dispose();

#pragma mark -
#pragma mark Getters
    /**
     * Returns a shared pointer to the vector of physics objects that compose
     * the room geometry.
     * {@note by Barry Feb 26: documentation addition: must call buildPhysicsGeometry() before getting them?}
     * @return  Shared pointer to vector of physics objects for room geometry
     */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> getPhysicsGeometry() { return _physicsGeometry; }

    /**
     * Returns whether or not this room is currently locked, meaning it cannot
     * be swapped.
     * 
     * @return  Whether this room is locked, meaning it can't be swapped
     */
    bool isLocked() { return locked; }

#pragma mark Setters
    /**
     * Sets this room to be locked, meaning it can no longer be swapped.
     */
    void lock() { locked = true; }

    /**
     * Sets this room to be unlocked, meaning it can now be swapped.
     */
    void unlock() { locked = false; }
};

#endif /* MPRoomModel_h */