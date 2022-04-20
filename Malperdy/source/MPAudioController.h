
#ifndef MP_AUDIO_H
#define MP_AUDIO_H

#include <cugl/cugl.h>
#include <string>

using namespace cugl;

class MPAudioController {

public:

	static void playAudio(std::shared_ptr<cugl::AssetManager>& assets, string sound, bool loop, float vol, bool isMusic);

	static bool isPlaying(string sound);

	static void stopAudio(std::shared_ptr<cugl::AssetManager>& assets, string sound, bool isMusic, float fadeAmount);

	static void setVolume(string sound, bool isMusic, float vol, bool isRel);
};

#endif 