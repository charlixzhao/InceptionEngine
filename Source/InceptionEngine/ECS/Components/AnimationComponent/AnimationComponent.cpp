
#include "IE_PCH.h"

#include "AnimationComponent.h"
#include "RunTime/Animation/AnimationController.h"
#include "PathHelper.h"

namespace inceptionengine
{
	AnimationComponent::AnimationComponent(AnimationSystem& system, Skeleton const& skeleton)
		:mAnimationSystem(system)
	{
		mAnimController = std::make_unique<AnimationController>(skeleton);
	}

	AnimationComponent::~AnimationComponent() = default;

	AnimationComponent::AnimationComponent(AnimationComponent&&) noexcept = default;

	
	void AnimationComponent::PlayAnimation(std::string const& filePath)
	{
		mAnimController->PlayAnimation(filePath);
	}

	void AnimationComponent::StopAnimation()
	{
		mAnimController.get()->StopAnimation();
		mPoseChange = true;
	}

}

