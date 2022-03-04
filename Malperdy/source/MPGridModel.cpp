//
//  MPGridModel.cpp
//  Malperdy
//
// This class encapsulates all the information for the grid of rooms; meaning
// the data structure storing all rooms in a level and their relative positions.
// The grid is called to build the drawing and physics geometry for all its rooms.
//
// Grid is a subclass of SceneNode, and so all of SceneNode's methods can be used
//  with it. This allows the whole grid and its contents to be scaled properly
//  when zooming in and out. Each room is stored as child SceneNode of the grid.
//
//
//  Created Evan Azari on 2/27/22.
//  Copyright � 2022 Humblegends. All rights reserved.
//

#include "MPGridModel.h"

using namespace cugl;

/** Initialize RoomLoader for loading in rooms from a JSON */
shared_ptr<GridLoader> GridModel::_gridLoader = GridLoader::alloc("json/testlevel.json");

#pragma mark Constructors

/**
 * Deafult init
 * @return a grid with 3x3 rooms, each room the default
 */
bool GridModel::init(float scale, bool json, float hgap, float vgap)
{
    _horizontal_gap = hgap;
    _vertical_gap = vgap;
    _physics_scale =  scale;
    
    if(!json){
        _grid = vector<vector<shared_ptr<RoomModel>>>();
        for (int i = 0; i < _size.y; i++)
        {
          _grid.push_back(vector<shared_ptr<RoomModel>>());
          for (int j = 0; j < _size.x; j++)
          {
            _grid.at(i).push_back(make_shared<RoomModel>());
            addChild(_grid.at(i).at(j));
          }
        }

//        return true;
    }
    else{
        shared_ptr<vector<shared_ptr<JsonValue>>> rooms = _gridLoader->getRoomData();
        shared_ptr<JsonValue> rows = _gridLoader->getRows();
        shared_ptr<JsonValue> cols = _gridLoader->getCols();
        
        _size = Vec2(cols->asInt(), rows->asInt());
        
        _grid = vector<vector<shared_ptr<RoomModel>>>();
        for (int i = 0; i < _size.y; i++)
        {
          _grid.push_back(vector<shared_ptr<RoomModel>>());
          for (int j = 0; j < _size.x; j++)
          {
            _grid.at(i).push_back(make_shared<RoomModel>());
            _grid.at(i).at(j)->init(j*DEFAULT_ROOM_WIDTH ,i* DEFAULT_ROOM_HEIGHT, rooms->at(_size.y * i+j)->asString());
            addChild(_grid.at(i).at(j));
          }
        }
        
        
//        return true;
    }

    return this->scene2::SceneNode::init();
};

///**
// * Init given size
// * @param width
// * @param height
// * @return a grid with width x height rooms
// */
//bool GridModel::init(int width, int height)
//{
//  _size = Vec2(height, width);
//  return init();
//};

/**
 *  Init given size and a room template
 * @param width
 * @param height
 * @param room
 * @return a grid with width x height rooms, is of the form specified by roomID
 */
bool GridModel::init(int width, int height, string roomID)
{
  _size = Vec2(height, width);
  _horizontal_gap = 0;
  _vertical_gap = 0;

  _grid = vector<vector<shared_ptr<RoomModel>>>();
  for (int i = 0; i < _size.y; i++)
  {
    _grid.push_back(vector<shared_ptr<RoomModel>>());
    for (int j = 0; j < _size.x; j++)
    {
      _grid.at(i).push_back(make_shared<RoomModel>());
        _grid.at(i).at(j)->init(j,i,roomID);
      addChild(_grid.at(i).at(j));
    }
  }

  return true;
};

/**
 * {@note by Barry feature request}
 * @param width
 * @param height
 * @param jsonPath
 * @param vgap: minimal gaps between rooms
 * @param hgap: minimal hotizal gaps between rooms
 * @return
 */
