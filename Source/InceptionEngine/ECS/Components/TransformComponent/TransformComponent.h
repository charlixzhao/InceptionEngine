
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

namespace inceptionengine
{
	class TransformSystem;

	class IE_API TransformComponent
	{
	public:
		TransformComponent();

		Matrix4x4f GetWorldTransform() const;

	private:
		friend class TransformSystem;

		Matrix4x4f mWorldTransform = Matrix4x4f(1.0f);
	};
}
