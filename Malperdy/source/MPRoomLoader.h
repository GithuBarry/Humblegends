//
//  MPRoomLoader.h
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
//  Created by Kristina Gu on 2/26/22.
//  Copyright � 2022 Humblegends. All rights reserved.
//

#include <cugl/cugl.h>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace cugl;

class RoomLoader {
private:
    /** Lookup table for room geometries, read from the JSON */
    shared_ptr<map<string, shared_ptr<vector<shared_ptr<JsonValue>>>>> lookup;

public:
#pragma mark Constructors
    /**
     * Initializes a loader that will be used to read in room geometries from
     * a JSON and can be queried to get the geometry for a specific room type.
     * 
     * @param path  The path to the JSON file containing the room geometries
     * @return      Whether the loader initialization was successful
     */
    bool init(const string path);

    /**
     * Returns a newly-allocated loader that will be used to read in room
     * geometries from a JSON and can be queried to get the geometry for a
     * specific room type.
     *
     * @param path  The path to the JSON file containing the room geometries
     * @return      A newly-allocated RoomLoader
     */
    static std::shared_ptr<RoomLoader> alloc(const string path) {
        std::shared_ptr<RoomLoader> result = std::make_shared<RoomLoader>();
        return (result->init(path) ? result : nullptr);
    }

    /**
     * Returns a pointer to the array of JsonValue pointers representing the
     * geometry of the room with the given ID.
     *
     * Specific rooms are represented by a room ID, which is a string used in
     * the JSONs to identify distinct rooms. This is used as a key to get the
     * relevant geometry.
     * 
     * @return  Pointer to vector of polygon geometries
     */
    shared_ptr<vector<shared_ptr<JsonValue>>> getRoomData(string roomID) { return lookup->at(roomID); }
};