#pragma once

#include "EngineGlobals/EngineApiPrefix.h"


namespace inceptionengine
{
	class AnimationSystem;
	class AnimationController;
	struct Skeleton;

	class IE_API AnimationComponent
	{
	public:
		AnimationComponent(AnimationSystem& system, Skeleton const& skeleton);

		~AnimationComponent();

		AnimationComponent(AnimationComponent&&) noexcept;

		void PlayAnimation(std::string const& filePath);

		void StopAnimation();


	private:
		friend class AnimationSystem;

		std::reference_wrapper<AnimationSystem> mAnimationSystem;

		std::unique_ptr<AnimationController> mAnimController = nullptr;

		bool mPoseChange = false;

	};
}