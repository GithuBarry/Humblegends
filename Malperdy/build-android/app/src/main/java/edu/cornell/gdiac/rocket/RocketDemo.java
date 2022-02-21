package edu.cornell.gdiac.rocket;

import org.libsdl.app.*;
import android.os.Bundle;

public class RocketDemo extends SDLActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Make sure this is before calling super.onCreate
        setTheme(R.style.CUGLTheme);
        super.onCreate(savedInstanceState);
    }
}
