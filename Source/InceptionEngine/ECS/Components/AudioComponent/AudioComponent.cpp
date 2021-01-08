
#include "IE_PCH.h"
#include "AudioComponent.h"

#include "PathHelper.h"
#include "ThirdPartyLibraries/irrKlang/include/irrKlang.h"




namespace inceptionengine
{
	static irrklang::ISoundEngine*& GetSoundEngine()
	{
		static irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
		return SoundEngine;
	}


	void AudioComponent::Drop()
	{
		if (GetSoundEngine() != nullptr)
		{
			GetSoundEngine()->drop();
			GetSoundEngine() = nullptr;
		}
	}

	void AudioComponent::PlaySound2D(std::string const& soundFilePath, bool loop)
	{
		GetSoundEngine()->play2D(PathHelper::GetAbsolutePath(soundFilePath).c_str(), loop);
	}

	void AudioComponent::InitializeSoundEngine()
	{
		GetSoundEngine()->play2D(PathHelper::GetAbsolutePath("StandAloneResource\\sice\\uneath.wav").c_str());
	}


}

