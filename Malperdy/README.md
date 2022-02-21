# Rocket Demo

This demo should be familar to students who have taken 3152.  It is the first part of the physics lab from that class, rewritten in CUGL.  We will have the ragdoll demo later.

Once again, you control the game differently on desktop and mobile.  On desktop, you control the ship entirely with the arrow keys.  On mobile, you tap the screen to get the rear thrusters to fire.  However, the left and right thrusters are controlled by the accelerometer.

Unlike the other demos, this demo is not so great for different aspect ratios.  It is specifically designed for a phone resolution.  You will see a lot of empty space at the top on more square displays like a tablet. While we have tools to rearrange scene graphs to fit different screens, physics bodies are a little more fragile.  If you would like to make a more robust (for arbitrary aspect ratios) version of this demo, you are welcome to do so.
