//
//  MPSpikeTrap.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//
//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

#ifndef MPSpikeTrap_hpp
#define MPSpikeTrap_hpp

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#include "MPTrapModel.hpp"
#include "MPRoomModel.h"

#include <stdio.h>
using namespace cugl;

#pragma mark -
#pragma mark SpikeTrap


class SpikeTrap : public TrapModel{

public:

#pragma mark Constructor

    /**
     * Initializes a new character at the given position.
     *
     * The character is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos       Initial position in world coordinates
     * @param drawScale The drawing scale (world to screen)
     * @param image     The image for the character's appearance
     *
     * @return  true if the character is initialized properly, false otherwise.
     */
    bool init(){
        // make the polygon for the spike trap
        float bounds[] = { 0,0,1,0};
        Path2 p = Path2(reinterpret_cast<Vec2*>(bounds), size(bounds)/2);
        p.closed =  true;
        SimpleExtruder se = SimpleExtruder();
        se.clear();
        se.set(p);
        se.calculate(0.1);
        Poly2 poly = se.getPolygon();
        
        // TODO: replace this with actual room size!
        poly.operator*=(Vec2(720,480));

        // Ensure that all points are integers
        vector<Vec2> verts = poly.getVertices();
        for (vector<Vec2>::iterator itr = verts.begin(); itr != verts.end(); ++itr) {
            (*itr).x = floor((*itr).x);
            (*itr).y = floor((*itr).y);
        }

        return this->TrapModel::init(poly);
    };



};


#endif /* MPSpikeTrap_hpp */
