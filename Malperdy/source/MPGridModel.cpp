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
//  Contributors: Evan Azari, Barry Wang, Jordan Selin
//  Version: 4/19/22
//
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPGridModel.h"

using namespace cugl;

#pragma mark Constructors

/**
 * Initializes and returns a grid of rooms with the given dimensions.
 *
 * @param width     The width/number of columns of rooms that the grid should have
 * @param height    The height/number of rows of rooms that the grid should have
 * @return          The initialized grid with the given dimensions
 */
shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> GridModel::initGrid(int width, int height)
{
    // Initialize grid
    shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> grid =
        make_shared<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>>();
    // Initialize RoomModel cache to temporarily store RoomModels
    shared_ptr<RoomModel> room;

    // Fill the grid with uninstantiated rooms
    for (int y = 0; y < height; y++)
    {
        // Initialize vector for a row of rooms
        shared_ptr<vector<shared_ptr<RoomModel>>> roomRow = make_shared<vector<shared_ptr<RoomModel>>>();

        for (int x = 0; x < width; x++)
        {
            room = make_shared<RoomModel>();
            roomRow->push_back(room);
        }
        // Add row of rooms to the grid
        grid->push_back(roomRow);
    }

    return grid;
}

/**
 * Initializes a region for the game. The region is placed such that its
 * lower left corner, its region origin, is at the given coordinates in the
 * overall grid space. Returns the bounds of the newly-created region.
 *
 * Note that everything here uses REGION space, not grid space. The only coordinates
 * in grid space are the region's origin, which will be used to organize the regions,
 * but otherwise regions only care about where things are relative to their origin.
 *
 * @param regionMetadata    The JSONValue for the metadata of the region to be initialized
 */