bool GridModel::init(int width, int height, string jsonPath, int vgap, int hgap){
    return false;
};

#pragma mark Destructors
/**
 * Disposes all resources and assets of this grid.
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a room may not be used until it is initialized again.
 */
void GridModel::dispose()
{
  removeAllChildren();
};

#pragma mark Accessors

/** Returns a 1-D vector of all the rooms */
vector<shared_ptr<RoomModel>> GridModel::getRooms()
{
  vector<shared_ptr<RoomModel>> rooms = vector<shared_ptr<RoomModel>>();

  for (vector<shared_ptr<RoomModel>> v : _grid)
  {
    for (shared_ptr<RoomModel> p : v)
    {
      rooms.push_back(p);
    }
  }
  return rooms;
};

/** Returns the ptr to the room located at the coordinate.
 *
 * If coord = (i,j), then this returns the room at the ith row from the bottom,
 * jth column from the left */
shared_ptr<RoomModel> GridModel::getRoom(Vec2 coord)
{
  if (coord.x > _size.x || coord.y > _size.y)
  {
    return nullptr;
  }
  return _grid.at(coord.x).at(coord.y);
};

#pragma mark Setters

/** Sets the room located at the ith row from the bottom, jth column from the left  */
void GridModel::setRoom(Vec2 coord, shared_ptr<RoomModel> room)
{
  if (coord.x > _size.x || coord.y > _size.y)
  {
    return;
  }
  _grid.at(coord.x).at(coord.y) = room;
};

/** Swaps two rooms given two room coordinates.
 * room = (i,j) meaning the room at row i, col j
 * returns true if the swap occurs successfully, returns false if rooms cannot be swapped */
bool GridModel::swapRooms(Vec2 room1, Vec2 room2)
{
  if (!canSwap(room1, room2))
  {
    return false;
  }

  shared_ptr<RoomModel> temp = _grid.at(room1.x).at(room1.y);

  _grid.at(room1.x).at(room1.y) = _grid.at(room2.x).at(room2.y);
  _grid.at(room2.x).at(room2.y) = temp;
    _grid.at(room1.x).at(room1.y)->setPosition(Vec2(DEFAULT_ROOM_WIDTH*room1.x, DEFAULT_ROOM_HEIGHT *(_size.y-room1.y)));
    _grid.at(room2.x).at(room2.y)->setPosition(Vec2(DEFAULT_ROOM_WIDTH *room2.x, DEFAULT_ROOM_HEIGHT *(_size.y-room2.y)));
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = _grid.at(room1.x).at(room1.y)->getPhysicsGeometry()->begin(); itr != _grid.at(room1.x).at(room1.y)->getPhysicsGeometry()->end(); ++itr) {
        (*itr)->setEnabled(false);// TODO Replace me with obstacles translation
    }
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr = _grid.at(room2.x).at(room2.y)->getPhysicsGeometry()->begin(); itr != _grid.at(room2.x).at(room2.y)->getPhysicsGeometry()->end(); ++itr) {
        (*itr)->setEnabled(false);// TODO Replace me with obstacles translation for Room2
    }
  return true;
};

/** Returns whether the rooms can be swapped or not */
bool GridModel::canSwap(Vec2 room1, Vec2 room2)
{
  if (room1.x > _size.x || room1.y > _size.y)
  {
    return false;
  }
  if (room2.x > _size.x || room2.y > _size.y)
  {
    return false;
  }
  return !(_grid.at(room1.x).at(room1.y)->isLocked() ||
           _grid.at(room2.x).at(room2.y)->isLocked());
};

/**
 * Returns a shared pointer to the vector of physics objects that compose
 * the geometry of all rooms in the grid.
 *
 * @return  Shared pointer to vector of physics objects for room geometry
 */
shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> GridModel::getPhysicsObjects()
{
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> obstacles = make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();

    // Initialize variables to temporarily hold data
    Poly2 poly;
    shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> geometry;

    // For each room in the grid
    for (int col = 0; col < _size.x; col++) {
        for (int row = 0; row < _size.y; row++) {
            // Get pointers to PolygonNodes with the room's geometry
            geometry = _grid[row][col]->getGeometry();

            // For each polygon in the room
            for (vector<shared_ptr<scene2::PolygonNode>>::iterator itr = geometry->begin(); itr != geometry->end(); ++itr) {
                // Copy polygon data
                poly = (*itr)->getPolygon();
                // Get node to world transformation and apply to the polygon
                poly *= (*itr)->getNodeToWorldTransform();
                // Scale to physics space
                poly /= _physics_scale;

                // Create physics obstacle
                shared_ptr<physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(poly, Vec2::ZERO);
                obstacle->setBodyType(b2_staticBody);
                obstacles->push_back(obstacle);
            }
        }
    }
    
    // MAKE BOUNDS OF LEVEL
    // DUMB WORKAROUND
    Vec2 roomscale = Vec2(DEFAULT_ROOM_WIDTH, DEFAULT_ROOM_HEIGHT);
    float LEFTWALL[] = { 0, 0,
        0, _size.y * roomscale.y
    };
    float RIGHTWALL[] = { _size.x * roomscale.x, 0,
        _size.x * roomscale.x, _size.y * roomscale.y
    };
    float BOTTOMFLOOR[] = { 0, 0,
        _size.x * roomscale.x, 0
    };
    float TOPFLOOR[] = { 0, _size.y * roomscale.y,
        _size.x * roomscale.x, _size.y * roomscale.y
    };
    
    Path2 path;
    SimpleExtruder se = SimpleExtruder();
    
    path = Path2( reinterpret_cast<Vec2*>(LEFTWALL), size(LEFTWALL)/2);
    path.closed =  true;
    se.clear();
    se.set(path);
    se.calculate(0.1);
    shared_ptr<physics2::PolygonObstacle> left = physics2::PolygonObstacle::alloc(se.getPolygon()/_physics_scale, Vec2::ZERO);
    left->setBodyType(b2_staticBody);
    obstacles->push_back(left);
    
    path = Path2( reinterpret_cast<Vec2*>(RIGHTWALL), size(RIGHTWALL)/2);
    path.closed =  true;
    se.clear();
    se.set(path);
    se.calculate(0.1);
    shared_ptr<physics2::PolygonObstacle> right = physics2::PolygonObstacle::alloc(se.getPolygon()/_physics_scale, Vec2::ZERO);
    right->setBodyType(b2_staticBody);
    obstacles->push_back(right);
    
    path = Path2( reinterpret_cast<Vec2*>(BOTTOMFLOOR), size(BOTTOMFLOOR)/2);
    path.closed =  true;
    se.clear();
    se.set(path);
    se.calculate(0.1);
    shared_ptr<physics2::PolygonObstacle> bottom = physics2::PolygonObstacle::alloc(se.getPolygon()/_physics_scale, Vec2::ZERO);
    bottom->setBodyType(b2_staticBody);
    obstacles->push_back(bottom);
    
    path = Path2( reinterpret_cast<Vec2*>(TOPFLOOR), size(TOPFLOOR)/2);
    path.closed =  true;
    se.clear();
    se.set(path);
    se.calculate(0.1);
    shared_ptr<physics2::PolygonObstacle> top = physics2::PolygonObstacle::alloc(se.getPolygon()/_physics_scale, Vec2::ZERO);
    top->setBodyType(b2_staticBody);
    obstacles->push_back(top);
    /// END MAKING BOUNDS OF LEVEL

    return obstacles;
};

#pragma mark Helpers

Poly2 GridModel::convertToScreen(Poly2 poly){
    vector<Vec2> verts;
    for(Vec2 v  : poly.getVertices()){
        verts.push_back(this->scene2::SceneNode::nodeToWorldCoords(v));
    }
    return Poly2(verts);
};
