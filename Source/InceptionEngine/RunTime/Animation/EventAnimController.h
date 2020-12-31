#pragma once

#include "AnimBlender.h"

namespace inceptionengine
{
	class AnimInstance;
	struct EventAnimPlaySetting;
	class AnimationController;

	class EventAnimController
	{
	public:
		EventAnimController(std::reference_wrapper<AnimationController> animationController);

		~EventAnimController();

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		bool IsPlayingAnimation() const;

		void Update(float dt);

		std::vector<Matrix4x4f> const& GetCurrentPose() const;

		bool IsBlending() const;

		float GetCurrentEventAnimTime() const;

		float GetCurrentEventAnimDuration() const;

		void InsertAnimSpeedRange(float startRatio, float endRatio, float playSpeed);

	private:
		std::unique_ptr<AnimInstance> mAnimInstance = nullptr;
		float mRunningTime = 0.0f;
		std::vector<Matrix4x4f> mCurrentPose;
		std::reference_wrapper<AnimationController> mAnimationController;
		AnimBlender mEventAnimBlender;


	};
}