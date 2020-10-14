
#pragma once

#include "ECS/Systems/SystemBase.h"
#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	class SkeletalMeshRenderSystem;

	class AnimationSystem : public SystemBase
	{
	public:
		AnimationSystem(ComponentsPool& componentsPool);

		void Update(float deltaTime);

	private:
	};
}