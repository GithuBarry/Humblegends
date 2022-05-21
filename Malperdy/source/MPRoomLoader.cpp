//
//  MPRoomLoader.cpp
//  Malperdy
//
//  This class loads in all of the room geometries from a JSON file,
//  storing them in a lookup table of JsonValues. The loader can then
//  be queried to get the JsonValues corresponding to a room's inner
//  geometry.
// 
//  This class should never be used except by RoomModel, which constructs
//  a single static instance to be used by all RoomModels.
//	
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 2/26/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#include "MPRoomLoader.h"

/**
 * Loads all room geometries from the JSON file at the given path, then
 * constructs a lookup table that a RoomModel can reference for creating
 * its own geometry.
 *
 * @return      Whether all rooms were loaded successfully from the JSON
 */
bool RoomLoader::init(const string path) {
    // Initialize lookup table
    lookup = make_shared<map<string, shared_ptr<vector<shared_ptr<JsonValue>>>>>();

    // Initialize JSON reader
    std::shared_ptr<JsonReader> reader = JsonReader::alloc(path);
    // Read and get JSON file
    std::shared_ptr<JsonValue> json = reader->readJson();

    // Initialize JsonValue ptr cache to hold pointer to room data
    shared_ptr<JsonValue> room;

    // For each room in the JSON, store its geometry in the map with its name as the key
    for (int ii = 0; ii < json->size(); ii++) {
        // Get JSON object for the array of room data
        room = json->get(ii);
        // Initialize vector of polygon coordinates for this room
        shared_ptr<vector<shared_ptr<JsonValue>>> roomGeometry = make_shared<vector<shared_ptr<JsonValue>>>();

        // For each piece of geometry within that room
        for (int k = 0; k < room->size(); k++) {
            // Store JsonValue for each polygon's coordinates in vector of polygons for the room
            roomGeometry->push_back(room->get(k));
        }

        // Add the data for this room to the map, with the key being the room ID
        lookup->emplace(room->key(), roomGeometry);
    }

    // Return that loading of rooms was successful
    return true;
}