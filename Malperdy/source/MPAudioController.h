
#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <cugl/cugl.h>
#include <string>

using namespace cugl;

class AudioController {
private:
	/** Store a reference to the assets for easy access */
	static std::shared_ptr<cugl::AssetManager>& _assets;

public:

	/**
	 * Initializes the audio controller by giving it a reference to the sound assets
	 * it will be using.
	 * 
	 * @param assets	Reference to the asset manager with the sounds
	 */
	static void init(std::shared_ptr<cugl::AssetManager>& assets) { _assets = assets; }

	static void playAudio(string sound, bool loop, float vol, bool isMusic);

	static bool isPlaying(string sound, bool isMusic);

	static void stopAudio(string sound, bool isMusic, float fadeAmount);

	static void setVolume(string sound, bool isMusic, float vol, bool isRel);
};

#endif 