//
//  MPInput.cpp
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//  Additional reference was from the CS 4152 Geometry Lab by Walker White, 2022
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Spencer Hurst
//  Version: 3/07/2022
// 
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#include "MPInput.h"

using namespace cugl;

#pragma mark Input Constants

/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How far we must swipe left or right for a gesture */
#define EVENT_SWIPE_LENGTH  100
/** How fast we must swipe left or right for a gesture */
#define EVENT_SWIPE_TIME   1000
/** How far we must pinch or zoom for a gesture */
#define EVENT_SPREAD_LENGTH  100
/** The key for the event handlers */
#define LISTENER_KEY        1

/* Below are the keys used for each action when running the game on desktop */

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::P
/** The key for exiting the game */
#define EXIT_KEY  KeyCode::ESCAPE

/** The key to dash right */
#define DASH_RIGHT_KEY  KeyCode::D
/** The key to dash left */
#define DASH_LEFT_KEY  KeyCode::A
/** The key to zoom in */
#define ZOOM_IN_KEY  KeyCode::Q
/** The key to zoom out */
#define ZOOM_OUT_KEY  KeyCode::E
/** The key to jump */
#define JUMP_KEY  KeyCode::W

#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_keyUp(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),

//General Touch Support
_currDown(false),
_prevDown(false),

//Mouse-Specific Support
_mouseDown(false),
_mouseKey(0),

//Touchscreen-Specific Support
_touchKey(0),
_touchDown(false),
_currentTouch(0),
_multiKey(0),
_isPinching(false),
_isZooming(false),

//Reynard Direct Presses
_spaceDown(false),
_qDown(false),
_eDown(false),
_aDown(false),
_dDown(false),
//Reynard Results/Outputs instantiated
_jumpPressed(false),
_dashRightPressed(false),
_dashLeftPressed(false),
_zoomInPressed(false),
_zoomOutPressed(false)
{
}

/**
* Initializes the control to support mouse and keyboard.
* Later, will also support touch and gesture on a mobile device.
*
* This method attaches all of the listeners. It tests which
* platform we are on (mobile or desktop) to pick the right
* listeners.
*
* This method will fail (return false) if the listeners cannot
* be registered or if there is a second attempt to initialize
* this controller
*
* @return true if the initialization was successful
*/
bool InputController::init() {
    //TODO: return false on a second attempt to initialize the controller
    _timestamp.mark();
    bool success = true;
    
#ifndef CU_TOUCH_SCREEN
    // Only process keyboard and mouse on desktop
    success = Input::activate<Keyboard>() && Input::activate<Mouse>();;
    Mouse* mouse = Input::get<Mouse>();
    Keyboard* keyboard = Input::get<Keyboard>();
    if (mouse && keyboard) {
        // Set listeners for mouse inputs
        mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
        _mouseKey = mouse->acquireKey();
        mouse->addPressListener(_mouseKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->mouseDownCB(event, clicks, focus);
            });
        mouse->addReleaseListener(_mouseKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->mouseUpCB(event, clicks, focus);
            });
    }
    else success = false;

#else
    // Otherwise process touchscreen & multitouch gestures
    success = Input::activate<Touchscreen>() && Input::activate<CoreGesture>();
    Touchscreen* screen = Input::get<Touchscreen>();
    CoreGesture* multitouch = Input::get<CoreGesture>();

    _touchKey = screen->acquireKey();
    screen->addBeginListener(_touchKey, [=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeginCB(event, focus);
        });
    screen->addMotionListener(_touchKey, [=](const cugl::TouchEvent& event, const Vec2 previous, bool focus) {
        this->touchMotionCB(event, previous, focus);
        });
    screen->addEndListener(_touchKey, [=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndCB(event, focus);
        });

    _multiKey = multitouch->acquireKey();
    multitouch->addBeginListener(_multiKey, [=](const cugl::CoreGestureEvent& event, bool focus) {
        this->multiBeginCB(event, focus);
        });
    multitouch->addChangeListener(_multiKey, [=](const cugl::CoreGestureEvent& event, bool focus) {
        this->multiChangeCB(event, focus);
        });
    multitouch->addEndListener(_multiKey, [=](const cugl::CoreGestureEvent& event, bool focus) {
        this->multiEndCB(event, focus);
        });

#endif
    _active = success;
    return success;
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(_mouseKey);
        mouse->removeReleaseListener(_mouseKey); 
        mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
        Input::deactivate<Keyboard>();
        Input::deactivate<Mouse>();
#else
        Input::deactivate<Touchscreen>();
        Input::deactivate<CoreGesture>();
        Touchscreen* screen = Input::get<Touchscreen>();
        screen->removeBeginListener(_touchKey);
        screen->removeEndListener(_touchKey);
        screen->removeMotionListener(_touchKey);
#endif
        _active = false;
    }
}

