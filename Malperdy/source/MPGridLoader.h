//
//  MPGridLoader.h
//  Malperdy
//
//  Created by Evan Azari on 2/27/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPGridLoader_h
#define MPGridLoader_h

#include <cugl/cugl.h>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace cugl;

class GridLoader {
private:
    shared_ptr<map<string, shared_ptr<vector<shared_ptr<JsonValue>>>>> lookup;
    
public:
#pragma mark Constructors
    /**
     * Initializes a loader that will be used to read in grid formats from
     * a JSON and can be queried to get room types for each coord.
     *
     * @param path  The path to the JSON file containing the room geometries
     * @return      Whether the loader initialization was successful
     */
    bool init(const string path);
    
    
    /**
     * Returns a newly-allocated loader that will be used to read in grid formats from
     * a JSON and can be queried to get room types for each coord.
     *
     * @param path  The path to the JSON file containing the room geometries
     * @return      A newly-allocated GridLoader
     */
    static std::shared_ptr<GridLoader> alloc(const string path) {
        std::shared_ptr<GridLoader> result = std::make_shared<GridLoader>();
        return (result->init(path) ? result : nullptr);
    }
    
    /**
     * Returns a pointer to the array of JsonValue pointers representing the
     * roomIDs
     *
     *
     * @return  Pointer to vector of roomIDs
     */
    shared_ptr<vector<shared_ptr<JsonValue>>> getRoomData() { return lookup->at("rooms"); }
    
    /**
     * Returns a pointer of JsonValue representing the num rows
     *
     *
     * @return  Pointer to JsonValue of num rowss
     */
    shared_ptr<JsonValue> getRows() { return (lookup->at("rows"))->at(0); }
    
    /**
     * Returns a pointer of JsonValue representing the num cols
     *
     *
     * @return  Pointer to JsonValue of num cols
     */
    shared_ptr<JsonValue> getCols() { return (lookup->at("cols"))->at(0); }
    
};


#endif /* MPGridLoader_h */
