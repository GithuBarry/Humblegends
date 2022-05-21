#include "MPAudioController.h"
#include <cugl/cugl.h>
#include <string>

//shared_ptr<AssetManager> AudioController::_assets = make_shared<AssetManager>();

using namespace cugl;

/** The value to scale music volume by */
float AudioController::musicVol = 0.5;
/** Whether music is muted */
bool AudioController::musicMuted = false;

/** The value to scale sfx volume by */
float AudioController::sfxVol = 0.5;
/** Whether sfx is muted */
bool AudioController::sfxMuted = false;

	//static bool init() {
	//	AudioEngine::start(24);

	//	//Just making sure
	//	if (AudioEngine::get() != nullptr)
	//	{
	//		return false;
	//	}
	//	return true;
	//}

	//static void del() {
	//	AudioEngine::stop();
	//}
	shared_ptr<AssetManager> AudioController::_assets = make_shared<AssetManager>();

	void AudioController::playAudio(string sound, bool loop, float vol, bool isMusic)
	{
		std::shared_ptr<Sound> source = _assets->get<Sound>(sound);
			
		if (isMusic)
		{
			AudioEngine::get()->getMusicQueue()->play(source, loop, getMusicVol() * vol);
		}
		else
		{
			AudioEngine::get()->play(sound, source, loop, getSfxVol() * vol, true);
		}
	}

	bool AudioController::isPlaying(string sound, bool isMusic)
	{
		if (isMusic)
		{
			return (AudioEngine::get()->getMusicQueue()->current() == sound);
		}
		else
		{
			return AudioEngine::get()->isActive(sound);
		}
	}

	void AudioController::stopAudio(string sound, bool isMusic, float fadeAmount)
	{
		if (isMusic) {
			//TODO: make it not clear the queue???
			AudioEngine::get()->getMusicQueue()->clear(fadeAmount);
		}
		else
		{
			AudioEngine::get()->clear(sound, fadeAmount);
		}
	}
		
	void AudioController::setVolume(string sound, bool isMusic, float vol, bool isRel) {
		if (isMusic)
		{
			AudioEngine::get()->setVolume(sound, isRel ? (AudioEngine::get()->AudioEngine::getVolume(sound) + vol) : vol);
		}
		else
		{
			AudioEngine::get()->getMusicQueue()->setVolume(isRel ? (AudioEngine::get()->getMusicQueue()->getVolume() + vol) : vol);
		}
	}