void GridModel::initRegion(shared_ptr<JsonValue> regionMetadata)
{
    int width = regionMetadata->getInt("width");
    int height = regionMetadata->getInt("height");
    int originX = regionMetadata->getInt("originX");
    int originY = regionMetadata->getInt("originY");

    // First create a new region with the given metadata
    shared_ptr<RegionModel> region = RegionModel::alloc(
        regionMetadata->getString("name"),
        regionMetadata->getInt("type"),
        width, height, originX, originY
    );
    _regions->push_back(region);

    // Now load in the actual region JSON
    shared_ptr<JsonValue> regionJSON = _assets->get<JsonValue>(regionMetadata->getString("name"));

    // Initialize grid for a sublevel layer (dimensions are the dimensions of the region)
    shared_ptr<vector<shared_ptr<vector<shared_ptr<RoomModel>>>>> sublevel = initGrid(width, height);

    // Start reading data from the JSON
    shared_ptr<JsonValue> layers = regionJSON->get("layers");

    // INSTANTIATING TRAPS
    // Find the offset for the entities tileset
    int entity_offset = 0;
    int room_offset = 0;
    shared_ptr<JsonValue> tilesets = regionJSON->get("tilesets");

    // loop through the tilesets of the level
    for (int i = 0; i < tilesets->size(); i++)
    {

        // if the tileset is the entities tileset, set the tileset offset
        if (tilesets->get(i)->get("source")->asString().find("entities") != string::npos)
        {
            entity_offset = tilesets->get(i)->get("firstgid")->asInt();
        }

        // if the tileset is the entities tileset, set the tileset offset
        if (tilesets->get(i)->get("source")->asString().find("rooms") != string::npos)
        {
            room_offset = tilesets->get(i)->get("firstgid")->asInt();
        }
    }

    // INSTANTIATING ROOMS

    // Room JSON cache
    shared_ptr<JsonValue> roomJSON;
    // Whether the room is solid
    bool isSolid = false;

    // Initialize variables to determine sublevel bounds
    int xMin, yMin, xMax, yMax;

    // Go through each layer to find the sublevel layers
    for (int i = 0; i < layers->size(); i++)
    {
        shared_ptr<JsonValue> layer = layers->get(i);

        // Parse each sublevel if the layer name contains "sublevel"
        if (layer->get("name")->asString().find("sublevel") != string::npos) {
            // Set min coords to high values and max coords to low values
            xMin = width;
            yMin = height;
            xMax = 0;
            yMax = 0;

            // For each room in the sublevel
            for (int j = 0; j < layer->get("data")->size() / _roomWidth / _roomHeight; j++) {
                isSolid = false;

                // These coordinates are from the UPPER left
                int x = j % width;
                int y = (j / width);

                // The room ID
                int bottom_corner = x * _roomWidth + (y + 1) * _roomWidth * width * _roomHeight - _roomWidth * width;
                int room_id = layer->get("data")->get(bottom_corner)->asInt();

                // If room ID = 0, there's no room there, so go on to the next room
                if (!room_id) continue;

                //string im = _roomsTileset->get("tiles")->get(room_id - room_offset)->get("image")->asString();
                shared_ptr<JsonValue> json1 = _roomsTileset->get("tiles");
                json1 = json1->get(room_id - room_offset);
                json1 = json1->get("image");
                string im = json1->asString();

                // Get the ID/name of the desired room type
                string roomID;
                if (im.rfind("solid") != string::npos)
                {
                    roomID = "room_solid";
                    isSolid = true;
                }
                else
                {
                    int p = im.rfind("room");
                    roomID = im.substr(p);
                    roomID = roomID.substr(0, roomID.length() - 4);
                }

                // Flip the y, so now the coordinates are from the lower left
                y = height - 1 - y;

                // instantiate the room and add it as a child
                // Make sure to place it at the right place in GRID space
                sublevel->at(y)->at(x)->init(x + originX, y + originY, roomID,
                    isSolid ? nullptr : RegionModel::getRandBG(region->getType()));
                if (isSolid) sublevel->at(y)->at(x)->setSolid();
                addChild(sublevel->at(y)->at(x));

                // Update min/max bounds if needed
                if (x <= xMin && y <= yMin) {
                    xMin = x;
                    yMin = y;
                }
                if (x >= xMax && y >= yMax) {
                    xMax = x;
                    yMax = y;
                }
            }
            // Store the final sublevel in the region
            // Sublevel origin is the min bound
            region->addSublevel(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1, sublevel);
        }
    }

    // TRAPS

    // create a map that maps values in data[] to entity names
    map<int, string> tile_to_traps = map<int, string>();
    shared_ptr<JsonValue> ent = _assets->get<JsonValue>("tileset_entities");

    // TODO: add more options for entities
    // go through all tiles in the entities tileset
    for (int i = 0; i < ent->get("tiles")->size(); i++)
    {

        // depending on the type of tile, add its mapping
        shared_ptr<JsonValue> the_tile = ent->get("tiles")->get(i);

        // if the tile represents reynard, then add a mapping for reynard according to the tile id
        if (the_tile->get("image")->asString().find("reynard") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "reynard";
        }
        else if (the_tile->get("image")->asString().find("spike") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "spike";
        }
        else if (the_tile->get("image")->asString().find("trapdoor") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "trapdoor";
        }
        else if (the_tile->get("image")->asString().find("keycheckpoint") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "keycheckpoint";
        }
        else if (the_tile->get("image")->asString().find("checkpoint") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "checkpoint";
        }
        else if (the_tile->get("image")->asString().find("keyenemy") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "keyenemy";
        }
        else if (the_tile->get("image")->asString().find("key") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "key";
        }
        else if (the_tile->get("image")->asString().find("locked") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "locked";
        }
        else if (the_tile->get("image")->asString().find("sap") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "sap";
        }
        else if (the_tile->get("image")->asString().find("exit") != string::npos)
        {
            tile_to_traps[the_tile->get("id")->asInt() + entity_offset] = "exit";
        }
    }

    // now, go through the data, and for each encountered entity, if it is a trap then add it to the appropriate room
    // first, loop through the level layers again
    for (int i = 0; i < regionJSON->get("layers")->size(); i++)
    {
        shared_ptr<JsonValue> layer = regionJSON->get("layers")->get(i);

        // if the layer is a tilelayer...
        if (layer->get("name")->asString().find("entities") != string::npos)
        {
            vector<int> data = layer->get("data")->asIntArray();

            // loop through its data
            for (int j = 0; j < data.size(); j++)
            {

                // if the data element represents an entity
                if (data.at(j) != 0)
                {

                    // calculate the room coordinate that the data element is in
                    int curr_row = ((_roomHeight * height - 1) - (j / (_roomWidth * width))) / _roomHeight;
                    int curr_col = (j % (_roomWidth * width)) / _roomWidth;

                    // if the tile is a trap, then add it
                    if (tile_to_traps[data.at(j)] == "trapdoor")
                    {
                        sublevel->at(curr_row)->at(curr_col)->initTrap(TrapModel::TrapType::TRAPDOOR);
                    }
                    else if (tile_to_traps[data.at(j)] == "spike")
                    {
                        sublevel->at(curr_row)->at(curr_col)->initTrap(TrapModel::TrapType::SPIKE);
                    }
                    else if (tile_to_traps[data.at(j)].find("checkpoint") != string::npos)
                    {
                        // Add locked checkpoint to the region
                        sublevel->at(curr_row)->at(curr_col)->initTrap(TrapModel::TrapType::CHECKPOINT,
                            tile_to_traps[data.at(j)].find("key") != string::npos);
                        // Add checkpoint to the region
                        region->addCheckpoint(dynamic_cast<Checkpoint*>(&(*(sublevel->at(curr_row)->at(curr_col)->getTrap())))->getID(),
                            curr_col, curr_row);
                        // Lock it by default
                        sublevel->at(curr_row)->at(curr_col)->setPermlocked();
                    }
                    else if (tile_to_traps[data.at(j)] == "sap")
                    {
                        // TODO: make sure this works
                        sublevel->at(curr_row)->at(curr_col)->initTrap(TrapModel::TrapType::SAP);
                    }
                    else if (tile_to_traps[data.at(j)] == "locked")
                    {
                        // TODO: make sure this works
                        sublevel->at(curr_row)->at(curr_col)->setPermlocked();
                    }
                    else if (tile_to_traps[data.at(j)] == "key")
                    {
                        // TODO: add key in this room (using curr_col / curr_row)
                        // createKey();

                    }
                    // ADD KEY ENEMY IN GAMESCENE WHERE THE OTHER ENEMY IS INSTANTIATED
                    // Gamescene line 401
                    else if (tile_to_traps[data.at(j)] == "exit")
                    {
                        // Mark the exit rooms
                        region->setExitRoom(curr_col, curr_row, _assets->get<Texture>("blocked"));
                    }
                }
            }
        }
    }

    // Update the world bounds based on this region's bounds
    _bounds = _bounds.merge(region->getBounds());
}

