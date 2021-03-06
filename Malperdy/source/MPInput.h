//
//  MPInput.h
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
#ifndef __MP_INPUT_H__
#define __MP_INPUT_H__

#include <cugl/cugl.h>

/* This class represents player input in Malperdy. */
class InputController {
// Common fields are protected
protected:
    /** Whether or not this input is active */
    bool _active;

    // PRESS SUPPORT
    /** Whether there is an active button/touch press */
    bool _currDown;
    /** Whether there was an active button/touch press last frame */
    bool _prevDown;
    /** The current touch/mouse position */
    cugl::Vec2 _currPos;

    /* Whether a drag is currently happening */
    bool _currDrag;
    /* Whether a drag just ended */
    bool _prevDrag;
    /* The start position of the current drag, or the last drag */
    cugl::Vec2 _dragStart;
    /* The end position of the last drag */
    cugl::Vec2 _dragEnd;

    /** Whether the zoom in toggle was chosen. */
    bool _zoomInPressed;
    /** Whether the zoom out toggle was chosen. */
    bool _zoomOutPressed;

    /* Whether the user is scrolling */
    bool _isScrolling;
    /* 
     * The difference between the scroll position this frame and last frame, if currently scrolling
     * Otherwise (0,0) if there is no current scroll
     */
    cugl::Vec2 _scrollOffset;

    /** The initial touch location for the current gesture */
    cugl::Vec2 _dtouch;
    /** The timestamp for the beginning of the current gesture */
    cugl::Timestamp _timestamp;

    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    //TODO: delete duplication
    /** Whether the reset action was chosen. */
    bool _keyReset;
    /** The key for debug listeners */
    bool _keyDebug;
    /** The key for exit listeners */
    bool _keyExit;

    //REYNARD CONTROLS
    /** Whether the jump action was chosen. */
    bool _jumpPressed;
    /** Whether the dash right action was chosen. */
    bool _dashRightPressed;
    /** Whether the dash left action was chosen. */
    bool _dashLeftPressed;

    // DEBUG
    /** Whether to auto-clear the first region */
    bool _clearReg1Pressed;
    /** Whether to auto-clear the second region */
    bool _clearReg2Pressed;

// Device-specific fields are kept private
private:

    // KEYBOAD EMULATION FOR REYNARD:
    /** Whether the up arrow key is pressed */
    bool _keyUp;
    /** Whether the Spacebar is pressed */
    bool _spaceDown;
    /** Whether the q key is pressed */
    bool _qDown;
    /** Whether the e key is pressed */
    bool _eDown;
    /** Whether the a key is pressed */
    bool _aDown;
    /** Whether the c key is pressed */
    bool _dDown;

    // MOUSE SUPPORT
    /* The key for the mouse listeners */
    Uint32 _mouseKey;
    /* Whether the left mouse button is down */
    bool _mouseDown;
    /* The mouse position (for mice-based interfaces) */
    cugl::Vec2 _mousePos;

    /* Whether the mouse is currently dragging */
    bool _mouseDragging;
    /* The start position of the current drag, or the last drag */
    cugl::Vec2 _mouseDragStart;
    /* The end position of the last drag */
    cugl::Vec2 _mouseDragEnd;

    // TOUCHSCREEN SUPPORT
    /* The key for touchscreen listeners */
    Uint32 _touchKey;
    /* Whether the left mouse button is down */
    bool _touchDown;
    /* The ID of the current touch, if _touchDown is true */
    cugl::TouchID _currentTouch;
    /* The position of the current touch (for touch-based interfaces) */
    cugl::Vec2 _touchPos;
    /* Whether the touch is currently dragging */
    bool _touchDragging;
    /*  
    * The start position of the current touch
    * 
    * Used as the start of the current or last drag
    * Also used to determine if a swipe occurred
    */
    cugl::Vec2 _touchStartPos;
    /* The end position of the last drag */
    cugl::Vec2 _touchEndPos;
    /* The length (in time) of the current touch */
    float _touchTime;

