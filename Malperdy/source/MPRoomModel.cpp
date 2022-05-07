//
//  MPRoomModel.cpp
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
//  Version: 4/16/22
//
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPRoomModel.h"
#include <cugl/util/CUDebug.h>
#include "MPCheckpoint.h"

using namespace cugl;

/** Initialize RoomLoader for loading in rooms from a JSON */
shared_ptr<RoomLoader> RoomModel::_roomLoader = RoomLoader::alloc("json/rooms.json");

#pragma mark -
#pragma mark Constants
/** Initialize scale by which rooms should be scaled to be in pixel space */
const Vec2 RoomModel::ROOM_SCALE = Vec2(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);

/** How big the boundary extrusion should be */
#define BOUND_WIDTH 10


// Create color for geometry
Color4 geometryColor = Color4(20,20,20,255);

#pragma mark Room Layout
/** The vertices for the boundary of a room */
// TODO: this is a dumb workaround to path not closing, fix it later
float BOUND[] = {				 0,						  0,
				DEFAULT_ROOM_WIDTH,						  0,
				DEFAULT_ROOM_WIDTH,		DEFAULT_ROOM_HEIGHT,
					-BOUND_WIDTH/2,		DEFAULT_ROOM_HEIGHT,
					-BOUND_WIDTH/2,						  0 };

/**
 * Creates all the polygons for any geometry for the room type with the given ID.
 * If no room ID is given, then it defaults to a room with only floor.
 *
 * This is a private helper function that is only used within the class.
 *
 * @param roomID	ID of room type with the desired geometry
 */
void RoomModel::buildGeometry(shared_ptr<JsonValue> roomJSON) {

//    string roomID = "";
//
//	// Get data for the room with the corresponding ID
//	// If no roomID is given, use a default room
//	shared_ptr<vector<shared_ptr<JsonValue>>> roomData = _roomLoader->getRoomData(roomID == "" ? "leftright" : roomID);

	// Initialize vector of physics objects for the room
	_physicsGeometry = make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();
	// Initialize vector of polygons for the room
	_geometry = make_shared<vector<shared_ptr<scene2::PolygonNode>>>();

	// Initialize variable to temporarily hold polygon info
	shared_ptr<Poly2> poly;
	vector<Vec2> verts;

    // get the geometry data from the JSON
    vector<int> data= roomJSON->get("layers")->get(0)->get("data")->asIntArray();

    // The tile in the top left corner (indicates geometry to be created)
    int tile = data.at(0);

    // room size
    int width = roomJSON->get("width")->asInt();
    int height = roomJSON->get("height")->asInt();
    PolyFactory pf = PolyFactory();
    Poly2 p;

    // for each tile,
    for(int i =  0; i < data.size(); i++){

        // get the current tile and its position within the room
        int current_tile = data.at(i);
        int curr_row = (height-1) - (i / width);
        int curr_col = i % width;

        // if the tile at this location should be created...
        if(current_tile == tile){

            // make a square in its position
            p = pf.makeRect(float(curr_col)/float(width), float(curr_row)/float(height), 1.0/float(width), 1.0/float(height));
            p *= ROOM_SCALE;

            // Convert polygon into a scene graph node and add as a child to the room node
            shared_ptr<scene2::PolygonNode> polyNode = scene2::PolygonNode::alloc();
            polyNode->setPolygon(p);
            polyNode->setColor(geometryColor);
            // Ensure that polygons are drawn to their absolute coordinates
            polyNode->setAbsolute(true);
            // Set position of polygon node accordingly
            addChild(polyNode);
            _geometry->push_back(polyNode);

            // Generate PolygonObstacle and set the corresponding properties for level geometry
            shared_ptr<physics2::PolygonObstacle> physPoly = physics2::PolygonObstacle::alloc(p, Vec2::ZERO);
            physPoly->setBodyType(b2_staticBody);
            // Store as part of the physics geometry
            _physicsGeometry->push_back(physPoly);

        }
    }

}

#pragma mark -
#pragma mark Constructors
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
 * @param bg		Background texture to apply to the room
 * @return          true if the room is initialized properly, false otherwise.
 */
bool RoomModel::init(float x, float y, shared_ptr<JsonValue> roomJSON, shared_ptr<Texture> bg) {
	// Add node for background texture if there is one
	if (bg != nullptr) {
		shared_ptr<scene2::PolygonNode> bgNode = scene2::PolygonNode::allocWithTexture(bg);
		bgNode->setPolygon(Rect(0, 0, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT));
		addChild(bgNode);
	}

	// Build geometry for the room type with the given ID
	buildGeometry(roomJSON);

	// Create path node for room boundary
	Path2 boundPath = Path2(reinterpret_cast<Vec2*>(BOUND), sizeof(BOUND) / 2);
	boundPath.closed = true;
	shared_ptr<scene2::PathNode> boundNode = scene2::PathNode::allocWithPath(boundPath, BOUND_WIDTH);
	boundNode->setColor(Color4(Vec4(0.65, 0.65, 0.65, 0.5)));
	boundNode->setClosed(true);
	addChild(boundNode);

    //Fog of war
	setColor(Color4(Vec4(0.2, 0.2, 0.2, 1)));
    destination = Vec2(x,y);
	// Initialize with the default room width/height and given position
	return this->initWithBounds(x * DEFAULT_ROOM_WIDTH, y * DEFAULT_ROOM_HEIGHT, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);
}

bool RoomModel::initTrap(TrapModel::TrapType type) {
    if (type == TrapModel::TrapType::SPIKE) {
        shared_ptr<SpikeTrap> trap = make_shared<SpikeTrap>();
        trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);

        _trap = trap;

        addChild(_trap);
    }
    else if (type == TrapModel::TrapType::TRAPDOOR) {
        shared_ptr<TrapDoor> trap = make_shared<TrapDoor>();
        trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);

        _trap = trap;

        addChild(_trap);
    }
    else if (type == TrapModel::TrapType::SAP){
        shared_ptr<SapTrap> trap = make_shared<SapTrap>();
        trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);

        _trap = trap;

        addChild(_trap);
    }
    else if (type == TrapModel::TrapType::CHECKPOINT) {
        shared_ptr<Checkpoint> trap = make_shared<Checkpoint>();
        trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT, false);

        _trap = trap;
        addChild(_trap);
    }
    else if (type == TrapModel::TrapType::GOAL) {
        shared_ptr<Checkpoint> trap = make_shared<Checkpoint>();
        trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT, true);

        _trap = trap;
        addChild(_trap);
    }
    else{
        return false;
    }
    return true;
}

#pragma mark Destructors
/**
 * Disposes all resources and assets of this room.
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a room may not be used until it is initialized again.
 */
void RoomModel::dispose() {
	removeAllChildren();
	_physicsGeometry = nullptr;
}

//void RoomModel::draw() {}
