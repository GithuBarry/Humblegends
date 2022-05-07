//
//  MPArrow.hpp
//  RocketDemo
//
//  Created by Spencer Hurst on 5/7/22.
//  Copyright © 2022 Cornell Game Design Initiative. All rights reserved.
//

#ifndef MPArrow_hpp
#define MPArrow_hpp

#include <stdio.h>

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include <map>
#include "MPAnimation.h"

using namespace cugl;

class Arrow : public cugl::physics2::CapsuleObstacle {
    
public:
    
    /** SceneNode representing the sprite for the character */
    shared_ptr<scene2::SpriteNode> _spriteNode;
    /*How long has this arrow existed for*/
    int _time = 0;
//    /*Direction of the arrow*/
//    bool _right;
    
    
#pragma mark -
#pragma mark Hidden Constructors

    /**
     * Creates a degenerate Character object.
     *
     * This constructor does not initialize any of the character values beyond
     * the defaults. To use a CharacterModel, you must call init().
     */
    Arrow() : CapsuleObstacle() {}

#pragma mark -
#pragma mark Constructors
    

    /**
     * Initializes a new Arrow at the given position.
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
    virtual bool init(const cugl::Vec2 &pos, float drawScale, bool right);

    
#pragma mark -
#pragma mark Destructor
    virtual ~Arrow(void){
       
        //geometry is used for boxobstacles
//        _geometry = nullptr;
        _core = nullptr;
        dispose();
    }

    /**
     * Disposes all resources and assets of this Arrow
     *
     * Any assets owned by this object will be immediately released. Once
     * disposed, an Arrow may not be used until it is initialized
     * again.
     */
    void dispose();

};








#endif /* MPArrow_hpp */
