//
//  MPAnimation.h
//  Malperdy
//
//  Created by Evan Azari on 4/22/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPAnimation_h
#define MPAnimation_h

#include <map>
#include <tuple>


#endif /* MPAnimation_h */

class Animation{
    
private:
#pragma mark Attributes
    
    shared_ptr<Texture> _spritesheet;
    
    int _cols;
    int _rows;
    int _size;
    
    map<string, tuple<int, int>> _frames;
    
public:
#pragma mark Constructors
    
    Animation(shared_ptr<Texture> sheet, shared_ptr<JsonValue> framedata){
        _spritesheet = sheet;
        
        _cols = framedata->get("cols")->asInt();
        _rows = framedata->get("rows")->asInt();
        _size = framedata->get("size")->asInt();
        
        int numAnims =  framedata->get("frames")->size();
        for(int i = 0; i< numAnims; i++){
            shared_ptr<JsonValue> ob = framedata->get("frames")->get(i);
            string key = ob->key();
            int start = ob->get(0)->asInt();
            int end = ob->get(1)->asInt();
            _frames[key] = tuple<int, int>(start, end);
        }
    };
    
};
