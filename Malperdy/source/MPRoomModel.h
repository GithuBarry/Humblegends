//
//  MPRoomModel.h
//
//  Room Model is a data class that holds its geometry, traps
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPRoomModel_h
#define MPRoomModel_h

#include <cugl/cugl.h>

class RoomModel {
    Poly2[] fixedGeometry;


public:
    /*
     * Given an array of [array of vertices interpreted as closed polygons], construct a room.
     * Better if use static alloc like in {@class MPRocketModel}
     * Remember to check orientation/catch exception
     * Unless specified otherwise, positions are relative to its left bottom corner.
     * Rooms geometry are specified by vertices on [0, 1]^2 space,
     * You need to time vertice's values by x size and y size to get actual relative coordinates
     */
    RoomModel(Vec2[][] vertices, x_size, y_size);

    /**
     * @return all the polygons in this room, with coordinates all relative to its left bottom corner
     */
    std::vector<Poly2> getPoly();

    /**
     * Obstacles, in GLOBAL Physics coordinate. Should be set and updated by GridModel
     */
    cugl::physics2::PolygonObstacle[] getObstacles();
    void setObstacles(cugl::physics2::PolygonObstacle[]);

    /**
     * Move all obstacles's position by {@param changes}
     * @param changes is given in physic-world-scaled translation ammount.
     */
    void moveAllObstacles(Vec2 changes);

}


#endif /* MPRoomModel_h */
