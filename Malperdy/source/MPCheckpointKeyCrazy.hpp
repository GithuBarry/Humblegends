//
//  MPCheckpointKeyCrazy.hpp
//  Rocket Demo (iOS)
//
//  Created by Abu Qader on 5/16/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPCheckpointKeyCrazy_hpp
#define MPCheckpointKeyCrazy_hpp

#include <stdio.h>

#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

using namespace cugl;

#pragma mark -
#pragma mark Key


class CheckpointKeyCrazy : public cugl::physics2::BoxObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(CheckpointKeyCrazy);

protected:
    /** The scene graph node for the Bullet. */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;
    bool _isPathFinding;

    /** Static variable to track the ID number across classes (must be odd) */
    static int _idCounter;

    /** A unique ID number for this key (must be odd) */
    int _keyID = -1;

public:
#pragma mark Constructors

    /**
     * Creates a degenerate Bullet object.
     *
     * This constructor does not initialize any of the Bullet values beyond
     * the defaults.  To use a Bullet, you must call init().
     */


    CheckpointKeyCrazy() : BoxObstacle() {
    }

    virtual ~CheckpointKeyCrazy(void) {
        _node = nullptr;
        dispose();
    }

    virtual bool init() override {
        return init(cugl::Vec2::ZERO, cugl::Size::ZERO);
    }

    virtual bool init(const cugl::Vec2 pos) override {
        return init(pos, cugl::Size(1, 1));
    }

    bool init(const cugl::Vec2 pos, const cugl::Size size) override;

    void setIsPathFinding(bool isPathFinding);

    bool isPathFinding();

    static std::shared_ptr<CheckpointKeyCrazy> alloc() {
        std::shared_ptr<CheckpointKeyCrazy> result = std::make_shared<CheckpointKeyCrazy>();
        return (result->init() ? result : nullptr);
    }

    static std::shared_ptr<CheckpointKeyCrazy> alloc(const cugl::Vec2 &pos) {
        std::shared_ptr<CheckpointKeyCrazy> result = std::make_shared<CheckpointKeyCrazy>();
        return (result->init(pos) ? result : nullptr);
    }

    static std::shared_ptr<CheckpointKeyCrazy> alloc(const cugl::Vec2 &pos, const cugl::Size &size) {
        std::shared_ptr<CheckpointKeyCrazy> result = std::make_shared<CheckpointKeyCrazy>();
        return (result->init(pos, size) ? result : nullptr);
    }


    void dispose();

#pragma mark Getters

    /**
     * Returns the unique ID number for this key. For possessed keys, the ID number is always ODD.
     *
     * @return  The unique odd ID number for this key
     */
    int getID() {
        return _keyID;
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
    const std::shared_ptr<cugl::scene2::SceneNode> &getSceneNode() const {
        return _node;
    }

    /**
     * Sets the scene graph node representing this Bullet.
     *
     * @param node  The scene graph node representing this Bullet, which has
     *              been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode> &node) {
        _node = node;
    }

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


    virtual void setPosition(const Vec2 value) override {
        BoxObstacle::setPosition(value);
        _node->setPosition(value * _drawScale);
    }

#pragma mark -
#pragma mark Physics Methods

    /**
    * Updates the object's physics state (NOT GAME LOGIC).
    *
    * We use this method to reset cooldowns.
    *
    * @param delta Number of seconds since last animation frame
    */
    void update(float dt) override;

    void createFixtures();

    void releaseFixtures();


};

#endif /* MPCheckpointKeyCrazy_hpp */


