#pragma once
#include "EngineGlobals/EngineApiPrefix.h"

namespace inceptionengine
{

	class IE_API AudioComponent
	{
	public:
		~AudioComponent();

		void PlaySound2D(std::string const& soundFilePath, bool loop = false);

	private:


	};

}