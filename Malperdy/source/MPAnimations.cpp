//
//  MPAnimations.cpp
//  RocketDemo
//
//  Created by Spencer Hurst on 4/19/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#include "MPAnimations.hpp"


bool Animations::init(shared_ptr<Texture> frames, int size, int cols, string loop){
    // Frame data
    _frames = frames;
    _size = size;
    _cols = cols;
    // Calculate the number of rows from size & cols
    _rows = (_size-1) / _cols + 1;
    if (loop == "true") _loop = true;
    
    // return false if spritesheet is null or the size is nonpositive
    return frames && size > 0;
}
