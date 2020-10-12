
#include "IE_PCH.h"

#include "AnimationComponent.h"
#include "RunTime/Animation/AnimationController.h"
#include "PathHelper.h"

namespace inceptionengine
{
	AnimationComponent::AnimationComponent(AnimationSystem& system)
		:mAnimationSystem(system)
	{
		mAnimController = std::make_unique<AnimationController>();
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

