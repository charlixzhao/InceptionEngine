#pragma once

#include "AnimBlender.h"

namespace inceptionengine
{
	class AnimInstance;
	struct EventAnimPlaySetting;
	class AnimationController;

	struct AnimNotify;

	class EventAnimController
	{
	public:
		EventAnimController(std::reference_wrapper<AnimationController> animationController);

		~EventAnimController();

		void PlayEventAnimation(EventAnimPlaySetting const& setting);

		bool IsPlayingAnimation() const;

		void Update(float dt);

		AnimPose const& GetCurrentPose() const;

		bool IsBlending() const;

		float GetCurrentEventAnimTime() const;

		float GetCurrentEventAnimDuration() const;

		void InsertAnimSpeedRange(float startRatio, float endRatio, float playSpeed);

		void InsertAnimNotify(AnimNotify const& notify);

		
	private:
		bool NotifyCurrentAnimInstance();

	private:
		std::unique_ptr<AnimInstance> mAnimInstance = nullptr;
		float mRunningTime = 0.0f;
		AnimPose mCurrentPose;
		std::reference_wrapper<AnimationController> mAnimationController;
		AnimBlender mEventAnimBlender;


	};
}