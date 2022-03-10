//
//  MPGridLoader.cpp
//  Malperdy
//
//  Owner: Evan Azari
//  Contributors: Evan Azari
//  Version: 2/27/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPGridLoader.h"

/**
 * Loads all room geometries from the JSON file at the given path, then
 * constructs a lookup table that a RoomModel can reference for creating
 * its own geometry.
 *
 * @return      Whether all rooms were loaded successfully from the JSON
 */
bool GridLoader::init(const string path) {
    // Initialize array to store level data in
    _level = make_shared<vector<shared_ptr<vector<string>>>>();

    // Initialize JSON reader
    std::shared_ptr<JsonReader> reader = JsonReader::alloc(path);
    // Read and get JSON file
    std::shared_ptr<JsonValue> json = reader->readJson();

    // Read dimensions of level from JSON
    _dims = Vec2(json->getInt("cols"), json->getInt("rows"));
    
    // Get all rooms in the level
    shared_ptr<JsonValue> rooms = json->get("rooms");

    // For each room in the JSON, store the room's ID in a grid where it can be accessed via its coordinates
    for (int row = 0; row < _dims.y; row++) {
        // Get IDs for row of rooms and store for later reference
        _level->push_back(make_shared<vector<string>>((rooms->get(row))->asStringArray()));
    }

    // Return that loading of rooms was successful
    return true;
}

