//
//  MPRoomModel.h
//  Malperdy
//
//  This class encapsulates all of the information for a single room, including
//  its geometry for both drawing and physics. Locations of everything within a
//  a room are stored relative to the room's origin, which is the lower left corner.
// 
//  Room geometries are read from a JSON and stored in a static lookup table shared
//  among all instances of RoomModel.
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
//  Owner: Kristina Gu
//  Contributors: Kristina Gu, Jordan Selin
//  Version: 3/28/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPRoomModel_h
#define MPRoomModel_h

#include <cugl/cugl.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <math.h>

#include "MPRoomLoader.h"
#include "MPTrapModel.hpp"
#include "MPSpikeTrap.hpp"
#include "MPTrapDoorModel.hpp"

using namespace cugl;

/** The default width of a room in grid space */
#define DEFAULT_ROOM_WIDTH 720
/** The default height of a room in grid space */
#define DEFAULT_ROOM_HEIGHT 480
/** The ID of the default room type */
#define DEFAULT_ROOM_ID "leftrightupdown"

class RoomModel : public cugl::scene2::SceneNode {
private:
    // ROOM LOADING
    /** Loads in room formats from a JSON and is used to look up geometries for rooms */
    static shared_ptr<RoomLoader> _roomLoader;

    // STATUS
    /** Whether this room is currently locked/unable to be swapped. False by default */
    bool locked = false;
    /* Whether this room's contents are currently hidden. False by default */
    bool fogged = true;
    


    // GEOMETRY
    /** Vector of polygon nodes forming the room's geometry */
    shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> _geometry;
    /** Vector of physics objects forming the room's geometry */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> _physicsGeometry;
    /** Vector constant representing by how much the room geometry needs to be scaled */
    static const Vec2 ROOM_SCALE;
    
    shared_ptr<TrapModel> _trap  = nullptr;

    /**
     * Creates all the polygons for any geometry for the room type with the given ID.
     * If no room ID is given, then it defaults to a room with only floor.
     * 
     * This is a private helper function that is only used within the class.
     *
     * @param roomID	ID of room type with the desired geometry
     */
    void buildGeometry(shared_ptr<JsonValue> roomJSON);

public:
#pragma mark Constructors
    /**
     * Creates a new, empty room.
     */
    RoomModel() {};

//    /**
//     * Initializes the default room at the origin, which is the lower left
//     * corner of the grid.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @return  true if the room is initialized properly, false otherwise.
//     */
//    bool init() { return init(0, 0, ""); }

//    /**
//     * Initializes the default room at the given location in grid space,
//     * where the location is given as a row and column to place the room
//     * at. These coordinates will be scaled by the room width and height to
//     * be placed in world space.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param pos   The room's location in grid space in the form (column, row)
//     * @return      true if the room is initialized properly, false otherwise.
//     */
//    bool init(const Vec2 pos) { return init(pos.x, pos.y, ""); }

//    /**
//     * Initializes the default room at the given location in grid space,
//     * where the location is given as a row and column to place the room
//     * at. These coordinates will be scaled by the room width and height to
//     * be placed in world space.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param x The column of the room in grid space
//     * @param y The row of the room in grid space
//     * @return  true if the room is initialized properly, false otherwise.
//     */
//    bool init(float x, float y) { return init(x, y, ""); }

//    /**
//     * Initializes the room with the type of the given ID at the origin, which
//     * is the lower left corner of the grid.
//     *
//     * The geometry corresponding to the room type given by the room ID is
//     * taken from the JSON file of rooms.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param roomID    ID of room type with the desired geometry
//     * @return          true if the room is initialized properly, false otherwise.
//     */
//    bool init(string roomID) { return init(0, 0, roomID); }

//    /**
//     * Initializes the room with the type of the given ID at the given
//     * location in grid space, where the location is given as a row and
//     * column to place the room at. These coordinates will be scaled by
//     * the room width and height to be placed in world space.
//     *
//     * The geometry corresponding to the room type given by the room ID is
//     * taken from the JSON file of rooms.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param pos       The room's location in grid space in the form (column, row)
//     * @param roomID    ID of room type with the desired geometry
//     * @return          true if the room is initialized properly, false otherwise.
//     */
//    bool init(Vec2 pos, string roomID) { return init(pos.x, pos.y, roomID); }

    /**
     * Initializes the room with the type of the given ID at the given
     * location in grid space, where the location is given as a row and
     * column to place the room at. These coordinates will be scaled by
     * the room width and height to be placed in world space.
     * 
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x         The column of the room in grid space
     * @param y         The row of the room in parent space
     * @param roomID    ID of room type with the desired geometry
     * @param bg		Background texture to apply to the room
     * @return          true if the room is initialized properly, false otherwise.
     */
    bool init(float x, float y, shared_ptr<JsonValue> roomJSON, shared_ptr<Texture> bg = nullptr);
    
