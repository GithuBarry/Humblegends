//
//  MPGameStateController.h
//  Malperdy
//
//  Keep track of whether the mode is zoomed in or zoomed out.
//  Also keep track of health/other resources
//
//  Owner: TBD
//  Contributors: Barry Wang
//  Version: 2/21/22
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//

#ifndef MPGameStateController_h
#define MPGameStateController_h

#include <cugl/cugl.h>

class GameStateController {
private:
    bool _zoomed_in = true;
    int SLOW_MO_SCALAR = 5;
public:
    /**
     * Change parameter as you need
     */
    GameStateController() {
    };


    /**
     Return whether the game is currently zoomed in
     */
    bool zoomed_in() {
        return _zoomed_in;
    }

    /**
     Set the game state to be [zoomed in]
     */
    void zoom_in() {
        _zoomed_in = true;
    }

    /**
     Set the game state to be [zoomed out]
     */
    void zoom_out() {
        _zoomed_in = false;
    }

    /**
        Switch the game state
     */
    void zoom_switch() {
        _zoomed_in = !_zoomed_in;
    }

    /**
     * @param dt the actual time past
     * @return the time for the physics simulation, based on the state of the game
     */
    float getScaledDtForPhysics(float dt) {
        if (_zoomed_in) {
            return dt;
        } else {
            return dt / SLOW_MO_SCALAR;
        }
    };

    /**
     * Base on the current zoom and state, return zoom to be applied to
     * @param currentZoom current zoom value of a node
     * @return zoom to be applied for this frame
     */
    float getZoom(float currentZoom) {
        float maxZoom = 2.4;
        float minZoom = 1;
        float result;
        if (_zoomed_in && currentZoom < maxZoom) {
            result = 1.0f + 0.02f * (maxZoom - currentZoom) * (maxZoom - currentZoom);
        } else if ((!_zoomed_in) && currentZoom > minZoom) {
            result = 0.975;
        } else {
            result =  1;
        }
        return result;

    }

    /**
     *
     * @param currentTranslation Current global translation of the scrollpane.
     * @param reynardScreenPosition location of reynard to follow, on screen
     * @param scale scale between drawing world and physics world
     * @param screenSize screen size
     * @param faceRight whether reynard is facing right.
     * @return Pan to be applied to nodes
     */
    Vec2 getPan(Vec2 currentTranslation, Vec2 reynardScreenPosition, float scale, Size screenSize, bool faceRight) {
        Vec2 result;
        if (_zoomed_in) {
            result = (Vec2(screenSize.width, screenSize.height) / 2 - reynardScreenPosition);

        } else {
            result = Vec2(screenSize.width / 2 - reynardScreenPosition.x, 0);
            result.add(Vec2(0, -currentTranslation.y));
            if (currentTranslation.x+result.x>0){
                result = Vec2(-currentTranslation.x, result.y);
            }
        }
        result = result * result.length() / 3000;
        return result;
    }


};

#endif /* MPGameStateController_h */
