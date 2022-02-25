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



    const std::shared_ptr<cugl::scene2::SceneNode>& getCharacterNode();

    void setCharaterNode(const std::shared_ptr<cugl::scene2::SceneNode>& node);
    //Should this be in the controller
    enum class CharacterState : int{
        SPAWN,
        RUNNING,
        RESTING,
        DASHING,
        JUMPING,
        TRAPPED,
        //add more when necessary
    };
    CharacterState getState();

    bool setState(CharacterState cs);
};

#endif /* MPCharacterModel_h */
