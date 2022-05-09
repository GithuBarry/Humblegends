#include "MPAudioController.h"
#include <cugl/cugl.h>
#include <string>

using namespace cugl;
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

	void MPAudioController::playAudio(std::shared_ptr<cugl::AssetManager> &assets, string sound, bool loop, float vol, bool isMusic)
	{
		std::shared_ptr<Sound> source = assets->get<Sound>(sound);
			
		if (isMusic)
		{
			AudioEngine::get()->getMusicQueue()->play(source, loop, vol);
		}
		else
		{
			AudioEngine::get()->play(sound, source, loop, vol, true);
		}
	}

	bool MPAudioController::isPlaying(string sound, bool isMusic)
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

	void MPAudioController::stopAudio(std::shared_ptr<cugl::AssetManager>& assets, string sound, bool isMusic, float fadeAmount)
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
		
	void MPAudioController::setVolume(string sound, bool isMusic, float vol, bool isRel) {
		if (isMusic)
		{
			AudioEngine::get()->setVolume(sound, isRel ? (AudioEngine::get()->AudioEngine::getVolume(sound) + vol) : vol);
		}
		else
		{
			AudioEngine::get()->getMusicQueue()->setVolume(isRel ? (AudioEngine::get()->getMusicQueue()->getVolume() + vol) : vol);
		}
	}