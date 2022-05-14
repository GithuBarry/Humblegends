//
//  MPLoadingScene.h
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
// 
//  Owner: Jordan Selin
//  Contributors: Barry Wang, Jordan Selin
//  Version: 2/21/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#ifndef __MP_LOADING_SCENE_H__
#define __MP_LOADING_SCENE_H__
#include <cugl/cugl.h>


/**
 * This class is a simple loading screen for asychronous asset loading.
 *
 * The screen will display a very minimal progress bar that displays the
 * status of the asset manager.  Make sure that all asychronous load requests
 * are issued BEFORE calling update for the first time, or else this screen
 * will think that asset loading is complete.
 *
 * Once asset loading is completed, it will display a play button.  Clicking
 * this button will inform the application root to switch to the gameplay mode.
 */
class LoadingScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // NO CONTROLLER (ALL IN SEPARATE THREAD)
    
    // VIEW
    /** The animated progress bar */
    std::shared_ptr<cugl::scene2::ProgressBar>  _bar;
    /** The game's name (to be replaced with the team logo) */
    std::shared_ptr<cugl::scene2::SceneNode>  _brand;

    /** The game's name */
    std::shared_ptr<cugl::scene2::SceneNode>  _title;
    /** The "play" button */
    std::shared_ptr<cugl::scene2::Button>    _button;
    /** The "load game" button */
    std::shared_ptr<cugl::scene2::SceneNode>   _load;
    /** The "new game" button */
    std::shared_ptr<cugl::scene2::SceneNode>    _new;

    // MODEL
    /** The progress displayed on the screen */
    float _progress;
    /** Whether or not the player has pressed play to continue */
    bool  _completed;
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
        
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    LoadingScene() : cugl::Scene2(), _progress(0.0f) {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LoadingScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    /**
     * Returns true if loading is complete, but the player has not pressed play
     *
     * @return true if loading is complete, but the player has not pressed play
     */
    bool isPending( ) const;
};


#endif /* __MP_LOADING_SCENE_H__ */
