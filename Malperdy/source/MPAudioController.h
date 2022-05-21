
#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <cugl/cugl.h>
#include <string>

using namespace cugl;

/* Sound */
/* Play these with MPAudioController::playAudio(_assets, XYZ_SOUND_GOES_HERE, false, 1, false); */
#define BUMP_SOUND "bump"
#define DASH_SOUND "dash"
#define JUMP_SOUND "jump"
#define LAND_SOUND "land"
#define SWAP_SOUND "swap_room"
#define NOSWAP_SOUND "noswap"
#define CHECKPOINT_SOUND "checkpoint"
#define VINE_SOUND "vine"

#define ZOOMIN_SOUND "zoom_in"
#define ZOOMOUT_SOUND "zoom_out"
#define PAUSE_UI_SOUND "pause_ui"

/* Play these with MPAudioController::playAudio(_assets, XYZ_MUSIC_GOES_HERE, false, 1, true); */
#define LEVEL_MUSIC "level_music"
#define TITLE_MUSIC "titlescreen_music"

class AudioController {
private:
	/** Store a reference to the assets for easy access */
	static std::shared_ptr<AssetManager> _assets;

	static void playAudio(string sound, bool loop, float vol, bool isMusic);

public:

	/**
	 * Initializes the audio controller by giving it a reference to the sound assets
	 * it will be using.
	 * 
	 * @param assets	Reference to the asset manager with the sounds
	 */
	static void init(std::shared_ptr<cugl::AssetManager>& assets) {
		_assets = assets;
	}

#pragma mark Playing/Stopping
	/**
	 * Plays a sound effect with the given volume. A sound effect plays once and
	 * then stops, and does not loop.
	 * 
	 * @param sound	The name of the sound to play (USE THE MACROS IN AUDIOCONTROLLER)
	 * @param vol	The relative volume at which to play the sound (1 by default)
	 */
	static void playSFX(string sound, float vol=1.0f) {
		playAudio(sound, false, vol, false);
	}

	/**
	 * Plays a music track with the given volume. A music track plays once and then
	 * loops.
	 *
	 * @param sound	The name of the music to play (USE THE MACROS IN AUDIOCONTROLLER)
	 * @param vol	The relative volume at which to play the music (1 by default)
	 */
	static void playMusic(string sound, float vol=1.0f) {
		playAudio(sound, true, vol, true);
	}

	/**
	 * Plays the given music track with the given volume. A music track plays once and
	 * then loops.
	 *
	 * @param sound	The music asset to play
	 * @param vol	The relative volume at which to play the music (1 by default)
	 */
	static void playGivenMusic(std::shared_ptr<Sound> sound, float vol = 1.0f) {
		AudioEngine::get()->getMusicQueue()->play(sound, true, vol);
	}

	static bool isPlaying(string sound, bool isMusic);

	static void stopAudio(string sound, bool isMusic, float fadeAmount);

	static void setVolume(string sound, bool isMusic, float vol, bool isRel);
};

#endif 
