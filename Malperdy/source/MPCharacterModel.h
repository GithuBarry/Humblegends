//
//  MPCharacterModel.h
//  Malperdy
//
//  Created by Barry Wang on 2/21/22.
//  Copyright © 2022 Humblegends. All rights reserved.
//

#ifndef MPCharacterModel_h
#define MPCharacterModel_h
#include <cugl/cugl.h>
/**
 * A Data holder superclass for characters.
 * All game changes should be in a controller.
 */
class CharacterModel : public cugl::physics2::BoxObstacle{
    /**
     * Apply non-game-logic changes
     * apply velocity or what
     * @param delta Time
     */
    void update(float delta);
    /** Global Coord of the Character */
    cugl::Vec2 _coord;
    /** Velocity of the Character */
    cugl::Vec2 _vel;

    /**
     * {@note by Barry:Feature request}
     * after calling this function, I should be able to call setCharaterNode and get a PolygonNode
     * @param texture
     */
    //void setTexture(Texture texture);


    /**
     * {@note by Barry: please complete documentation for every function}
     * must be called after calling setTexture
     * @return
     */
    const std::shared_ptr<cugl::scene2::PolygonNode>& getCharacterNode();
    void setCharaterNode(const std::shared_ptr<cugl::scene2::SceneNode>& node);

    //CharacterState getState();
    //bool setState(CharacterState cs);

public:
    enum class CharacterState : int{
        MOVING,
        RESTING,
        DASHING,
        TRAPPED,
        //add more when necessary
    };
};

#endif /* MPCharacterModel_h */