/**
 * Deafult init
 * @param assets: the asset manager of the game
 * @param scale: the physics scale
 * @return a grid with 3x3 rooms, each room the default
 */
bool GridModel::init(shared_ptr<AssetManager> assets, float scale)
{
    _assets = assets;
    _physics_scale = scale;

    // Get JSON for the world metadata and parse for constants
    shared_ptr<JsonValue> worldJSON = assets->get<JsonValue>("world");

    // Get room dimensions in tiles
    _roomWidth = worldJSON->get("roomWidth")->asInt();
    _roomHeight = worldJSON->get("roomHeight")->asInt();
    // The size of each tile in pixels
    _tileSize = worldJSON->get("tileSize")->asInt();

    // Get the tileset for the rooms
    _roomsTileset = assets->get<JsonValue>("tileset_rooms");

    /**************************************************************/
    // REGIONS
    /**************************************************************/
    // First give all the regions access to the backgrounds they'll need
    RegionModel::setBackgrounds(assets, worldJSON);

    // Get the list of regions in the world
    vector<shared_ptr<JsonValue>> regions = worldJSON->get("regions")->children();

    // Initialize Rect for region bounds
    Rect _regionBounds = Rect();

    // Now build each region based on the corresponding JSON metadata
    for (shared_ptr<JsonValue> regionMetadata : regions) {
        initRegion(regionMetadata);
    }

    // Set the size and origin based on the full world bounds
    _size.x = _bounds.getMaxX() - _bounds.getMinX();
    _size.y = _bounds.getMaxY() - _bounds.getMinY();
    _originX = _bounds.getMinX();
    _originY = _bounds.getMinY();

    // Set only first region to be active at start
    _activeRegions->push_back(_regions->at(0));

    // Fill any empty spaces with solid rooms
    for (int y = 0; y < _size.y; y++) {
        for (int x = 0; x < _size.x; x++) {
            // If there's no room there, put a solid one
            if (getRoom(x, y) == nullptr) {
                // Need to offset by grid origin to get GRID coordinates
                shared_ptr<RoomModel> room = RoomModel::alloc(x + _originX, y + _originY, "room_solid");
                setRoom(x, y, room);
                _filler->push_back(room);
            }
        }
    }

    return this->scene2::SceneNode::init();
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

/**
 * Returns the pointer to the room located in the xth column
 * from the left and the yth row from the bottom in HOUSE space.
 *
 * If the coordinate is out of bounds, then a null pointer is
 * returned.
 *
 * @param x The column from the left that the desired room is located in
 * @param y The row from the bottom that the desired room is located in
 * @return  The room located at the given coordinates
 */
shared_ptr<RoomModel> GridModel::getRoom(int x, int y)
{
    // Transform these HOUSE coordinates to GRID space
    x += _originX;
    y += _originY;
    int region = getRegion(x, y);
    return (region == 0) ? nullptr : _regions->at(region - 1)->getRoom(x, y);
};

/**
 * Helper function that returns the number of the region that the given
 * GRID space coordinates are located within.
 *
 * @param x     An x-coordinate in GRID space
 * @param y     A y-coordinate in GRID space
 * @return      The region # (1-3) that these coordinates are in, or 0 if not found
 */
int GridModel::getRegion(int x, int y)
{
    for (int k = 0; k < _regions->size(); k++)
    {
        if (_regions->at(k)->isInRegion(x, y))
            return (k + 1);
    }
    return 0;
}

#pragma mark Setters

/**
 * Sets the given room to be located in the xth column from the
 * left and the yth row from the bottom in HOUSE space.
 *
 * Returns whether or not the room was set successfully.
 *
 * @param x     The column from the left that the desired room is located in (GRID space)
 * @param y     The row from the bottom that the desired room is located in (GRID space)
 * @param room  The room to be placed at the given coordinates
 * @return      Whether the room was set successfully
 */
bool GridModel::setRoom(int x, int y, shared_ptr<RoomModel> room)
{
    // Transform these HOUSE coordinates to GRID space
    x += _originX;
    y += _originY;
    int regionNum = getRegion(x, y);
    return (regionNum == 0) ? false : _regions->at(regionNum - 1)->setRoom(x, y, room);
};

/**
 * Swaps the two rooms at the given coordinates, where the x-coordinate is
 * the column from the left and the y-coordinate is the row from the bottom.
 *
 * Returns true if the swap was successfully performed.
 *
 * @param pos1  The coordinates of the first room to swap in (column, row) form
 * @param pos2  The coordinates of the second room to swap in (column, row) form
 * @return      Whether the swap was performed successfully
 */
bool GridModel::swapRooms(Vec2 pos1, Vec2 pos2, bool forced)
{
    // Don't swap if not allowed
    if (!forced && !canSwap(pos1, pos2))
        return false;

    shared_ptr<RoomModel> room1 = getRoom(pos1);
    shared_ptr<RoomModel> room2 = getRoom(pos2);
    Vec2 gridOrigin = Vec2(_originX, _originY);

    // Store Room2 at Room1's old location
    setRoom(pos1, room2);
    // Update Room2's location accordingly
    room2->setPosition(pos1 + gridOrigin, !forced);
    // Store Room1 at Room2's old location
    setRoom(pos2, room1);
    // Update Room1's location accordingly
    room1->setPosition(pos2 + gridOrigin, !forced);

    // For each obstacle in room1
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr =
        getPhysicsGeometryAt(pos1.y, pos1.x)->begin();
        itr != getPhysicsGeometryAt(pos1.y, pos1.x)->end(); ++itr)
    {

        // convert the position from physics to world space
        Vec2 newPos = (*itr)->getPosition() * _physics_scale;
        // Now world space to grid node space
        newPos = this->worldToNodeCoords(newPos);
        // offset the obstacles in grid space
        newPos.x -= (pos1.x - pos2.x) * DEFAULT_ROOM_WIDTH;
        newPos.y -= (pos1.y - pos2.y) * DEFAULT_ROOM_HEIGHT;
        // convert back to physics space
        newPos = this->nodeToWorldCoords(newPos) / _physics_scale;
        (*itr)->setPosition(newPos);
    }

    // For each obstacle in room2
    for (vector<shared_ptr<physics2::PolygonObstacle>>::iterator itr =
        getPhysicsGeometryAt(pos2.y, pos2.x)->begin();
        itr != getPhysicsGeometryAt(pos2.y, pos2.x)->end(); ++itr)
    {

        // convert the position from physics to grid space
        Vec2 newPos = (*itr)->getPosition() * _physics_scale;
        newPos = this->worldToNodeCoords(newPos);
        // offset the obstacles in grid space
        newPos.x += (pos1.x - pos2.x) * DEFAULT_ROOM_WIDTH;
        newPos.y += (pos1.y - pos2.y) * DEFAULT_ROOM_HEIGHT;
        // convert back to physics space
        newPos = this->nodeToWorldCoords(newPos) / _physics_scale;
        (*itr)->setPosition(newPos);
    }

    // swap the vectors of obstacles in _physicsGeometry
    // create a temp holder
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> tempPhysics =
        getPhysicsGeometryAt(pos1.y, pos1.x);
    setPhysicsGeometryAt(pos1.y, pos1.x, getPhysicsGeometryAt(pos2.y, pos2.x));
    setPhysicsGeometryAt(pos2.y, pos2.x, tempPhysics);

    return true;
};