/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void InputController::update(float dt) {

    _prevDown = _currDown;

#ifndef CU_TOUCH_SCREEN
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
    
    // Reynard Specific Code:
    _dDown = keys->keyPressed(DASH_RIGHT_KEY);
    _aDown = keys->keyPressed(DASH_LEFT_KEY);
    _qDown = keys->keyPressed(ZOOM_IN_KEY);
    _eDown = keys->keyPressed(ZOOM_OUT_KEY);

    _spaceDown = keys->keyPressed(JUMP_KEY);

    _currDown = _mouseDown;
    _currPos = _mousePos;

    keys->keyDown(KeyCode::ARROW_DOWN);
    
#else
    _currDown = _touchDown;
    _currPos = _touchPos;

    _zoomOutPressed = _isPinching;
    _zoomInPressed = _isZooming;

#endif

    // USE INTERNAL PRIVATE VARIABLES TO CHANGE THE EXTERNAL FLAGS
    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    
    _jumpPressed = _spaceDown;
    _dashRightPressed = _dDown;
    _dashLeftPressed = _aDown;
    _zoomInPressed = _qDown;
    _zoomOutPressed = _eDown;

// If it does not support keyboard, we must reset "virtual" keyboard
//    TODO: ADD TO THIS WHEN DOING SPRINT 2 (Might cause bugs without it)
#ifdef CU_TOUCH_SCREEN
    _keyDebug = false;
    _keyReset = false;
    _keyDebug = false;
#endif
}

/* Clears any buffered inputs so that we may start fresh. */
void InputController::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
    
    _jumpPressed = false;
    _dashRightPressed = false;
    _dashLeftPressed = false;
    _zoomInPressed = false;
    _zoomOutPressed = false;
    
    _dtouch = Vec2::ZERO;
    _timestamp.mark();
}

#pragma mark -
#pragma mark Mouse Callbacks
/**
 * Call back to execute when a mouse button is first pressed.
 *
 * This function will record a press only if the left button is pressed.
 *
 * @param event     The event with the mouse information
 * @param clicks    The number of clicks (for double clicking) (UNUSED)
 * @param focus     Whether this device has focus (UNUSED)
 */
void InputController::mouseDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    if (!_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = true;
        _mousePos = event.position;
    }
}

/**
 * Call back to execute when a mouse button is first released.
 *
 * This function will record a release for the left mouse button.
 *
 * @param event     The event with the mouse information
 * @param clicks    The number of clicks (for double clicking) (UNUSED)
 * @param focus     Whether this device has focus (UNUSED)
 */
void InputController::mouseUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    if (_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = false;
    }
}

#pragma mark Touch Callbacks

/**
 * Callback to execute when a new touch begins.
 *
 * @param event     The event with the touch information
 */
void InputController::touchBeginCB(const cugl::TouchEvent& event, bool focus) {
    CULog("Touch Begin");
    if (!_touchDown) {
        _touchDown = true;
        _currentTouch = event.touch;
        _touchPos = event.position;
    }
}

/*
* Callback to execute when a touch shifts location.
*
* @param event     The event with the touch information
*/
void InputController::touchMotionCB(const cugl::TouchEvent& event, const cugl::Vec2 previous, bool focus) {
    CULog("Touch Move");
    if (_touchDown && event.touch == _currentTouch) {
        _touchPos = event.position;
    }
}
 
/**
 * Callback to execute when a touch ends.
 *
 * @param event     The event with the touch information
 */
void InputController::touchEndCB(const cugl::TouchEvent& event, bool focus) {
    CULog("Touch End");
    if (_touchDown && event.touch == _currentTouch) {
        _touchDown = false;
    }
}

/*
* Callback to execute when two fingers are detected on the device.
*
* @param event  The touch event for this gesture
* @param focus  Whether the listener currently has focus (UNUSED)
*/
void InputController::multiBeginCB(const cugl::CoreGestureEvent& event, bool focus) {
    //TODO: implement
}

/*
* Callback to execute when the gesture is updated.
*
* @param event  The touch event for this gesture
* @param focus  Whether the listener currently has focus (UNUSED)
*/
void InputController::multiChangeCB(const cugl::CoreGestureEvent& event, bool focus) {
    //TODO: implement
    if (event.type == CoreGestureType::PINCH) {
        CULog("Pinch gesture type");
        float spreadDiff = event.currSpread - event.origSpread;
        _isPinching = spreadDiff < -EVENT_SPREAD_LENGTH;
        _isZooming = spreadDiff > EVENT_SPREAD_LENGTH;
        if (_isPinching)CULog("Pinch detected");
        if (_isZooming)CULog("Zoom detected");
    }
    else {
        _isPinching = false;
        _isZooming = false;
    }
}

/*
* Callback to execute when there are no longer two fingers on the device.
* This could mean that either that fingers were removed or fingers were added.
*
* @param event  The touch event for this gesture
* @param focus  Whether the listener currently has focus (UNUSED)
*/
void InputController::multiEndCB(const cugl::CoreGestureEvent& event, bool focus) {
    _isPinching = false;
    _isZooming = false;
}