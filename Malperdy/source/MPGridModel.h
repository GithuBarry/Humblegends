//
//  MPGridModel.h
//  Malperdy
//
//  GridModel system of rooms.
//  Storing all data of rooms grid
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPGridModel_h
#define MPGridModel_h

#include <cugl/cugl.h>
#include "MPRoomModel.h"

using namespace cugl;

class GridModel {
    float horizontal_gap_between_rooms;
    float vertical_gap_between_rooms;

    /**
     * Room length, in terms of pixels
     */
    float room_size_x;
    float room_size_y;

    /**
     * (global_boundary_x, global_boundary_y)
     * should be the global coordinate of the left down corner of the left down room when zoomed out
     */
    float global_boundary_x;
    float global_boundary_y;

    /**
     * How many rooms on each row/col when zoomed out
     */
    int n_rooms_on_each_row;
    int n_rooms_on_each_col;

    float physics_scale;

    /*
     * Rooms, presumably rooms[0] -> left corner room, and room[4] is room above that in a 3x3 level.
     * Implementer feel free to place them in order however you like. just document well enough for others.
     * Feel free to use shared pointer
     */
    // Vector<RoomModel> rooms;


public:
    //Constructor placeholder, presumably need all private fields (except rooms)
    // and array of [array of paths (in a room)]

    /**
     * @return all rooms, in a well-defined order
     */
    // RoomModel[] getRooms();



    int getNRoomsOnEachRow() const {
        return n_rooms_on_each_row;
    }

    int getNRoomsOnEachCol() const {
        return n_rooms_on_each_col;
    }


};

#endif /* MPGridModel_h */
