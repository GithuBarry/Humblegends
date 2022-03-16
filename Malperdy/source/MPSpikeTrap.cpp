//
//  MPSpikeTrap.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 3/13/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.

//  Owner: Spencer Hurst
//  Contributors: Evan, Abu
//  Copyright (c) 2022 Humblegends. All rights reserved.
//  Version: 3/10/22

//

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>

#include "MPTrapModel.hpp"
#include "MPSpikeTrap.hpp"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new trap at the given position
 *
 * Currently hard coded spawn in across bottom of the room.
 *
 * @return  true if the trap is initialized properly, false otherwise.
 */
bool SpikeTrap::init(){
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

}
