//
//  MPLoadingScene.h
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
// 
//  Owner: Jordan Selin
//  Contributors: Barry Wang, Jordan Selin
//  Version: 5/14/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#ifndef __MP_LOADING_SCENE_H__
#define __MP_LOADING_SCENE_H__

#include <cugl/cugl.h>
#include "MPAudioController.h"

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
    
    // LOADING VIEW
    /** The animated progress bar */
    std::shared_ptr<cugl::scene2::ProgressBar>  _bar;
    /** The game's name (to be replaced with the team logo) */
    std::shared_ptr<cugl::scene2::SceneNode>  _brand;

    // MAIN MENU VIEW
    /** The game's name */
    std::shared_ptr<cugl::scene2::SceneNode>  _title;
    /** The "load game" button */
    std::shared_ptr<cugl::scene2::Button>     _load;
    /** The "new game" button */
    std::shared_ptr<cugl::scene2::Button>     _new;
    /** The "settings" button */
    std::shared_ptr<cugl::scene2::Button>     _settingsButton;
    /** The "credits" button */
    std::shared_ptr<cugl::scene2::Button>     _credits;

    // SETTINGS VIEW
    /** The "done" button */
    std::shared_ptr<cugl::scene2::Button>     _done;

    // MODEL
    /** The progress displayed on the screen */
    float _progress;
    /** Whether or not the player has pressed play to continue */
    bool  _completed;
    /**
    * State of the menu, so update can switch assets properly
    * 
    * 0 = loading
    * 1 = main menu
    * 2 = settings menu
    * 3 = credits menu
    */
    int _state = 0;
    /**
    * Button state, to avoid double detection of problematic buttons
    *
    * 0 = not yet pressed
    * 1 = pressed once and ignored
    */
    int _buttonState = 0;
    /**
    * How the user wants to start playing
    * 0: not yet chosen
    * 1: Start a new game
    * 2: Start from memory
    */
    int _mode = 0;
    
    
    
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
     * Returns true if loading is complete, but the player has not started the game
     *
     * @return true if loading is complete, but the player has not started the game
     */
    bool isPending( ) const;

    /**
    * Returns a value based on how the player wants to start playing
    * 0: not yet chosen
    * 1: Start a new game
    * 2: Start from memory
    * 
    * @return   value based on how the player wants to start playing
    */
    int getMode() {
        return _mode;
    }

private:
#pragma mark -
#pragma mark Helper Functions
    /*
    * Returns whether there is a save file to load
    *
    * @return whether there is a save file to load
    */
    bool saveFileExists();

    /* 
    * Hides all assets so it's safe to switch screens
    * 
    * @param helper     0 if proceeding as normal
    *                   1 to not deactivate settings
    *                   2 to not deactivate credits
    *                   3 to not deactivate done
    */
    void hideAll(int helper = 0);

    /* Switches to main menu screen */
    void showMainMenu();

    /* Switches to settings screen */
    void showSettings();

    /* Switches to credits screen */
    void showCredits();

};

#endif /* __MP_LOADING_SCENE_H__ */
