//
//  MPRoomModel.cpp
//  Malperdy
//
//  This class encapsulates all of the information for a single room, including
//  its geometry for both drawing and physics. Locations of everything within a
//  a room are stored relative to the room's origin, which is the lower left corner.
// 
//  Geometry within a room is defined as a percentage of the room's actual width/
//  height. Changing the macros for default room width/height in the headerfile will
//  scale the rooms accordingly.
// 
//  Rooms are not locked, meaning they can be swapped, by default upon creation.
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
//  Created by Kristina Gu on 2/22/22.
//  Copyright � 2022 Humblegends. All rights reserved.
//

#include "MPRoomModel.h"

using namespace cugl;

#pragma mark -
#pragma mark Room Display Constants
/** How big the boundary extrusion should be */
#define BOUND_WIDTH 10

#pragma mark Room Layout
/** The vertices for a floor in a room */
float FLOOR[] = {	0,		0,
					1,		0,
					1,	 0.25,
					0,	 0.25 };

/** The vertices for the boundary of a room */
// TODO: this is a dumb workaround to path not closing, fix it later
float BOUND[] = {				 0,						  0,
				DEFAULT_ROOM_WIDTH,						  0,
				DEFAULT_ROOM_WIDTH,		DEFAULT_ROOM_HEIGHT,
					-BOUND_WIDTH/2,		DEFAULT_ROOM_HEIGHT,
					-BOUND_WIDTH/2,						  0 };

/**
 * Converts the given geometry scaled to the room's dimensions.
 *
 * Room coordinates are given as a percentage of the room's actual
 * dimensions. This method converts takes in an array of these coordinates,
 * then modifies the array itself to convert to pixel space. The new pixel
 * coordinates can then be found in the original array.
 *
 * @param coords    Coordinates of room geometry, as percentage of actual dims
 */
void RoomModel::roomToPixelCoords(float coords[]) {
	// Get length of array
	int len = sizeof(coords);
	// Scale x-coordinates by room width and y-coordinates by room height
	for (int k = 0; k < len; k++) {
		// Even-indexed elements are x-coordinates
		if (k % 2 == 0) {
			coords[k] = coords[k] * DEFAULT_ROOM_WIDTH;
		}
		// Odd-indexed elements are y-coordinates
		else {
			coords[k] = coords[k] * DEFAULT_ROOM_HEIGHT;
		}
	}
}

/**
 * Rebuilds the geometry.
 *
 * This method should recreate all the polygons for any geometry in the room.
 * 
 * For now, it assumes every room only has the floor for geometry.
 */
void RoomModel::buildGeometry() {
	// Convert floor coordinates from room to pixel space
	roomToPixelCoords(FLOOR);

	// Create path for floor
	Path2 floorPath = Path2(reinterpret_cast<Vec2*>(FLOOR), size(FLOOR) / 2);
	// Fill floor
	EarclipTriangulator et = EarclipTriangulator();
	et.set(floorPath);
	et.calculate();

	// Convert floor into a scene graph node and add as child to room
	shared_ptr<scene2::PolygonNode> floorNode = scene2::PolygonNode::alloc();
	floorNode->setPolygon(et.getPolygon());
	floorNode->setColor(Color4::BLACK);
	addChild(floorNode);
}

/**
 * Rebuilds the physics geometry.
 *
 * This method should recreate all the physics objects corresponding to any
 * geometry in the room.
 */
void RoomModel::buildPhysicsGeometry() {
	// TODO: fix this with scene graphs

	// For every polygon in room geometry
	for (std::vector<Poly2>::iterator it = _geometry->begin(); it != _geometry->end(); ++it) {
		// Create a corresponding physics object for each
		shared_ptr<physics2::PolygonObstacle> physPoly = make_shared<physics2::PolygonObstacle>();
		// Generate PolygonObstacle and set the corresponding properties for level geometry
		physPoly = physics2::PolygonObstacle::alloc(*it, Vec2::ZERO);
		physPoly->setBodyType(b2_staticBody);
		// Store as part of the physics geometry
		_physicsGeometry->push_back(physPoly);
	}
}

#pragma mark -
#pragma mark Constructors
/**
 * Initializes a room with the given geometry at the given location.
 *
 * Rooms are automatically initialized to have the bounds given by
 * the default room width/height.
 *
 * @param x         The x-coordinate of the room in parent space
 * @param y         The y-coordinate of the room in parent space
 * @param geometry  Shared pointer to the vector of polygons containing the room's geometry
 * @return          true if the room is initialized properly, false otherwise.
 */
bool RoomModel::init(float x, float y, shared_ptr<vector<Poly2>> geometry) {
	// Create path node for room boundary
	Path2 boundPath = Path2(reinterpret_cast<Vec2*>(BOUND), size(BOUND) / 2);
	boundPath.closed = true;
	shared_ptr<scene2::PathNode> boundNode = scene2::PathNode::allocWithPath(boundPath, BOUND_WIDTH);
	boundNode->setColor(Color4::BLACK);
	boundNode->setClosed(true);
	addChild(boundNode);

	// Set geometry if given
	if (geometry != nullptr) _geometry = geometry;
	// Otherwise initialize vector for level geometry and build it
	else {
		_geometry = make_shared<std::vector<Poly2>>();
		buildGeometry();
	}

	// Initialize with the default room width/height and given position
	return this->initWithBounds(x, y, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);
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
	_geometry = nullptr;
	_physicsGeometry = nullptr;
	//this->dispose();
}
