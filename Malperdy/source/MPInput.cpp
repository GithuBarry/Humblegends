//
//  MPInput.cpp
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Humblegends
//  Contributors: Spencer Hurst, Jordan Selin
//  Version: 3/01/2022
//
#include "MPInput.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::P
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE

/** The key for exitting the game */
#define DASH_RIGHT_KEY  KeyCode::D
/** The key for exitting the game */
#define DASH_LEFT_KEY  KeyCode::A
/** The key for exitting the game */
#define ZOOM_IN_KEY  KeyCode::Q
/** The key for exitting the game */
#define ZOOM_OUT_KEY  KeyCode::E


/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How far we must swipe left or right for a gesture */
#define EVENT_SWIPE_LENGTH  100
/** How fast we must swipe left or right for a gesture */
#define EVENT_SWIPE_TIME   1000
/** How far we must turn the tablet for the accelerometer to register */
#define EVENT_ACCEL_THRESH  M_PI/10.0f
/** The key for the event handlers */
#define LISTENER_KEY        1


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
_keyDown(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),

//Touch Support
_currDown(false),
_prevDown(false),
_mouseDown(false),
_mouseKey(0),

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
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(_mouseKey);
        mouse->removeReleaseListener(_mouseKey); 
        mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
#else
        Input::deactivate<Accelerometer>();
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
}

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @return true if the controller was initialized successfully
 */
bool InputController::init() {
    _timestamp.mark();
    bool success = true;
    
    // Only process keyboard and mouse on desktop
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
        _mouseKey = mouse->acquireKey();
        mouse->addPressListener(_mouseKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonDownCB(event, clicks, focus);
            });
        mouse->addReleaseListener(_mouseKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->buttonUpCB(event, clicks, focus);
        });
    }
    else success = false;
#else
    success = Input::activate<Accelerometer>();
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const cugl::TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
#endif
    _active = success;
    return success;
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
//    TODO: This code serves as a reference for the //Mobile controls section
//    TODO: Please delete this section post second Sprint.
    
//    int left = false;
//    int rght = false;
//    int up   = false;
//    int down = false;
    
    int space = false;

    _prevDown = _currDown;

#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
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

    space = keys->keyDown(KeyCode::SPACE);

    _currDown = _mouseDown;
    _currPos = _mousePos;
    
//    TODO: This code serves as a reference for the //Mobile controls section
//    TODO: Please delete this section post second Sprint.
//    left = keys->keyDown(KeyCode::ARROW_LEFT);
//    rght = keys->keyDown(KeyCode::ARROW_RIGHT);
//    up   = keys->keyDown(KeyCode::ARROW_UP);
//    down = keys->keyDown(KeyCode::ARROW_DOWN);
    
#else
    // MOBILE CONTROLS
//    TODO: This code is to be rewitten next sprint to allow for mobile controls
//    Vec3 acc = Input::get<Accelerometer>()->getAcceleration();
//
//    // Measure the "steering wheel" tilt of the device
//    float pitch = atan2(-acc.x, sqrt(acc.y*acc.y + acc.z*acc.z));
//
//    // Check if we turned left or right
//    left |= (pitch > EVENT_ACCEL_THRESH);
//    rght |= (pitch < -EVENT_ACCEL_THRESH);
//    up   |= _keyUp;
#endif

    // USE INTERNAL PRIVATE VARIABLES TO CHANGE THE EXTERNAL FLAGS
    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    
    _jumpPressed = space;
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

/**
 * Clears any buffered inputs so that we may start fresh.
 * This is primarily handled by setting all flags to false
 */
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
void InputController::buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    // Only recognize the left mouse button
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
void InputController::buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    // Only recognize the left mouse button
    if (_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = false;
    }
}

#pragma mark Touch Callbacks

///**
// * Callback for the beginning of a touch event
// *
// * @param t     The touch information
// * @param event The associated event
// */

//void InputController::touchBeganCB(const cugl::TouchEvent& event, bool focus) {
//    // All touches correspond to key up
//    _keyUp = true;
//
//    // Update the touch location for later gestures
//    _timestamp = event.timestamp;
//    _dtouch = event.position;
//}
 
///**
// * Callback for the end of a touch event
// *
// * @param t     The touch information
// * @param event The associated event
// */

//void InputController::touchEndedCB(const cugl::TouchEvent& event, bool focus) {
//    // Gesture has ended.  Give it meaning.
//    cugl::Vec2 diff = event.position-_dtouch;
//    bool fast = (event.timestamp.ellapsedMillis(_timestamp) < EVENT_SWIPE_TIME);
//    _keyReset = fast && diff.x < -EVENT_SWIPE_LENGTH;
//    _keyExit  = fast && diff.x > EVENT_SWIPE_LENGTH;
//    _keyDebug = fast && diff.y > EVENT_SWIPE_LENGTH;
//    _keyUp = false;
//}

bool InputController::didEndSwipe() { 
    //TODO
    return false;
}

std::vector<cugl::Vec2> InputController::getSwipeStartEnd(){
    //TODO
    return std::vector<cugl::Vec2>();
}

