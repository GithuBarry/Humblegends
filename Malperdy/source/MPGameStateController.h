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
    bool _zoomed_in;
    bool paused = false;
    bool super_zoom_out = false;
public:
    bool isSuperZoomOut() const {
        return super_zoom_out;
    }

private:
    int SLOW_MO_SCALAR;
    float maxZoom = 2.4;
    float minZoom = 1;
    float superMinZoom = 0.6;
    std::chrono::duration<unsigned long long> fourSeconds = std::chrono::seconds(4);
    std::chrono::time_point<std::chrono::system_clock> sinceResume = std::chrono::system_clock::now() - fourSeconds;
public:
    /**
     * Change parameter as you need
     */
    GameStateController() {
        reset();
    };

    void reset() {
        _zoomed_in = true;
        SLOW_MO_SCALAR = 7;
    }


    /**
     Return whether the game is currently zoomed in
     */
    bool zoomed_in() {
        return _zoomed_in;
    }

    /**
     Set the game state to be [zoomed in]
     */
    void zoom_in(float currentZoom) {
        if (super_zoom_out && finishedZooming(currentZoom)) {
            super_zoom_out = false;
        } else {
            _zoomed_in = true;
        }


    }

    /**
     Set the game state to be [zoomed out]
     */
    void zoom_out(float currentZoom) {
        if (!_zoomed_in && finishedZooming(currentZoom)) {
            super_zoom_out = true;
        }
        _zoomed_in = false;

    }

    /**
        Switch the game state
     */
    void zoom_switch() {
        _zoomed_in = !_zoomed_in;
    }

    void pause() {
        paused = true;
    }

    void unpause() {
        paused = false;
        sinceResume = std::chrono::system_clock::now();
    }

    void pauseSwitch() {
        if (paused) {
            unpause();
        } else {
            pause();
        }
    }

    bool isPaused() {
        return paused;
    }

    bool finishedZooming(float currentScale) {
        return abs(currentScale - maxZoom) < 0.05 || abs(currentScale - minZoom) < 0.05 || abs(currentScale - superMinZoom) < 0.05;
    }

    float secondsAfterResume() {
        std::chrono::duration<float> diff = std::chrono::system_clock::now() - sinceResume;
        return diff.count();
    }


    /**
     * @param dt the actual time past
     * @return the time for the physics simulation, based on the state of the game
     */
    float getScaledDtForPhysics(float dt) {
        if (paused) {
            return 0;
        }
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
        if (paused) {
            return 1;
        }

        float targetZoom = maxZoom;
        if (!_zoomed_in) {
            if (super_zoom_out) {
                targetZoom = superMinZoom;
            } else {
                targetZoom = minZoom;
            }
        }


        float result;
        if (currentZoom < targetZoom) {
            result = 1.0f + 0.02f * (targetZoom - currentZoom) * (targetZoom - currentZoom);
        } else if (currentZoom > targetZoom) {
            result = 0.975;
        } else {
            result = 1;
        }
        return result;

    }

    /**
     *
     * @param currentTranslation Current global translation of the scrollpane.
     * @param target location of reynard to follow, on screen
     * @param scale scale between drawing world and physics world
     * @param screenSize screen size
     * @param faceRight whether reynard is facing right.
     * @return Pan to be applied to nodes
     */
    Vec2 getPan(Vec2 currentTranslation, Vec2 target, float scale, Size screenSize, bool faceRight, Vec2 reynardVelocity) {
        if (paused) {
            return Vec2();
        }
        Vec2 result;
        int thr = 12;
        if (reynardVelocity.y < -thr) {
            target = target + Vec2(0, (reynardVelocity.y + thr) * 15);
        }

        if (_zoomed_in) {
            result = (Vec2(screenSize.width, screenSize.height) / 2 - target);
        } else {
            result = Vec2(screenSize.width, screenSize.height) / 2 - target;

            //if (currentTranslation.x + result.x > 0) {
            //    result = Vec2(-currentTranslation.x, result.y);
            //}
            /*if (currentTranslation.y + result.y > 0) {
                result = Vec2(result.x, -currentTranslation.y);
            }*/
        }
        result = result * result.length() / 3000;
        return result;
    }


};

#endif /* MPGameStateController_h */
