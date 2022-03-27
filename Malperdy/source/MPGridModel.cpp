//
//  MPGridModel.cpp
//  Malperdy
//
//  This class encapsulates all the information for the grid of rooms; meaning
//  the data structure storing all rooms in a level and their relative positions.
//  The grid is called to build the drawing and physics geometry for all its rooms.
//
//  Grid is a subclass of SceneNode, and so all of SceneNode's methods can be used
//  with it. This allows the whole grid and its contents to be scaled properly
//  when zooming in and out. Each room is stored as child SceneNode of the grid.
//
//  Owner: Evan Azari
//  Contributors: Evan Azari, Barry Wang
//  Version: 2/21/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPGridModel.h"

using namespace cugl;

/** Initialize RoomLoader for loading in rooms from a JSON */
shared_ptr<GridLoader> GridModel::_gridLoader = GridLoader::alloc("json/testlevel2.json");

#pragma mark Constructors

/**
 * Deafult init
 * @param scale: the physics scale
 * @param json: true if GridModel should use the JSON
 * @param hgap: the horizontal gap between rooms (unimplemented)
 * @param vgap: the vertical gap between rooms (unimplemented)
 * @param bg    A default background to place in the back of rooms
 * @return a grid with 3x3 rooms, each room the default
 */
bool GridModel::init(float scale, bool json, float hgap, float vgap, shared_ptr<Texture> bg) {
    //cout<<"HERE YOU ARE 3"<<endl;
    _horizontal_gap = hgap;
    _vertical_gap = vgap;
    _physics_scale = scale;

    // Case where we don't load from JSON
    if (!json) {
        // Create a new grid
        _grid = shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>>();
        // Make a new room for each cell in the grid
        for (int i = 0; i < _size.y; i++) {
            _grid->push_back(make_shared<vector<shared_ptr<RoomModel>>>());
            for (int j = 0; j < _size.x; j++) {
                _grid->at(i)->push_back(make_shared<RoomModel>());
                addChild(_grid->at(i)->at(j));
            }
        }
        //cout<<"HERE YOU ARE 5"<<endl;
    } else {
        // Get level dimensions
        _size = _gridLoader->getDims();
        // Initialize grid to store RoomModels in level
        _grid = make_shared<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>>();
        // Initialize pointer to temporarily point to RoomModels
        shared_ptr<RoomModel> room;

        // For each room in the level
        for (int y = 0; y < _size.y; y++) {
            // Initialize vector for a row of rooms
            shared_ptr<vector<shared_ptr<RoomModel>>> roomRow = make_shared<vector<shared_ptr<RoomModel>>>();
            // Construct and store the RoomModel corresponding to the ID of the room at this location
            for (int x = 0; x < _size.x; x++) {
                room = RoomModel::alloc(x, y, _gridLoader->getRoomAt(x, y), bg);
                roomRow->push_back(room);
                // Add room to scene graph
                addChild(room);
            }
            // Add row of rooms to the full grid
            _grid->push_back(roomRow);
        }
        //cout<<"HERE YOU ARE 6"<<endl;
    }

    // TODO: REPLACE THIS CODE LATER TO THE JSON
    // TODO: WHY DOES THIS CODE HAVE NO IMPACT ON ANYTHING
    // create organized back of physics geometry
    //calculatePhysicsGeometry();
    _grid->at(1)->at(1)->initTrap("trapdoor");
    _grid->at(1)->at(3)->initTrap("spike");
//    cout<<"HERE YOU ARE 1"<<endl;
//    cout<<_grid->at(0)->at(1)->initTrap("spike")<<endl;
//    cout<<"HERE YOU ARE 2"<<endl;

    return this->scene2::SceneNode::init();
};

#pragma mark Destructors

/**
 * Disposes all resources and assets of this grid.
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a room may not be used until it is initialized again.
 */
void GridModel::dispose() {
    removeAllChildren();
};

#pragma mark Accessors

/** Returns a 1-D vector of all the rooms */
vector<shared_ptr<RoomModel>> GridModel::getRooms() {
    // The value to return
    vector<shared_ptr<RoomModel>> rooms = vector<shared_ptr<RoomModel>>();
    // For each room in the 2D grid
    for (vector<shared_ptr<vector<shared_ptr<RoomModel>>>>::iterator rowItr = _grid->begin(); rowItr != _grid->end(); ++rowItr) {
        for (vector<shared_ptr<RoomModel>>::iterator colItr = (*rowItr)->begin(); colItr != (*rowItr)->end(); ++colItr) {
            // Add the room to the 1D vector
            rooms.push_back(*colItr);
        }
    }
    return rooms;
};

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
shared_ptr<RoomModel> GridModel::getRoom(int x, int y) {
    // Return null pointer if out of bounds
    if (x >= _size.x || x < 0 || y < 0 || y >= _size.y) {
        return nullptr;
    }
    return _grid->at(y)->at(x);


};

