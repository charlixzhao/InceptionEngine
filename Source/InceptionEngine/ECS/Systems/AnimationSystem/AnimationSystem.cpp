#include "IE_PCH.h"

#include "AnimationSystem.h"
#include "ECS/Entity/EntityComponentPool.hpp"
#include "RunTime/Animation/AnimationController.h"

namespace inceptionengine
{
	AnimationSystem::AnimationSystem(ComponentsPool& componentsPool)
		:SystemBase(componentsPool)
	{

	}

	void AnimationSystem::Update(float deltaTime)
	{
		auto& view = mComponentsPool.get().GetComponentPool<AnimationComponent>()->GetComponentView();

		for (auto& component : view)
		{
			component.mPoseChange = component.mAnimController->Update(deltaTime);
		}
	}
	bool AnimationSystem::EntityPoseChange(EntityID id) const
	{
		return mComponentsPool.get().EntityHasComponent<AnimationComponent>(id) &&
			mComponentsPool.get().GetComponent<AnimationComponent>(id).mPoseChange;
	}
	std::vector<Matrix4x4f> const& AnimationSystem::GetEntityPose(EntityID id) const
	{
		assert(mComponentsPool.get().EntityHasComponent<AnimationComponent>(id));
		return mComponentsPool.get().GetComponent<AnimationComponent>(id).mAnimController->GetFinalPose();
	}
	void AnimationSystem::FinishUpdatePose(EntityID id) const
	{
		assert(mComponentsPool.get().EntityHasComponent<AnimationComponent>(id));
		mComponentsPool.get().GetComponent<AnimationComponent>(id).mPoseChange = false;
	}
}
