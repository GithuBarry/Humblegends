//
//  MPRoomLoader.h
//  Malperdy
//
//  This class loads in all of the room geometries from a JSON file.
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

    bool init(const string path);

    static std::shared_ptr<RoomLoader> alloc(const string path) {
        std::shared_ptr<RoomLoader> result = std::make_shared<RoomLoader>();
        return (result->init(path) ? result : nullptr);
    }

    shared_ptr<vector<shared_ptr<JsonValue>>> getRoomData(string roomID) { return lookup->at(roomID); }
};