#pragma mark Setters

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
bool GridModel::setRoom(int x, int y, shared_ptr<RoomModel> room) {
    // Return false if coordinate is out of bounds
    if (x >= _size.x || y >= _size.y) {
        return false;
    }
    // Otherwise, set room at coordinate to be given room and return true
    _grid->at(y)->at(x) = room;
    return true;
};

/**
 * Swaps the two rooms at the given coordinates, where the x-coordinate is
 * the column from the left and the y-coordinate is the row from the bottom.
 *
 * Returns true if the swap was successfully performed.
 *
 * @param room1 The coordinates of the first room to swap in (column, row) form
 * @param room2 The coordinates of the second room to swap in (column, row) form
 * @return      Whether the swap was performed successfully
 */
bool GridModel::swapRooms(Vec2 room1, Vec2 room2) {
    // Don't swap if not allowed
    if (!canSwap(room1, room2)) return false;

    shared_ptr<RoomModel> temp = getRoom(room1);
    // Store Room2 at Room1's old location
    setRoom(room1, getRoom(room2));
    // Update Room2's location accordingly
    getRoom(room2)->setPosition(room1);
    // Store Room1 at Room2's old location
    setRoom(room2, temp);
    // Update Room1's location accordingly
    temp->setPosition(room2);

    // For each obstacle in room1
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = _physicsGeometry.at(room1.y).at(room1.x).begin(); itr != _physicsGeometry.at(room1.y).at(room1.x).end(); ++itr) {

        // convert the position from physics to grid space
        Vec2 newPos = (*itr)->getPosition() * _physics_scale;
        newPos = this->worldToNodeCoords(newPos);
        // offset the obstacles in grid space
        newPos.x -= (room1.x - room2.x) * DEFAULT_ROOM_WIDTH;
        newPos.y -= (room1.y - room2.y) * DEFAULT_ROOM_HEIGHT;
        // convert back to physics space
        newPos = this->nodeToWorldCoords(newPos) / _physics_scale;
        (*itr)->setPosition(newPos);

    }

    // For each obstacle in room2
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = _physicsGeometry.at(room2.y).at(room2.x).begin(); itr != _physicsGeometry.at(room2.y).at(room2.x).end(); ++itr) {

        // convert the position from physics to grid space
        Vec2 newPos = (*itr)->getPosition() * _physics_scale;
        newPos = this->worldToNodeCoords(newPos);
        // offset the obstacles in grid space
        newPos.x += (room1.x - room2.x) * DEFAULT_ROOM_WIDTH;
        newPos.y += (room1.y - room2.y) * DEFAULT_ROOM_HEIGHT;
        // convert back to physics space
        newPos = this->nodeToWorldCoords(newPos) / _physics_scale;
        (*itr)->setPosition(newPos);
    }

    // swap the vectors of obstacles in _physicsGeometry
    // create a temp holder
    vector<shared_ptr<physics2::PolygonObstacle>> tempPhysics = _physicsGeometry.at(room1.y).at(room1.x);
    _physicsGeometry.at(room1.y).at(room1.x) = _physicsGeometry.at(room2.y).at(room2.x);
    _physicsGeometry.at(room2.y).at(room2.x) = tempPhysics;

    return true;
};

/**
 *
 * Returns whether the two given rooms can be swapped
 *
 * @param room1 The coordinates of the first room to swap in (column, row) form
 * @param room2 The coordinates of the second room to swap in (column, row) form
 * @return      Whether the rooms can be swappedd
 */
bool GridModel::canSwap(Vec2 room1, Vec2 room2) {
    // If the room bounds are out of bounds
    if (room1.x >= _size.x || room1.y >= _size.y) {
        return false;
    }
    if (room2.x < 0 || room2.y < 0) {
        return false;
    }
    // If the bounds are correct, then return if both rooms are unlocked
    return !(_grid->at(room1.y)->at(room1.x)->isLocked() ||
            _grid->at(room2.y)->at(room2.x)->isLocked());
};

