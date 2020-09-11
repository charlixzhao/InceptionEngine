
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include <iostream>

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
		TransformSystem& mSystem;

		int mX = 0;
		int mY = 0;
	};
}