    // MULTITOUCH SUPPORT
    /* The key for multitouch listeners */
    Uint32 _multiKey;
    /* 
     * Whether a multitouch gesture is being detected 
     * Used to turn off single-touch detection
     */
    bool _inMulti;
    /* Whether a pinch gesture was detected */
    bool _pinchGesture;
    /* Whether a zoom gesture was detected */
    bool _zoomGesture;
    /* Whether a scroll gesture was detected */
    bool _panGesture;
    /* The current pan position */
    cugl::Vec2 _panCurr;
    /* The previous pan position */
    cugl::Vec2 _panPrev;

public:
#pragma mark -
#pragma mark Constructors

    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    InputController(); // Don't initialize.  Allow stack based

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
    bool init();

    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() {
        dispose();
    }

    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();

#pragma mark -
#pragma mark Input Detection

    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive() const {
        return _active;
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
    void update(float dt);

    /* Clears any buffered inputs so that we may start fresh. */
    void clear();

#pragma mark -
#pragma mark Input Results

    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
    bool didReset() const {
        return _resetPressed;
    }

    /**
     * Returns true if the button to toggle debug mode was pressed.
     *
     * @return true if the button to toggle debug mode was pressed.
     */
    bool didDebug() const {
        return _debugPressed;
    }

    /**
     * Returns true if the button to zoom in was pressed.
     *
     * @return true if the button to zoom in was pressed.
     */
    bool didZoomIn() const {
        return _zoomInPressed;
    }

    /**
     * Returns true if the button to zoom out was pressed.
     *
     * @return true if the button to zoom out was pressed.
     */
    bool didZoomOut() const {
        return _zoomOutPressed;
    }

    /**
     * Returns true if user is currently scrolling.
     *
     * @return true if user is currently scrolling.
     */
    bool isScrolling() const {
        return _isScrolling;
    }

    /**
     * Returns the offset of the current scroll since the last frame, if there is a current scroll.
     * Otherwise returns zero if there is no current scroll.
     *
     * @return the offset of the current scroll since the last frame, if there is one
     *         zero if there is no current scroll
     */
    cugl::Vec2 scrollOffset() const {
        return _scrollOffset;
    }

    /**
     * Returns true if the exit button was pressed.
     *
     * @return true if the exit button was pressed.
     */
    bool didExit() const {
        return _exitPressed;
    }

    /**
     * Returns 1 if the dash right button was pressed,
     * -1 if the dash left button was pressed, or
     * 0 if dash was not pressed at all.
     */
    int getDashDirection() const {
        if (_dashRightPressed) return 1;
        else if (_dashLeftPressed) return -1;
        else return 0;
    }

    /**
     * Returns true if the jump button was pressed.
     *
     * @return true if the jump button was pressed.
     */
    bool didJump() const {
        return _jumpPressed;
    }

    /**
     * Return true if the user initiated a press this frame.
     *
     * A press means that the user is pressing (mouse/finger) this
     * animation frame, but was not pressing during the last frame.
     *
     * @return true if the user initiated a press this frame.
     */
    bool didPress() const {
        return !_prevDown && _currDown;
    }

    /**
     * Return true if the user released a press this frame.
     *
     * A release means that the user is not pressing (mouse/finger) 
     * this animation frame, but was pressing during the last frame.
     *
     * @return true if the user initiated a press this frame.
     */
    bool didRelease() const {
        return _prevDown && !_currDown;
    }

    /**
     * Returns the current mouse/touch position
     *
     * @return the current mouse/touch position
     */
    const cugl::Vec2 &getPosition() const {
        return _currPos;
    }

    /*
    * Returns true if the user is currently executing a drag
    * 
    * @return true if the user is currently executing a drag
    */
    bool isDragging() const {
        return _currDrag;
    }

    /*
    * Returns true if the user ended a drag this frame
    * 
    * @return true if the user ended a drag this frame
    */
    bool didEndDrag() const {
        return !_currDrag && _prevDrag;
    }

    /*
    * Returns the start position of the current drag or the drag that just ended
    * Should only be called if user is currently dragging or has just finished a drag
    * 
    * @return the start position of the current drag or the drag that just ended
    */
    cugl::Vec2 getDragStart() const {
        return _dragStart;
    }

    /*
    * Returns the start position of the drag that just ended
    * Should only be called if the user has just finished a drag
    *
    * @return the start position of the drag that just ended
    */
    cugl::Vec2 getDragEnd() const {
        return _dragEnd;
    }

    /**
     * Returns whether region 1 should be cleared automatically
     */
    bool didClearRegion1() const {
        return _clearReg1Pressed;
    }

    /**
     * Returns whether region 2 should be cleared automatically
     */
    bool didClearRegion2() const {
        return _clearReg2Pressed;
    }

#pragma mark -
#pragma mark Mouse Callbacks
private:
    /**
     * Callback to execute when a mouse button is first pressed.
     *
     * This function will record a press only if the left button is pressed.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking) (UNUSED)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void mouseDownCB(const cugl::MouseEvent &event, Uint8 clicks, bool focus);

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
    void mouseDragCB(const cugl::MouseEvent &event, const cugl::Vec2 previous, bool focus);

    /**
     * Callback to execute when a mouse button is first released.
     *
     * This function will record a release for the left mouse button.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking) (UNUSED)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void mouseUpCB(const cugl::MouseEvent &event, Uint8 clicks, bool focus);

#pragma mark Touch Callbacks

    /**
     * Callback to execute when a new touch begins.
     * 
     * @param event     The touch event for this press
     * @param focus     Whether the listener currently has focus (UNUSED)
     */
    void touchBeginCB(const cugl::TouchEvent &event, bool focus);

    /*
     * Callback to execute when a touch shifts location.
     *
     * @param event     The touch event for this movement
     * @param previous  The previous position of the touch
     * @param focus     Whether the listener currently has focus (UNUSED)
     */
    void touchMotionCB(const cugl::TouchEvent &event, const cugl::Vec2 previous, bool focus);

    /**
     * Callback to execute when a touch ends.
     *
     * @param event     The touch event for this release
     * @param focus     Whether the listener currently has focus (UNUSED)
     */
    void touchEndCB(const cugl::TouchEvent &event, bool focus);

    /*
    * Callback to execute when two fingers are detected on the device.
    * 
    * @param event  The touch event for this gesture
    * @param focus  Whether the listener currently has focus (UNUSED)
    */
    void multiBeginCB(const cugl::CoreGestureEvent &event, bool focus);

    /*
    * Callback to execute when the gesture is updated.
    *
    * @param event  The touch event for this gesture
    * @param focus  Whether the listener currently has focus (UNUSED)
    */
    void multiChangeCB(const cugl::CoreGestureEvent &event, bool focus);

    /*
    * Callback to execute when there are no longer two fingers on the device.
    * This could mean that either that fingers were removed or fingers were added.
    *
    * @param event  The touch event for this gesture
    * @param focus  Whether the listener currently has focus (UNUSED)
    */
    void multiEndCB(const cugl::CoreGestureEvent &event, bool focus);
};

#endif /* __MP_INPUT_H__ */
