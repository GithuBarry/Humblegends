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

/** The class representing an animation. It holds the corresponding spritesheet as well as frame data for the spire sheet */
class Animation{
    
private:
#pragma mark Attributes
    
    // The sprite sheets
    shared_ptr<Texture> _spritesheet;
    
    // Number of cols, rows, and total frames in the sprite sheet
    int _cols;
    int _rows;
    int _size;
    
    // whether the animations in the spire sheet are forward or backwardss when reading left to right
    bool _reversed;
    
    // animation name : (start frame, end frame, loop, flip)
    map<string, tuple<int, int, bool, bool>> _frames;
    
public:
#pragma mark Constructors
    
    /** The sheet is the spritesheet for the animation. The framedata is a JSON value obtained from the framedata.json */
    Animation(shared_ptr<Texture> sheet, shared_ptr<JsonValue> framedata){
        _spritesheet = sheet;
        
        // Extract frame data
        _cols = framedata->get("cols")->asInt();
        _rows = framedata->get("rows")->asInt();
        _size = framedata->get("size")->asInt();
        _reversed = framedata->get("reversed")->asString() == "true";
        
        // For each animation specified in the frame data
        int numAnims =  framedata->get("frames")->size();
        for(int i = 0; i< numAnims; i++){
            shared_ptr<JsonValue> ob = framedata->get("frames")->get(i);
            string key = ob->key();
            
            // store the animation's start/last frames, as well as whether to loop it and if the animation is horizontally flipped
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
