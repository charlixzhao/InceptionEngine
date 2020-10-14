
#pragma once

#include "ECS/Systems/SystemBase.h"
#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	class TransformSystem : public SystemBase
	{
	public:
		explicit TransformSystem(ComponentsPool& componentsPool);

		Matrix4x4f GetEntityWorldTransform(EntityID id) const;
	
	private:

	};
}