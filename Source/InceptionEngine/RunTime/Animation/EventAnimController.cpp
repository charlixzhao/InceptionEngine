
#include "IE_PCH.h"
#include "EventAnimController.h"
#include "AnimInstance.h"
#include "AnimationController.h"

namespace inceptionengine
{
	EventAnimController::EventAnimController(std::reference_wrapper<AnimationController> animationController)
		:mAnimationController(animationController)
	{

	}

	EventAnimController::~EventAnimController() = default;

	void EventAnimController::PlayEventAnimation(EventAnimPlaySetting const& setting)
	{
		if (mAnimInstance != nullptr)
		{
			mAnimInstance->Interrupt();
		}

		mAnimInstance = std::make_unique<AnimInstance>(setting);
		mRunningTime = 0.0f;
		mAnimInstance->Start();
		mAnimationController.get().EventAnimStart();
	}

	bool EventAnimController::IsPlayingAnimation() const
	{
		return mAnimInstance != nullptr;
	}
	void EventAnimController::Update(float dt)
	{
		if (mAnimInstance != nullptr)
		{
			float currentAnimSpeend = mAnimInstance->QueryAnimSpeed(mRunningTime / mAnimInstance->GetDuration());
			mRunningTime += dt * currentAnimSpeend;
			if (mRunningTime > mAnimInstance->GetDuration())
			{
				//stop animation
				mAnimInstance->End();
				mRunningTime = 0.0f;
				mAnimInstance = nullptr;
				mAnimationController.get().EventAnimFinish();
			}
			else
			{
				mAnimInstance->Notify(mRunningTime);
				mCurrentPose = mAnimInstance->Sample(mRunningTime);
			}
		}
	}
	std::vector<Matrix4x4f> const& EventAnimController::GetCurrentPose() const
	{
		return mCurrentPose;
	}
}


