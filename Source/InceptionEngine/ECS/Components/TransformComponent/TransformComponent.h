
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include <iostream>
#include <functional>

namespace inceptionengine
{
	class TransformSystem;

	class IE_API TransformComponent
	{
	public:
		TransformComponent(TransformSystem& system, int x, int y);

		void Hello();

	private:
		friend class TransformSystem;
		std::reference_wrapper<TransformSystem> mSystem;

		int mX = 0;
		int mY = 0;
	};
}
