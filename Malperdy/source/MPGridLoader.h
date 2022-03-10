//
//  MPGridLoader.h
//  Malperdy
//
//  Owner: Evan Azari
//  Contributors: Evan Azari
//  Version: 2/27/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
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
    /** Array that stores IDs of rooms that form the level */
    shared_ptr<vector<shared_ptr<vector<string>>>> _level;
    /** Dimensions of the level */
    Vec2 _dims;
    
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
     * Returns the ID of the room at the given location in the level grid.
     * 
     * @param col   The column of the location to get the room ID from
     * @param row   The row of the location to get the room ID from
     * @return      ID of the room at the given location
     */
    string getRoomAt(int col, int row) { return _level->at(_dims.y - row - 1)->at(col); }

    /**
     * Returns the dimensions of the level as a vector, where the x-value is
     * the number of columns and the y-value is the number of rows.
     * 
     * @return  Dimensions of the level
     */
    Vec2 getDims() { return _dims; }
    
};


#endif /* MPGridLoader_h */
