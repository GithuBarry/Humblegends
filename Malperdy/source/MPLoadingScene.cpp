//
//  MPLoadingScene.cpp
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
#include "MPLoadingScene.h"

using namespace cugl;

/** This is the ideal size of the logo */
#define SCENE_SIZE  1024

#pragma mark -
#pragma mark Constructors

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
bool LoadingScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE/dimen.width;
    } else {
        dimen *= SCENE_SIZE/dimen.height;
    }
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/loading.json");
    auto layer = assets->get<scene2::SceneNode>("load");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _bar = std::dynamic_pointer_cast<scene2::ProgressBar>(assets->get<scene2::SceneNode>("load_bar"));
    _brand = assets->get<scene2::SceneNode>("load_logo");
    _title = assets->get<scene2::SceneNode>("load_title");
    if (saveFileExists()) {
        _load = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_load-withSave"));
        _load->addListener([=](const std::string& name, bool down) {
            this->_mode = 2;
            this->_active = down;
        });
    }else {
        _load = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_load-noSave"));
    }
    _new = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_new"));
    _new->addListener([=](const std::string& name, bool down) {
        this->_mode = 1;
        this->_active = down;
    });
    _settings = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_settings"));
    _settings->addListener([=](const std::string& name, bool down) {
        showSettings();
    });
    _credits = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("load_credits"));
    _credits->addListener([=](const std::string& name, bool down) {
        showCredits();
    });
    Application::get()->setClearColor(Color4::BLACK);
    addChild(layer);

    // Play title screen music
    AudioController::playGivenMusic(_assets->get<Sound>("titlescreen_music"));

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LoadingScene::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _new->deactivate();
        _load->deactivate();
    }
    _new = nullptr;
    _load = nullptr;
    _settings = nullptr;
    _credits = nullptr;
    _title = nullptr;
    _brand = nullptr;
    _bar = nullptr;
    _assets = nullptr;
    _progress = 0.0f;
}


#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void LoadingScene::update(float progress) {
    if (_progress < 1) {
        _progress = _assets->progress();
        if (_progress >= 1) {
            showMainMenu();
        }
        _bar->setProgress(_progress);
    }
}

/**
 * Returns true if loading is complete, but the player has not started the game
 *
 * @return true if loading is complete, but the player has not started the game
 */
bool LoadingScene::isPending( ) const {
    return _new != nullptr && _new->isVisible();
}

#pragma mark -
#pragma mark Helper Functions

/*
* Returns whether there is a save file to load
*
* @return whether there is a save file to load
*/
bool LoadingScene::saveFileExists() {
    vector<std::string> file_path_list = vector<std::string>(2);
    file_path_list[0] = Application::get()->getSaveDirectory();
    file_path_list[1] = "state.json";
    return filetool::file_exists(cugl::filetool::join_path(file_path_list));
}

/* Hides all assets so it's safe to switch screens */
void LoadingScene::hideAll(int helper) {
    _bar->setVisible(false);
    _brand->setVisible(false);

    _title->setVisible(false);
    _load->setVisible(false);
    if (saveFileExists()) _load->deactivate();
    _new->setVisible(false);
    _new->deactivate();
    _settings->setVisible(false);
    if (helper != 1) _settings->deactivate();
    _credits->setVisible(false);
    if (helper != 2) _credits->deactivate();
}

/* Switches to main menu screen */
void LoadingScene::showMainMenu() {
    hideAll();
    _title->setVisible(true);
    _load->setVisible(true);
    if (saveFileExists()) _load->activate();
    _new->setVisible(true);
    _new->activate();
    _settings->setVisible(true);
    _settings->activate();
    _credits->setVisible(true);
    _credits->activate();
}

/* Switches to settings screen */
void LoadingScene::showSettings() {
    //hideAll(1);
}

/* Switches to credits screen */
void LoadingScene::showCredits() {
    //hideAll(2);
}