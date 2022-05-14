//
//  MPInput.cpp
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//  Additional reference was from the CS 4152 Geometry Lab by Walker White, 2022
//
//  Owner: Jordan Selin
//  Contributors: Jordan Selin, Spencer Hurst, Barry Wang
//  Version: 3/07/2022
//
//  Copyright (c) 2022 Humblegends. All rights reserved.
//
#include "MPInput.h"
#include <cmath>

using namespace cugl;

#pragma mark Input Constants

/** How fast a double click must be in milliseconds */
#define EVENT_DOUBLE_CLICK  400
/** How far we must move for a drag */
#define EVENT_DRAG_LENGTH  100
/** How far we must swipe left, right, up or down for a gesture */
#define EVENT_SWIPE_LENGTH  200
/** How fast we must swipe left or right for a gesture (in milliseconds) */
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
#define ZOOM_IN_KEY  KeyCode::E
/** The key to zoom out */
#define ZOOM_OUT_KEY  KeyCode::Q
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
        _isScrolling(false),
        _scrollOffset(cugl::Vec2::ZERO),
        _currDrag(false),
        _prevDrag(false),

//Mouse-Specific Support
        _mouseDown(false),
        _mouseKey(0),
        _mouseDragging(false),

//Touchscreen-Specific Support
        _touchKey(0),
        _touchDown(false),
        _currentTouch(0),
        _touchDragging(false),
        _touchTime(0.0),

        _multiKey(0),
        _inMulti(false),
        _pinchGesture(false),
        _zoomGesture(false),
        _panGesture(false),
        _panCurr(cugl::Vec2::ZERO),
        _panPrev(cugl::Vec2::ZERO),

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
        _zoomOutPressed(false) {
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
        mouse->addDragListener(_mouseKey, [=](const cugl::MouseEvent& event, const Vec2 previous, bool focus) {
            this->mouseDragCB(event, previous, focus);
            });
        mouse->addReleaseListener(_mouseKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
            this->mouseUpCB(event, clicks, focus);
            });
    }
    else success = false;

#else
    // Otherwise process touchscreen & multitouch gestures
    success = Input::activate<Touchscreen>() && Input::activate<CoreGesture>();
    Touchscreen *screen = Input::get<Touchscreen>();
    CoreGesture *multitouch = Input::get<CoreGesture>();

    _touchKey = screen->acquireKey();
    screen->addBeginListener(_touchKey, [=](const cugl::TouchEvent &event, bool focus) {
        this->touchBeginCB(event, focus);
    });
    screen->addMotionListener(_touchKey, [=](const cugl::TouchEvent &event, const Vec2 previous, bool focus) {
        this->touchMotionCB(event, previous, focus);
    });
    screen->addEndListener(_touchKey, [=](const cugl::TouchEvent &event, bool focus) {
        this->touchEndCB(event, focus);
    });

    _multiKey = multitouch->acquireKey();
    multitouch->addBeginListener(_multiKey, [=](const cugl::CoreGestureEvent &event, bool focus) {
        this->multiBeginCB(event, focus);
    });
    multitouch->addChangeListener(_multiKey, [=](const cugl::CoreGestureEvent &event, bool focus) {
        this->multiChangeCB(event, focus);
    });
    multitouch->addEndListener(_multiKey, [=](const cugl::CoreGestureEvent &event, bool focus) {
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
        mouse->removeDragListener(_mouseKey);
        mouse->removeReleaseListener(_mouseKey);
        mouse->setPointerAwareness(Mouse::PointerAwareness::BUTTON);
        Input::deactivate<Keyboard>();
        Input::deactivate<Mouse>();
#else
        Input::deactivate<Touchscreen>();
        Input::deactivate<CoreGesture>();
        Touchscreen *screen = Input::get<Touchscreen>();
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
    _prevDrag = _currDrag;

#ifndef CU_TOUCH_SCREEN
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);

    // Reynard Specific Code:
    _dDown = keys->keyPressed(DASH_RIGHT_KEY);
    _aDown = keys->keyPressed(DASH_LEFT_KEY);
    _eDown = keys->keyPressed(ZOOM_IN_KEY);
    _qDown = keys->keyPressed(ZOOM_OUT_KEY);

    _spaceDown = keys->keyPressed(JUMP_KEY);

    _currDown = _mouseDown;
    _currPos = _mousePos;

    _currDrag = _mouseDragging;
    _dragStart = _mouseDragStart;
    _dragEnd = _mouseDragEnd;

    keys->keyDown(KeyCode::ARROW_DOWN);

    // USE INTERNAL PRIVATE VARIABLES TO CHANGE THE EXTERNAL FLAGS
    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed = _keyExit;

    _jumpPressed = _spaceDown;
    _dashRightPressed = _dDown;
    _dashLeftPressed = _aDown;
    _zoomInPressed = _eDown;
    _zoomOutPressed = _qDown;

#else
    _currDown = _touchDown && !_inMulti;
    _currPos = _touchPos;

    _zoomOutPressed = _pinchGesture;
    _zoomInPressed = _zoomGesture;

    _isScrolling = _panGesture;
    _scrollOffset = _panCurr - _panPrev;
    _panPrev = _panCurr;

    _currDrag = _touchDragging;
    _dragStart = _touchStartPos;
    _dragEnd = _touchEndPos;

    if (_currDown && _prevDown) {
        _touchTime += dt;
    }
    bool couldBeSwipe = didRelease() && _touchTime <= EVENT_SWIPE_TIME;
    float xDist = (_currPos - _touchStartPos).x;
    float yDist = (_currPos - _touchStartPos).y;
    bool vertical = abs(yDist) > abs(xDist);
    _dashLeftPressed = couldBeSwipe && xDist <= -EVENT_SWIPE_LENGTH && !vertical;
    _dashRightPressed = couldBeSwipe && xDist >= EVENT_SWIPE_LENGTH && !vertical;
    _jumpPressed = couldBeSwipe && abs(xDist) < EVENT_SWIPE_LENGTH; //Release to jump
    //_jumpPressed = couldBeSwipe && yDist <= -5 && vertical; //SWIPE UP TO JUMP

#endif

// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyDebug = false;
    _keyReset = false;
    _keyDebug = false;
#endif
}