/**
 *
 * Flattens _physicsGeometry, adds the bounds, and returns this vector
 *
 * @return a flat version of _physicsGeometry, with the level bounds included
 */
shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> GridModel::getPhysicsObjects() {
    // flat vector to store all room obstacles
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> obstacles = make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();

    calculatePhysicsGeometry();

    // For each room in the grid
    for (int col = 0; col < _size.x; col++) {
        for (int row = 0; row < _size.y; row++) {

            // For each polygon in the room
            for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = _physicsGeometry.at(row).at(col).begin(); itr != _physicsGeometry.at(row).at(col).end(); ++itr) {

                // add the obstacle to the flat vector
                obstacles->push_back(*itr);
            }
        }
    }

    // MAKE BOUNDS OF LEVEL

    // Get room dimensions scaled by grid scale
    Vec2 roomscale = Vec2(DEFAULT_ROOM_WIDTH * getScaleX(), DEFAULT_ROOM_HEIGHT * getScaleY());
    // Create path for level bounds
    Path2 path = Path2(Rect(0, 0, _size.x * roomscale.x, _size.y * roomscale.y));
    path.closed = true;
    // Create geometry for level bounds
    SimpleExtruder se = SimpleExtruder();
    se.set(path);
    se.calculate(0.1f);
    shared_ptr<physics2::PolygonObstacle> ob = physics2::PolygonObstacle::alloc(se.getPolygon() / _physics_scale, Vec2::ZERO);
    ob->setBodyType(b2_staticBody);
    obstacles->push_back(ob);

    /// END MAKING BOUNDS OF LEVEL

    return obstacles;
};

#pragma mark Helpers

/**
 * Returns the ptr to the room located at the coordinate.
 *
 * If coord = (i,j), then this returns the room at the ith row from the bottom,
 * jth column from the left */
//Vec2 GridModel::worldToRoomCoords(Vec2 coord)
//{
//    //TODO: convert to room row and column
//    //TODO: return null if not corresponding to a room
//    return coord;
//};

Poly2 GridModel::convertToScreen(Poly2 poly) {
    vector<Vec2> verts;
    for (Vec2 v: poly.getVertices()) {
        verts.push_back(this->scene2::SceneNode::nodeToWorldCoords(v));
    }
    return Poly2(verts);
};

void GridModel::calculatePhysicsGeometry() {
    _physicsGeometry = vector<vector<vector<shared_ptr<physics2::PolygonObstacle>>>>();

    // For each room in _grid...
    for (int row = 0; row < _size.y; row++) {
        _physicsGeometry.push_back(vector<vector<shared_ptr<physics2::PolygonObstacle>>>());
        for (int col = 0; col < _size.x; col++) {
            _physicsGeometry.at(row).push_back(vector<shared_ptr<physics2::PolygonObstacle>>());
            // Get pointers to PolygonNodes with the room's geometry
            shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> geometry = getRoom(col, row)->getGeometry();

            // For each polygon in the room
            for (vector<shared_ptr<scene2::PolygonNode>>::iterator itr = geometry->begin(); itr != geometry->end(); ++itr) {
                // Copy polygon data
                Poly2 poly = (*itr)->getPolygon();
                // Get node to world transformation and apply to the polygon
                poly *= (*itr)->getNodeToWorldTransform();
                // Scale to physics space
                poly /= _physics_scale;

                // Create physics obstacle
                shared_ptr<physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(poly, Vec2::ZERO);
                obstacle->setBodyType(b2_staticBody);

                _physicsGeometry.at(row).at(col).push_back(obstacle);
            }

            // TODO: Inspect code for bug
            // TODO: Why does this have no impact on instantiation
            // if the room has a trap
            if (true && _grid->at(row)->at(col)->getTrap()) {
                shared_ptr<scene2::PolygonNode> pn = _grid->at(row)->at(col)->getTrap()->getPolyNode();
                Poly2 p = pn->getPolygon();
                p *= pn->getNodeToWorldTransform();
                p /= _physics_scale;

                // Create physics obstacle
                shared_ptr<physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(p, Vec2::ZERO);
                obstacle->setBodyType(b2_staticBody);

                _physicsGeometry.at(row).at(col).push_back(obstacle);
                _grid->at(row)->at(col)->getTrap()->initObstacle(obstacle);
                if(_grid->at(row)->at(col)->getTrap()->getType()=="trapdoor"){
                    _grid->at(row)->at(col)->getTrap()->getObstacle()->setSensor(true);
                }
            }
        }
    }

}