/**
 *
 * Returns whether the two rooms with the given HOUSE space coordinates can be swapped.
 *
 * Rooms can only be swapped if they are both in-bounds and within the same region
 * and sublevel.
 *
 * @param room1 The coordinates of the first room to swap in (column, row) form
 * @param room2 The coordinates of the second room to swap in (column, row) form
 * @return      Whether the rooms can be swappedd
 */
bool GridModel::canSwap(Vec2 room1, Vec2 room2)
{
    // Transform from HOUSE to GRID space
    room1.x += _originX;
    room1.y += _originY;
    room2.x += _originX;
    room2.y += _originY;

    // Can't swap if in different regions or in different sublevels within the same region
    for (auto itr = _regions->begin(); itr != _regions->end(); ++itr) {
        if ((*itr)->areInSameSublevel(room1.x, room1.y, room2.x, room2.y))
            return true;
    }
    return false;
};

/**
 *
 * Flattens _physicsGeometry, adds the bounds, and returns this vector
 *
 * @return a flat version of _physicsGeometry, with the level bounds included
 */
shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> GridModel::getPhysicsObjects()
{
    // flat vector to store all room obstacles
    shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>> obstacles = make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>();

    calculatePhysicsGeometry();

    // For each room in the grid
    for (int col = 0; col < _size.x; col++)
    {
        for (int row = 0; row < _size.y; row++)
        {

            // For each polygon in the room
            for (auto itr = getPhysicsGeometryAt(row, col)->begin();
                itr != getPhysicsGeometryAt(row, col)->end(); ++itr)
            {
                // add the obstacle to the flat vector
                obstacles->push_back(*itr);
            }
        }
    }

    // MAKE BOUNDS OF LEVEL

    // Get room dimensions scaled by grid scale
    Vec2 roomscale = Vec2(DEFAULT_ROOM_WIDTH * getScaleX(), DEFAULT_ROOM_HEIGHT * getScaleY());
    // Create path for level bounds
    Path2 path = Path2(Rect(_originX * roomscale.x, _originY * roomscale.y,
        _size.x * roomscale.x, _size.y * roomscale.y));
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

Poly2 GridModel::convertToScreen(Poly2 poly)
{
    vector<Vec2> verts;
    for (Vec2 v : poly.getVertices())
    {
        verts.push_back(this->scene2::SceneNode::nodeToWorldCoords(v));
    }
    return Poly2(verts);
};

void GridModel::calculatePhysicsGeometry()
{
    _physicsGeometry = make_shared<vector<shared_ptr<vector<shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>>>>>>();

    // Trap cache
    shared_ptr<TrapModel> trap;
    // Room cache
    shared_ptr<RoomModel> room;

    int fillerInd = 0;
    // For each room in the world
    for (int row = 0; row < _size.y; row++)
    {
        _physicsGeometry->push_back(make_shared<vector<shared_ptr<vector<shared_ptr<physics2::PolygonObstacle>>>>>());
        
        for (int col = 0; col < _size.x; col++)
        {
            _physicsGeometry->at(row)->push_back(make_shared<vector<shared_ptr<physics2::PolygonObstacle>>>());

            room = getRoom(col, row);

            // If there's no room here, pull the corresponding solid one from the fillers
            // This loop happens in the same order that the fillers were created, so we can just pop off
            if (room == nullptr) {
                room = _filler->at(fillerInd);
                fillerInd++;
            }

            // Get pointers to PolygonNodes with the room's geometry
            shared_ptr<vector<shared_ptr<scene2::PolygonNode>>> geometry = room->getGeometry();

            // For each polygon in the room
            for (vector<shared_ptr<scene2::PolygonNode>>::iterator itr = geometry->begin(); itr != geometry->end(); ++itr)
            {
                // Copy polygon data
                Poly2 poly = (*itr)->getPolygon();
                // Get node to world transformation and apply to the polygon
                poly *= (*itr)->getNodeToWorldTransform();
                // Scale to physics space
                poly /= _physics_scale;

                // Create physics obstacle
                shared_ptr<physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(poly, Vec2::ZERO);
                obstacle->setBodyType(b2_staticBody);

                getPhysicsGeometryAt(row, col)->push_back(obstacle);
            }

            // TODO: Inspect code for bug
            // TODO: Why does this have no impact on instantiation
            // if the room has a trap
            trap = room->getTrap();
            if (trap)
            {
                shared_ptr<scene2::PolygonNode> pn = trap->getPolyNode();
                Poly2 p = pn->getPolygon();
                p *= pn->getNodeToWorldTransform();
                p /= _physics_scale;

                // Create physics obstacle
                shared_ptr<physics2::PolygonObstacle> obstacle = physics2::PolygonObstacle::alloc(p, Vec2::ZERO);
                obstacle->setBodyType(b2_staticBody);

                getPhysicsGeometryAt(row, col)->push_back(obstacle);
                trap->initObstacle(obstacle);
                if (trap->getType() == TrapModel::TrapType::TRAPDOOR)
                {
                    trap->getObstacle()->setSensor(true);
                }
                if (trap->getType() == TrapModel::TrapType::CHECKPOINT)
                {
                    trap->getObstacle()->setSensor(true);
                }
            }
        }
    }

    // Also do exit blockade geometry
    Poly2 blockPoly;
    shared_ptr<physics2::PolygonObstacle> obstacle;
    shared_ptr<vector<shared_ptr<RoomModel>>> exitRooms;
    int counter = 0;
    // For each region
    for (auto regItr = _regions->begin(); regItr != _regions->end(); ++regItr) {
        exitRooms = (*regItr)->getExitRooms();
        // Don't do this region if exit rooms == nullptr, meaning the region has been cleared
        if (exitRooms == nullptr) continue;
        // For each blockade in that region
        for (auto blockItr = (*regItr)->getBlockades()->begin();
            blockItr != (*regItr)->getBlockades()->end(); ++blockItr) {
            blockPoly = (*blockItr)->getPolygon();
            blockPoly *= (*blockItr)->getNodeToWorldTransform();
            blockPoly /= _physics_scale;

            // Create physics obstacle
            obstacle = physics2::PolygonObstacle::alloc(blockPoly, Vec2::ZERO);
            obstacle->setBodyType(b2_staticBody);

            // Counter will let rooms/blockades align because they were added simultaneously
            getPhysicsGeometryAt(exitRooms->at(counter)->getPositionX() / DEFAULT_ROOM_WIDTH,
                exitRooms->at(counter)->getPositionY() / DEFAULT_ROOM_HEIGHT)->push_back(obstacle);
            (*regItr)->addBlockadeObs(obstacle);

            counter++;
        }
        counter = 0;
    }
}

#pragma mark Checkpoints
/**
 * Clears all the rooms associated with the checkpoint with the given ID (backgrounds
 * are swapped to the "cleared" option for the associated region). If this was the last
 * checkpoint in the region, it then also clears the region blockades.
 *
 * Returns 0 if the checkpoint was not found, 1 if it cleared but the region is
 * not fully cleared yet, and 2 if it cleared and it was the last checkpoint so
 * so the region was also cleared.
 *
 * @param cID	The unique ID number for a specific checkpoint
 * @return      An integer from 0-2 representing the outcome of the attempt
 */
int GridModel::clearCheckpoint(int cID)
{
    int outcome = -1;

    for (auto regItr = _regions->begin(); regItr != _regions->end(); ++regItr)
    {
        outcome = (*regItr)->clearCheckpoint(cID);

        // Succeed if checkpoint was found and cleared
        if (outcome == 1) {
            // Checkpoint cleared
            return 1;
        }
        // If the region is now clear, clear it accordingly
        else if (outcome == 2) {
            (*regItr)->clearRegion();
            // Checkpoint and region cleared
            return 2;
        }
    }
<<<<<<< HEAD
    return false;
}
=======

    // Checkpoint not found
    return 0;
}
>>>>>>> main
