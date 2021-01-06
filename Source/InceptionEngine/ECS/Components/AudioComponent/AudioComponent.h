#pragma once
#include "EngineGlobals/EngineApiPrefix.h"

namespace inceptionengine
{

	class IE_API AudioComponent
	{
	public:
		~AudioComponent() = default;

		void PlaySound2D(std::string const& soundFilePath, bool loop = false);

		static void InitializeSoundEngine();

		static void Drop();

	private:


	};

}