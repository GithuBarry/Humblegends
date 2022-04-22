//
//  MPAnimation.h
//  Malperdy
//
//  Created by Evan Azari on 4/22/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPAnimation_h
#define MPAnimation_h

#include "map";


#endif /* MPAnimation_h */

class Animation{
    
private:
#pragma mark Attributes
    
    shared_ptr<Texture> _spritesheet;
    
    int _cols;
    int _rows;
    int _size;
    
    map<string, (int, int)> _frames;
    
public:
#pragma mark Constructors
    
    Animation(shared_ptr<Texture> sheet, shared_ptr<JsonValue> framedata);
    
}
