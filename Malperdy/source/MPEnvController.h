//
//  MPEnvController.h
//  Malperdy
//
//  Initialize and update grid
//  (room swapping, physics objects coordinate updates), interactions
//
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPEnvController_h
#define MPEnvController_h

#include <cugl/cugl.h>

class EnvController: scene2::SceneNode {
    Vec2 getGridcoordOfRoom(RoomModel room);

    RoomModel getRoom(int row, int col);

    /**
     * Given global coordinate (when zoomed out), find if it is inside an area of a room
     * @param globalCoordinate
     * @return
     */
    RoomModel getRoom(Vec2 globalCoordinate);

    /**
     * Return two vec2, former indicates left bottom corner GLOBAL coordinate,
     * latter indicates upper right when zoomed out
     */
    Vec2[] getGlobalcoordOfRoom(RoomModel room);
    Vec2[] getGlobalcoordOfRoom(int row, int col);

    /**
     * Change room grid coordinate, and update the physics world objects (obstacles) in rooms
     * with correct, new Global physics coordinates
     * @param room1
     * @param room2
     */
    void swapRoom(RoomModel room1, RoomModel room2);

    cugl::physics2::Obstacle getAllObstacles();
};

#endif /* MPEnvController_h */
