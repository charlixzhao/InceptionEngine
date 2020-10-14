#include "IE_PCH.h"

#include "AnimationSystem.h"
#include "ECS/Entity/EntityComponentPool.hpp"
#include "RunTime/Animation/AnimationController.h"
#include "ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.h"

namespace inceptionengine
{
	AnimationSystem::AnimationSystem(ComponentsPool& componentsPool)
		:SystemBase(componentsPool)
	{

	}

	void AnimationSystem::Update(float deltaTime)
	{
		auto& view = mComponentsPool.get().GetComponentPool<SkeletalMeshComponent>()->GetComponentView();

		for (auto& component : view)
		{
			component.mAnimationController->Update(deltaTime);
		}
	}


}
