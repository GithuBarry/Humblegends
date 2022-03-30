//
//  MPCheckpoint.h
//  Malperdy
//
//  Owner: Kristina Gu
//  Contributors: Kristina Gu
//  Version: 3/29/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPCheckpoint_h
#define MPCheckpoint_h

#include <cugl/cugl.h>
#include <stdlib.h>

using namespace cugl;

class Checkpoint : public cugl::physics2::BoxObstacle {
public:

    /** The most recent checkpoint that Reynard hit */
    static shared_ptr<Checkpoint> activeCheckpoint;

#pragma mark Constructors
    /**
     * Creates a blank checkpoint. Should not be called; use init() or alloc() instead.
     */
    Checkpoint() {};

    /**
     * Initializes a checkpoint with the given texture.
     *
     * @return          true if the checkpoint is initialized properly, false otherwise.
     */
    bool init();

    /**
     * Returns a newly-allocated checkpoint with the given texture.
     * 
     * @return          A newly-allocated RoomModel
     */
    static std::shared_ptr<Checkpoint> alloc() {
        std::shared_ptr<Checkpoint> result = std::make_shared<Checkpoint>();
        return (result->init() ? result : nullptr);
    }

    /**
     * Called when Reynard collides with this checkpoint.
     */
    void activate();

};

#endif /* MPCheckpoint_h */
