//
//  MPCheckpoint.h
//  Malperdy
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin
//  Version: 5/6/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPCheckpoint_h
#define MPCheckpoint_h

#include <cugl/cugl.h>
#include <stdlib.h>
#include "MPTrapModel.hpp"
#include "MPRoomModel.h"

using namespace cugl;

class Checkpoint : public TrapModel {
private:
    /** All rooms linked to this checkpoint, so hitting the checkpoint clears them */
    shared_ptr<vector<shared_ptr<RoomModel>>> _rooms = make_shared<vector<shared_ptr<RoomModel>>>();

    /** The region this checkpoint is in, as an integer from 1-3 */
    int _region;

    /** Static reference to the cleared background textures */
    static shared_ptr<vector<shared_ptr<Texture>>> _bgsCleared;

    /** Whether the level associated with this checkpoint has already been cleared */
    bool _cleared = false;

public:

#pragma mark Constructors
    /*
     * Initializes a new checkpoint for a level in the given region.
     *
     * @param roomWidth     The width of the room the checkpoint is in
     * @param roomHeight    The height of the room the checkpoint is in
     * @param isFinal       Whether this is a "final" checkpoint, meaning a goal
     * @param region        An integer from 1-3 indicating which region this checkpoint is in
     * @return  true if the trap is initialized properly, false otherwise.
     */
    bool init(float roomWidth, float roomHeight, bool isFinal, int region);

    /**
     * Links the given room to this checkpoint.
     * 
     * @param room  The room to link to this checkpoint
     */
    void linkRoom(shared_ptr<RoomModel> room) {
        _rooms->push_back(room);
    }

    /**
     * Stores a static reference to all the cleared background options so that every
     * checkpoint can access them.
     * 
     * @param cleared   All the cleared backgrounds for the regions, indexed by region
     */
    static void setClearedBackgrounds(shared_ptr<vector<shared_ptr<Texture>>> cleared) {
        _bgsCleared = cleared;
    }

    /**
     * Clears all the rooms associated with this checkpoint (backgrounds are swapped to
     * the "cleared" option for the associated region).
     *
     * @return  Whether the checkpoint's associated rooms were cleared successfully
     */
    bool clearCheckpoint();
};
#endif /* MPCheckpoint_h */