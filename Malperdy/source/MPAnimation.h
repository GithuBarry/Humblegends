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
#include <cugl/cugl.h>

using namespace cugl;


#endif /* MPAnimation_h */

class Animation{
    
private:
#pragma mark Attributes
    
    shared_ptr<Texture> _spritesheet;
    
    int _cols;
    int _rows;
    int _size;
    bool _reversed;
    
    // animation name : (start frame, end frame, loop, flip)
    map<string, tuple<int, int, bool, bool>> _frames;
    
public:
#pragma mark Constructors
    
    Animation(shared_ptr<Texture> sheet, shared_ptr<JsonValue> framedata){
        _spritesheet = sheet;
        
        _cols = framedata->get("cols")->asInt();
        _rows = framedata->get("rows")->asInt();
        _size = framedata->get("size")->asInt();
        _reversed = framedata->get("reversed")->asString() == "true";
        
        int numAnims =  framedata->get("frames")->size();
        for(int i = 0; i< numAnims; i++){
            shared_ptr<JsonValue> ob = framedata->get("frames")->get(i);
            string key = ob->key();
            int start = ob->get("start")->asInt();
            int last = ob->get("last")->asInt();
            bool loop = ob->get("loop")->asString() == "true";
            bool flip = ob->get("flip")->asString() == "true";
            
            _frames[key] = tuple<int, int, bool, bool>(start, last, loop, flip);
        }
    };

#pragma mark Accessors
    
    shared_ptr<Texture> getSheet(){
        return _spritesheet;
    }
    
    int getRows(){ return _rows;}
    
    int getCols(){ return _cols;}
    
    int getSize(){ return _size;}
    
    bool hasKey(string key){
        return _frames.count(key) > 0;
    }
    
    int getStart(string key){
        return std::get<0>(_frames[key]);
    }
    
    int getLast(string key){
        return std::get<1>(_frames[key]);
    }
    
    bool isLoop(string key){
        return std::get<2>(_frames[key]);
    }
    
    bool isFlip(string key){
        return std::get<3>(_frames[key]);
    }
    
    bool isReversed(){
        return _reversed;
    }
    
    
};
