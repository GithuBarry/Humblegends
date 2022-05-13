//
//  FallingTrapV.hpp
//  Malperdy
//
//  Created by Abu Qader on 5/7/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPFallingTrap_hpp
#define MPFallingTrap_hpp

#include <stdio.h>
#include <cugl/cugl.h>
#include <cugl/physics2/CUPolygonObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include "MPReynardController.h"


#pragma mark -
class FallingTrap : public cugl::physics2::PolygonObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(FallingTrap);

protected:
    /** The scene graph node for the FallingTrap. */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    /** The node for debugging the sensor */
    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    bool fallen; 
public:
#pragma mark Constructors
    /**
     * Creates a degenerate Bullet object.
     *
     * This constructor does not initialize any of the Bullet values beyond
     * the defaults.  To use a Bullet, you must call init().
     */
    FallingTrap() : cugl::physics2::PolygonObstacle() { }
    
    virtual bool init(const Poly2& poly) { return init(poly,Vec2::ZERO); }

    bool init(const Poly2& poly, const Vec2 origin);
    
    bool initWithAnchor(const Poly2& poly, const Vec2 anchor);
    
    /**
     * Disposes all resources and assets of this Bullet
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a Bullet may not be used until it is initialized again.
     */
    void dispose();
    
    void setSprite(shared_ptr<cugl::scene2::SpriteNode> _spriteNode) {
        _node =_spriteNode;
    }
    
    
#pragma mark -
#pragma mark Static Constructors
    /**
     * Creates a new Bullet at the given position.
     *
     * The bullet is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     * @param  radius   The radius of the Bullet obstacle.
     *
     * @return  A newly allocated Bullet at the given position, with the given radius
     */
    static std::shared_ptr<FallingTrap> alloc(const cugl::Poly2& poly) {
        std::shared_ptr<FallingTrap> result = std::make_shared<FallingTrap>();
        return (result->init(poly) ? result : nullptr);
    }

    static std::shared_ptr<FallingTrap> allocWithAnchor(const cugl::Poly2& poly,  const Vec2 anchor) {
        std::shared_ptr<FallingTrap> result = std::make_shared<FallingTrap>();
        return (result->initWithAnchor(poly, anchor) ? result : nullptr);
    }


#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this Bullet.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this Bullet.
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }
    
    /**
     * Sets the scene graph node representing this Bullet.
     *
     * @param node  The scene graph node representing this Bullet, which has
     *              been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) {
        _node = node;
    }
    
    
     // currently working on getting the
    /**
     * Sets the ratio of the Bullet sprite to the physics body
     *
     * The Bullet needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @param scale The ratio of the Bullet sprite to the physics body
     */
    void setDrawScale(float scale) {
        _drawScale = scale;
    }
    
    void fall();

#pragma mark -
#pragma mark Physics Methods
    /**
    * Updates the object's physics state (NOT GAME LOGIC).
    *
    * We use this method to reset cooldowns.
    *
    * @param delta Number of seconds since last animation frame
    */
    void update(const shared_ptr<ReynardController>& reynard, float dt);
};

#endif /* FallingTrapV_hpp */
