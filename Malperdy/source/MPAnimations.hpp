//
//  MPAnimations.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 4/19/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPAnimations_hpp
#define MPAnimations_hpp

#include <stdio.h>
#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include <map>

using namespace cugl;


/** Class representing an animation */
class Animations{
public:
    // The sprite sheet
    shared_ptr<Texture> _frames;
    
    // Frame data
    int _size;
    int _cols;
    int _rows;
    bool _loop = false;
    
    // Empty constructor, must initialize to usse
    Animations(){};
    
    // Sets all attributes
    bool init(shared_ptr<Texture> frames, int size, int cols, string loop);
};



#endif /* MPAnimations_hpp */
