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
bool GridModel::init(bool json, float hgap, float vgap)
{
    _horizontal_gap = hgap;
    _vertical_gap = vgap;
    
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

        return true;
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
            _grid.at(i).at(j)->init(j,i, rooms->at(_size.y * i+j)->asString());
            addChild(_grid.at(i).at(j));
          }
        }
        
        
        return true;
    }

};

/**
 * Init given size
 * @param width
 * @param height
 * @return a grid with width x height rooms
 */
bool GridModel::init(int width, int height)
{
  _size = Vec2(height, width);
  return init();
};

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
Vec2 GridModel::worldToRoomCoords(Vec2 coord)
{
    //TODO: convert to room row and column
    //TODO: return null if not corresponding to a room
    return coord;
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

/** returns all the physics geometry in the grid
 */
shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> GridModel::getPhysicsObjects()
{
  shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> obstacles = make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();

  for (vector<shared_ptr<RoomModel>> v : _grid)
  {
    for (shared_ptr<RoomModel> p : v)
    {
      shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> room_obstacles = p->getPhysicsGeometry();
      for (shared_ptr<physics2::PolygonObstacle> p : *room_obstacles)
      {
        obstacles->push_back(p);
      }
    }
  }
  return obstacles;
};
