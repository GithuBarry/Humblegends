//
//  MPGridLoader.cpp
//  Malperdy
//
//  Created by Evan Azari on 2/27/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
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
    // Initialize lookup table
    lookup = make_shared<map<string, shared_ptr<vector<shared_ptr<JsonValue>>>>>();

    // Initialize JSON reader
    std::shared_ptr<JsonReader> reader = JsonReader::alloc(path);
    // Read and get JSON file
    std::shared_ptr<JsonValue> json = reader->readJson();

    // Initialize JsonValue ptr cache to hold num rows
    shared_ptr<JsonValue> row = json->get(0);
    shared_ptr<vector<shared_ptr<JsonValue>>> rowvec = make_shared<vector<shared_ptr<JsonValue>>>();
    rowvec->push_back(row);
    lookup->emplace(json->get(0)->key(), rowvec);
    
    // Initialize JsonValue ptr cache to hold num cols
    shared_ptr<JsonValue> col = json->get(1);
    shared_ptr<vector<shared_ptr<JsonValue>>> colvec = make_shared<vector<shared_ptr<JsonValue>>>();
    colvec->push_back(col);
    lookup->emplace(json->get(1)->key(), colvec);
    
    // Initialize JsonValue ptr cache to hold num cols
    shared_ptr<JsonValue> rooms = json->get(2);
    
    shared_ptr<vector<shared_ptr<JsonValue>>> roomvec = make_shared<vector<shared_ptr<JsonValue>>>();

    // For each room in the JSON, store its geometry in the map with its name as the key
    for (int ii = 0; ii < rooms->size(); ii++) {
        
        // Get JSON object for the array of room data
        shared_ptr<JsonValue> room_row = rooms->get(ii);
        
        for(int jj = 0; jj < room_row->size(); jj++){
            roomvec->push_back(room_row->get(jj));
        }
    }
    lookup->emplace(json->get(2)->key(), roomvec);

    // Return that loading of rooms was successful
    return true;
}

