#include "IE_PCH.h"


#include "Animation.h"
#include "AnimationController.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

#include "RunTime/Resource/ResourceManager.h"

namespace inceptionengine
{
	AnimationController::AnimationController() = default;
	AnimationController::~AnimationController()
	{

	}

	bool AnimationController::Update(float deltaTime)
	{
		if (mCurrentAnimation != nullptr)
		{
			mCurrentTime += deltaTime;
			mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->GetDuration());
			mFinalPose = mCurrentAnimation->Interpolate(mCurrentTime);
			return true;
		}
		return false;

	}

	void AnimationController::PlayAnimation(std::string const& filePath)
	{
		std::shared_ptr<Animation> pAnimation = gResourceMgr.GetResource<Animation>(filePath);
		assert(pAnimation != nullptr);
		std::cout << "animation start!\n";
		mCurrentAnimation = pAnimation;
		mCurrentTime = 0.0f;

	}
	void AnimationController::StopAnimation()
	{
		//mFinalPose = mCurrentAnimation->mSkeleton->GetBindPose();
		mCurrentAnimation = nullptr;
		mCurrentTime = 0.0f;
	}
}