/* Clears any buffered inputs so that we may start fresh. */
void InputController::clear() {
    //TODO: update this
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed = false;

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
void InputController::mouseDownCB(const cugl::MouseEvent &event, Uint8 clicks, bool focus) {
    if (!_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = true;
        _mousePos = event.position;
        _mouseDragStart = event.position;
    }
}

/**
* Callback to execute when a mouse button is dragged.
* A drag is mouse motion while a mouse key is pressed
*
* This function will record a drag only if the left button is pressed.
*
* @param event     The event with the mouse information
* @param previous  The previous position of the mouse (UNUSED)
* @param focus     Whether this device has focus (UNUSED)
*/
void InputController::mouseDragCB(const cugl::MouseEvent& event, const cugl::Vec2 previous, bool focus){
    if (event.buttons.hasLeft()) {
        float dist = std::abs((event.position - _mouseDragStart).length());
        _mouseDragging = dist >= EVENT_DRAG_LENGTH;
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
void InputController::mouseUpCB(const cugl::MouseEvent &event, Uint8 clicks, bool focus) {
    if (_mouseDown && event.buttons.hasLeft()) {
        _mouseDown = false;
        if (_mouseDragging) {
            _mouseDragEnd = event.position;
            _mouseDragging = false;
        }
    }
}

#pragma mark Touch Callbacks

/**
 * Callback to execute when a new touch begins.
 *
 * @param event     The event with the touch information
 */
void InputController::touchBeginCB(const cugl::TouchEvent &event, bool focus) {
    if (!_touchDown) {
        _touchDown = true;
        _currentTouch = event.touch;
        _touchPos = event.position;
        _touchStartPos = event.position;
        _touchTime = 0;
    }
}

/*
* Callback to execute when a touch shifts location.
*
* @param event     The event with the touch information
*/
void InputController::touchMotionCB(const cugl::TouchEvent &event, const cugl::Vec2 previous, bool focus) {
    if (_touchDown && event.touch == _currentTouch) {
        _touchPos = event.position;

        float dist = std::abs((event.position - _touchStartPos).length());
        _touchDragging = dist >= EVENT_DRAG_LENGTH;
    }
}

/**
 * Callback to execute when a touch ends.
 *
 * @param event     The event with the touch information
 */
void InputController::touchEndCB(const cugl::TouchEvent &event, bool focus) {
    if (_touchDown && event.touch == _currentTouch) {
        _touchDown = false;
        if (_touchDragging) {
            _touchEndPos = event.position;
            _touchDragging = false;
        }
    }
}

/*
* Callback to execute when two fingers are detected on the device.
*
* @param event  The touch event for this gesture
* @param focus  Whether the listener currently has focus (UNUSED)
*/
void InputController::multiBeginCB(const cugl::CoreGestureEvent &event, bool focus) {
    _inMulti = true;
}

/*
* Callback to execute when the gesture is updated.
*
* @param event  The touch event for this gesture
* @param focus  Whether the listener currently has focus (UNUSED)
*/
void InputController::multiChangeCB(const cugl::CoreGestureEvent &event, bool focus) {
    if (event.type == CoreGestureType::PINCH) {
        _panGesture = false;
        _panPrev = Vec2::ZERO;
        _panCurr = Vec2::ZERO;
        float spreadDiff = event.currSpread - event.origSpread;
        _pinchGesture = spreadDiff < -EVENT_SPREAD_LENGTH;
        _zoomGesture = spreadDiff > EVENT_SPREAD_LENGTH;
    }
    else if (event.type == CoreGestureType::PAN) {
        Vec2 scrollVec = event.currPosition - event.origPosition;
        _panGesture = true;
        if (_panGesture) {
            _panCurr = event.currPosition;
        }
    } else {
        _panGesture = false;
        _panPrev = Vec2::ZERO;
        _panCurr = Vec2::ZERO;
        _pinchGesture = false;
        _zoomGesture = false;
    }
}

/*
* Callback to execute when there are no longer two fingers on the device.
* This could mean that either that fingers were removed or fingers were added.
*
* @param event  The touch event for this gesture
* @param focus  Whether the listener currently has focus (UNUSED)
*/
void InputController::multiEndCB(const cugl::CoreGestureEvent &event, bool focus) {
    _pinchGesture = false;
    _zoomGesture = false;
    _panGesture = false;
    _panPrev = Vec2::ZERO;
    _panCurr = Vec2::ZERO;
    _inMulti = false;
}
