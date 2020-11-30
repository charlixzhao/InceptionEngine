
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

		void SetWorldTransform(Matrix4x4f const& t) { mWorldTransform = t; }

	private:
		friend class TransformSystem;

		Matrix4x4f mWorldTransform = Matrix4x4f(1.0f);
	};
}
