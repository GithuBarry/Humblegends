//
//  MPInput.h
//  Malperdy
//
//  This file is based on the CS 4152 RocketDemo by Walker White, 2017
//  That was based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//  Additional reference was from the CS 4152 Geometry Labby Walker White, 2022
//
//  Author: Humblegends
//  Contributors: Spencer Hurst, Jordan Selin
//  Version: 3/01/2022
//
// Later Edited by Spencer Hurst on 2/25/22.
// Copyright (c) 2022 Humblegends. All rights reserved.
//
#ifndef __MP_INPUT_H__
#define __MP_INPUT_H__
#include <cugl/cugl.h>

/**
 * This class represents player input in the rocket demo.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this
 * controller as a field without using pointers. We simply add the class to the
 * header file of its owner, and delay initialization (via the method init())
 * until later. This is one of the main reasons we like to avoid initialization
 * in the constructor.
 */
class InputController {
private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the up arrow key is down */
    bool  _keyUp;
    /** Whether the down arrow key is down */
    bool  _keyDown;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    
    // KEYBOAD EMULATION FOR REYNARD:
    /** Whether the Spacebar is down */
    bool  _spaceDown;
    /** Whether the q  key is down */
    bool  _qDown;
    /** Whether the e key is down */
    bool  _eDown;
    /** Whether the a key is down */
    bool  _aDown;
    /** Whether the c key is down */
    bool  _dDown;

    // GENERAL TOUCH SUPPORT
    /** Whether there is an active button/touch press */
    bool _currDown;
    /** Whether there was an active button/touch press last frame*/
    bool _prevDown;
    /** The current touch/mouse position */
    cugl::Vec2 _currPos;

    // MOUSE SUPPORT
    /** Whether the (left) mouse button is down */
    bool _mouseDown;
    /** The mouse position (for mice-based interfaces) */
    cugl::Vec2 _mousePos;
    /** The key for the mouse listeners */
    Uint32 _mouseKey;

    /** The initial touch location for the current gesture */
    cugl::Vec2 _dtouch;
    /** The timestamp for the beginning of the current gesture */
    cugl::Timestamp _timestamp;

protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    
    // INPUT RESULTS SPECIFICALLY FOR REYNARD
    /** Whether the jump action was chosen. */
    bool _jumpPressed;
    /** Whether the dash right action was chosen. */
    bool _dashRightPressed;
    /** Whether the dash left action was chosen. */
    bool _dashLeftPressed;
    /** Whether the zoom in toggle was chosen. */
    bool _zoomInPressed;
    /** Whether the zoom out toggle was chosen. */
    bool _zoomOutPressed;



    
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
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    bool init();
    
#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

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
    void  update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark -
#pragma mark Input Results
    
    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
    bool didReset() const { return _resetPressed; }
    
    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didDebug() const { return _debugPressed; }
    
    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didZoomIn() const { return _zoomInPressed; }

    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didZoomOut() const { return _zoomOutPressed; }
    
    /**
     * Returns true if the exit button was pressed.
     *
     * @return true if the exit button was pressed.
     */
    bool didExit() const { return _exitPressed; }
    
    /**
     * Returns true if the dash RIGHT button was pressed.
     *
     * @return true if the dash button was pressed.
     */
    bool didDashRight() const { return _dashRightPressed; }
    
    /**
     * Returns true if the dash LEFT button was pressed.
     *
     * @return true if the dash button was pressed.
     */
    bool didDashLeft() const { return _dashLeftPressed; }
    
    /**
     * Returns true if the jump button was pressed.
     *
     * @return true if the jump button was pressed.
     */
    bool didJump() const { return _jumpPressed; }

    /**
     * Return true if the user initiated a press this frame.
     *
     * A press means that the user is pressing (button/finger) this
     * animation frame, but was not pressing during the last frame.
     *
     * @return true if the user initiated a press this frame.
     */
    bool didPress() const {
        return !_prevDown && _currDown;
    }

    /**
     * Returns the current mouse/touch position
     *
     * @return the current mouse/touch position
     */
    const cugl::Vec2& getPosition() const {
        return _currPos;
    }
    
    /**
     * @return Whether a swipe just ended
     */
    bool didEndSwipe();

    /**
     * @return the start and the end global coordinates of a swipe (mobile and mouse)
     * in form of [start_pos, end_pos]
     * return null when didEndSwipe() is false
     */
    std::vector<cugl::Vec2> getSwipeStartEnd();
    
    
#pragma mark -

#pragma mark Mouse Callbacks
private:
    /**
     * Call back to execute when a mouse button is first pressed.
     *
     * This function will record a press only if the left button is pressed.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void buttonDownCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

    /**
     * Call back to execute when a mouse button is first released.
     *
     * This function will record a release for the left mouse button.
     *
     * @param event     The event with the mouse information
     * @param clicks    The number of clicks (for double clicking)
     * @param focus     Whether this device has focus (UNUSED)
     */
    void buttonUpCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);

#pragma mark Touch Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);


};

#endif /* __MP_INPUT_H__ */
