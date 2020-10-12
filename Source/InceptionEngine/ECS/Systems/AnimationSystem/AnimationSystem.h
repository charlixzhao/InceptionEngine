
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

		bool EntityPoseChange(EntityID id) const;

		std::vector<Matrix4x4f> const& GetEntityPose(EntityID id) const;

		void FinishUpdatePose(EntityID id) const;

	private:
	};
}