    /** */
    bool initTrap(string type);

#pragma mark Static Constructors
//    /**
//     * Returns a default room at the origin, which is the lower left
//     * corner of the grid.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @return  A newly-allocated RoomModel at the origin.
//     */
//    static std::shared_ptr<RoomModel> alloc() {
//        std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
//        return (result->init() ? result : nullptr);
//    }

//    /**
//     * Returns a newly-allocated default room at the given location in
//     * grid space, where the location is given as a row and column to place
//     * the room at. These coordinates will be scaled by the room width and
//     * height to be placed in world space.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param pos   The room's location in grid space in the form (column, row)
//     * @return      A newly-allocated RoomModel
//     */
//    static std::shared_ptr<RoomModel> alloc(const Vec2 pos) {
//        std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
//        return (result->init(pos) ? result : nullptr);
//    }

//    /**
//     * Returns a newly-allocated default room at the given location in
//     * grid space, where the location is given as a row and column to place
//     * the room at. These coordinates will be scaled by the room width and
//     * height to be placed in world space.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param x The column of the room in grid space
//     * @param y The row of the room in grid space
//     * @return  A newly-allocated RoomModel
//     */
//    static std::shared_ptr<RoomModel> alloc(float x, float y) {
//        std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
//        return (result->init(x, y) ? result : nullptr);
//    }

//    /**
//     * Returns a newly-allocated room with the type of the given ID at the
//     * origin, which is the lower left corner of the grid.
//     *
//     * The geometry corresponding to the room type given by the room ID is
//     * taken from the JSON file of rooms.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param roomID    ID of room type with the desired geometry
//     * @return          A newly-allocated RoomModel
//     */
//    static std::shared_ptr<RoomModel> alloc(string roomID) {
//        std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
//        return (result->init(roomID) ? result : nullptr);
//    }

//    /**
//     * Returns a newly-allocated room with the type of the given ID at
//     * the given location in grid space, where the location is given as
//     * a row and column to place the room at. These coordinates will be
//     * scaled by the room width and height to be placed in world space.
//     *
//     * The geometry corresponding to the room type given by the room ID is
//     * taken from the JSON file of rooms.
//     *
//     * Rooms are automatically initialized to have the bounds given by
//     * the default room width/height.
//     *
//     * @param pos       The room's location in grid space in the form (column, row)
//     * @param roomID    ID of room type with the desired geometry
//     * @return          A newly-allocated RoomModel
//     */
//    static std::shared_ptr<RoomModel> alloc(Vec2 pos, string roomID) {
//        std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
//        return (result->init(pos, roomID) ? result : nullptr);
//    }

    /**
     * Returns a newly-allocated room with the type of the given ID at
     * the given location in grid space, where the location is given as
     * a row and column to place the room at. These coordinates will be
     * scaled by the room width and height to be placed in world space.
     *
     * The geometry corresponding to the room type given by the room ID is
     * taken from the JSON file of rooms.
     *
     * Rooms are automatically initialized to have the bounds given by
     * the default room width/height.
     *
     * @param x         The column of the room in grid space
     * @param y         The row of the room in grid space
     * @param roomID    ID of room type with the desired geometry
     * @param bg		Background texture to apply to the room
     * @return          A newly-allocated RoomModel
     */
    static std::shared_ptr<RoomModel> alloc(float x, float y, shared_ptr<JsonValue> roomJSON, shared_ptr<Texture> bg) {
        std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
        return (result->init(x, y, roomJSON, bg) ? result : nullptr);
    }

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
    
    bool permlocked = false;
    
    /**
     * Returns a shared pointer to the vector of physics objects that compose
     * the room geometry.
     * 
     * @return  Shared pointer to vector of physics objects for room geometry
     */
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> getPhysicsGeometry() { return _physicsGeometry; }

    /**
     * Returns a shared pointer to the vector of polygon nodes that compose the
     * room geometry.
     *
     * @return  Shared pointer to vector of polygon nodes for room geometry
     */
    shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> getGeometry() { return _geometry; }

    /**
     * Returns whether or not this room is currently locked, meaning it cannot
     * be swapped.
     * 
     * @return  Whether this room is locked, meaning it can't be swapped
     */
    bool isLocked() { return locked || permlocked; }

    /**
     * Returns whether or not this room has fog of war, meaning its 
     * contents are hidden.
     *
     * @return  true if this room has fog of war
     */
    bool isFogged() { return fogged; }
    
    shared_ptr<TrapModel> getTrap(){
        return _trap;
    }

#pragma mark Setters
    /**
     * Sets this room to be locked, meaning it can no longer be swapped.
     */
    void lock() { locked = true; }

    /**
     * Sets this room to be unlocked, meaning it can now be swapped.
     */
    void unlock() { locked = false; }

    /**
     * Sets whether this room is fogged (contents hidden) or not.
     */
    void setFogged(bool isFogged) { fogged = isFogged; }

    /**
     * Sets this room to be at the given location in grid space, where the
     * location is given as a row and column to place the room at. These
     * coordinates will be scaled by the room width and height to be placed
     * in world space.
     * 
     * @param value The room's desired location in grid space in the form (column, row)
     */
    void setPosition(const Vec2 value) { this->setPosition(value.x, value.y); }

    /**
     * Sets this room to be at the given location in grid space, which is
     * given as a row and column to place the room at. These coordinates
     * will be scaled by the room width and height to be placed in world space.
     *
     * @param x The column of the room in grid space
     * @param y The row of the room in grid space
     */
    void setPosition(float x, float y) { this->SceneNode::setPosition(x * DEFAULT_ROOM_WIDTH, y * DEFAULT_ROOM_HEIGHT); }
};

#endif /* MPRoomModel_h */
