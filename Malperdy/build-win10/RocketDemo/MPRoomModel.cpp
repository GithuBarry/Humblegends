//
//  MPRoomModel.h
//  Malperdy
//
//  This class encapsulates all of the information for a single room, including
//  its geometry for both drawing and physics. Locations of everything within a
//  a room are stored relative to the room's origin, which is the upper left corner.
//
//  Created by Kristina Gu on 2/22/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#include "MPRoomModel.h"

using namespace cugl;

/**
 * Rebuilds the geometry.
 *
 * This method should recreate all the polygons for any geometry in the room.
 * It should also recreate all physics objects.
 */
void RoomModel::buildGeometry() {
	// TODO

	return;
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
	// Initialize with the default room width/height and given position
	this->initWithBounds(x, y, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);

	// Set geometry
	_geometry = geometry;

	// Build room geometry
	buildGeometry();
}