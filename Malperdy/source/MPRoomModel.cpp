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
Color4 fogColor = Color4(Color4(Vec4(0.2, 0.2, 0.2, 1)));

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
 * If no room ID is given, then it defaults to a solid room.
 *
 * This is a private helper function that is only used within the class.
 *
 * @param roomID	ID of room type with the desired geometry
 */
void RoomModel::buildGeometry(string roomID, int region) {
    // Get data for the room with the corresponding ID
	// If no roomID is given, use a default solid room
	shared_ptr<vector<shared_ptr<JsonValue>>> roomData = _roomLoader->getRoomData(roomID == "" ? "room_solid" : roomID);

	// Initialize vector of physics objects for the room
	_physicsGeometry = make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();
	// Initialize vector of polygons for the room
	_geometry = make_shared<vector<shared_ptr<scene2::PolygonNode>>>();

    // Initialize variable to temporarily hold polygon info
    shared_ptr<Poly2> poly;
    vector<Vec2> verts;

    // For each set of polygon coordinates in the room's geometry
    for (int k = 0; k < roomData->size(); k++) {
        // Get polygon
        poly = make_shared<Poly2>(roomData->at(k));
        // Scale coordinates to default room size
        poly->operator*=(ROOM_SCALE);
        // Ensure that all points are integers
        verts = poly->getVertices();
        for (vector<Vec2>::iterator itr = verts.begin(); itr != verts.end(); ++itr) {
            (*itr).x = floor((*itr).x);
            (*itr).y = floor((*itr).y);
        }

        // Convert polygon into a scene graph node and add as a child to the room node
        shared_ptr<scene2::PolygonNode> polyNode = scene2::PolygonNode::alloc();
        polyNode->setPolygon(*poly);
        if (region == 3) region = 5;
        GEOMETRY_COLOR = Color4(10 * region, 10 * region, 10 * region, 255);
        polyNode->setColor(GEOMETRY_COLOR);
        // Ensure that polygons are drawn to their absolute coordinates
        polyNode->setAbsolute(true);
        // Set position of polygon node accordingly
        addChild(polyNode);
        _geometry->push_back(polyNode);

        // Generate PolygonObstacle and set the corresponding properties for level geometry
        shared_ptr<physics2::PolygonObstacle> physPoly = physics2::PolygonObstacle::alloc(*poly, Vec2::ZERO);
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
 * The geometry corresponding to the room type given by the room ID is
 * taken from the JSON file of rooms.
 *
 * Rooms are automatically initialized to have the bounds given by
 * the default room width/height.
 *
 * @param x         The x-coordinate of the room in parent space
 * @param y         The y-coordinate of the room in parent space
 * @param roomID    ID of room type with the desired geometry
 * @param bg		Background texture to apply to the room (nullptr by default)
 * @return          true if the room is initialized properly, false otherwise.
 */
bool RoomModel::init(float x, float y, string roomID, int region, shared_ptr<Texture> bg) {
    this->setOrder(scene2::OrderedNode::Order::ASCEND);
    // Store room's original location
    _originalLoc = Vec2(x, y);

    // Add node for background texture if there is one
	if (bg != nullptr) {
        // Create ordered node so background nodes can be sorted
        _bgOrderNode = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
        addChild(_bgOrderNode);

        // Set background node's texture
		shared_ptr<scene2::PolygonNode> bgNode = scene2::PolygonNode::allocWithTexture(bg);
		bgNode->setPolygon(Rect(0, 0, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT));
		_bgOrderNode->addChild(bgNode);
        _bgNode = bgNode;

        // Set cleared background node, without texture for now
        shared_ptr<scene2::PolygonNode> bgClearedNode =
            scene2::PolygonNode::allocWithPoly(Rect(0, 0, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT));
        // Place it behind the original background
        bgClearedNode->setPriority(-1);
        _bgOrderNode->addChild(bgClearedNode);
        _bgClearedNode = bgClearedNode;
	}

    // Initialize lock icon
    _lockIcon = scene2::PolygonNode::allocWithFile("textures/lock_icon.png");
    _lockIcon->setAnchor(Vec2::ANCHOR_CENTER);
    Vec2 roomCorner = Vec2(DEFAULT_ROOM_WIDTH / 2, DEFAULT_ROOM_HEIGHT / 2);
    Vec2 padding = Vec2(-20, -20);
    _lockIcon->setPosition(roomCorner);
    _lockIcon->setColor(Color4(Vec4(1, 1, 1, .75)));
    _lockIcon->setScale(8);
    _lockIcon->setVisible(false);
    _lockIcon->setPriority(1);
    addChild(_lockIcon);

    
	// Build geometry for the room type with the given ID
	buildGeometry(roomID, region);

	// Create path node for room boundary
	/*Path2 boundPath = Path2(reinterpret_cast<Vec2*>(BOUND), sizeof(BOUND) / 2);
	boundPath.closed = true;
	shared_ptr<scene2::PathNode> boundNode = scene2::PathNode::allocWithPath(boundPath, BOUND_WIDTH);
	boundNode->setColor(Color4(Vec4(0.65, 0.65, 0.65, 0.5)));
	boundNode->setClosed(true);
	addChild(boundNode);*/

    //Fog of war
	setColor(fogColor);

    destination = Vec2(x,y);
	// Initialize with the default room width/height and given position
	return this->initWithBounds(x * DEFAULT_ROOM_WIDTH, y * DEFAULT_ROOM_HEIGHT, DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);
}

/**
 * Initializes a trap of the given type in this room.
 *
 * @param type  The type of the trap to instantiate
 * @param param An extra parameter that may do something different based on the trap type
 */
bool RoomModel::initTrap(TrapModel::TrapType type, bool param) {
    if (type == TrapModel::TrapType::SPIKE) {
        shared_ptr<SpikeTrap> trap = make_shared<SpikeTrap>();
        
        if(_physicsGeometry->size() == 0){
            trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT, true);
        }
        else{
            trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT, false);
        }


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
        trap->init(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT, false, param);

        _trap = trap;
        addChild(_trap);
        setColor(Color4::WHITE);
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
    _lockIcon = nullptr;
}

#pragma mark Setters
/**
 * Sets this room to be cleared, meaning the background will gradually
 * change to the given background.
 *
 * @param bg    The "cleared background" texture to change this room to.
 */
void RoomModel::clear(shared_ptr<Texture> bg) {
    // Don't do anything if already cleared or has no background (solid room)
    if (_isCleared || isSolid || !_bgClearedNode) return;
    _isCleared = true;

    // Set cleared background node's texture
    _bgClearedNode->setTexture(bg);
}

#pragma mark Updates
bool RoomModel::update(float dt) {
    // SHOWING LOCKS
    // Only show if the rooms are zoomed out and there's a pending lock state update
    if (isZoomedOut && lockChangePending) {
        updateLockedAppearance();
    }

    // BACKGROUND CLEAR TRANSITION
    // When a room is being cleared, transition smoothly between backgrounds

    // Only transition if the room is being cleared, so bgClear isn't nullptr
    if (_isCleared) {
        bgOpacity -= CLEAR_RATE;
        // If old background is now fully clear
        if (bgOpacity < 0.0f) {
            _bgNode = _bgClearedNode;
            _bgClearedNode = nullptr;
        }
        else _bgNode->setColor(Color4(Vec4(1, 1, 1, bgOpacity)));
    }

    // traps
    if (_trap != nullptr) {
        _trap->update(dt);
        return true;
    }

    return false;
}

/**
 * Execute animations
 *
 * @return whether it finished and does not need any more updates
 */
bool RoomModel::updateSwap() {
    // ROOM SWAP TRANSITION
    if ((Vec2(destination.x * DEFAULT_ROOM_WIDTH,
        destination.y * DEFAULT_ROOM_HEIGHT)).distance(SceneNode::getPosition()) < 10) {
        this->setPosition(destination.x, destination.y);
    }
    else {
        // Move the room over time, rather than just teleporting to its new destination
        float cur_x = SceneNode::getPosition().x;
        float cur_y = SceneNode::getPosition().y;
        float diff_x = destination.x * DEFAULT_ROOM_WIDTH - cur_x;

        if (abs(destination.x * DEFAULT_ROOM_WIDTH - cur_x) < 5) {
            this->SceneNode::setPosition(destination.x * DEFAULT_ROOM_WIDTH,
                destination.y * DEFAULT_ROOM_HEIGHT * (SWAP_SPEED)+cur_y * (1 - SWAP_SPEED));
        }
        else {
            float yfactor = 1 / (abs(diff_x) / 100 + 1);

            this->SceneNode::setPosition(destination.x * DEFAULT_ROOM_WIDTH * (SWAP_SPEED)+cur_x * (1 - SWAP_SPEED),
                destination.y * (yfactor)*DEFAULT_ROOM_HEIGHT + cur_y * (1 - yfactor));
        }
    }

    return false;
}
