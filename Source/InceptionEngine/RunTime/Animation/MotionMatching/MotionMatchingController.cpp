
#include "IE_PCH.h"

#include "MotionMatchingController.h"
#include "RunTime/Resource/ResourceManager.h"
#include "RunTime/Animation/Animation.h"
#include "MatchingDatabase.h"


namespace inceptionengine
{
	MotionMatchingController::MotionMatchingController() = default;
	MotionMatchingController::~MotionMatchingController() = default;

	void MotionMatchingController::LoadMatchingDatabase(std::string const& filePath)
	{
		mMatchingDB = gResourceMgr.GetResource<MatchingDatabase>(filePath);
		for (auto const& animPath : mMatchingDB->animPaths)
		{
			mAnimDB.push_back(gResourceMgr.GetResource<Animation>(animPath));
		}

		/*
		auto anim = gResourceMgr.GetResource<Animation>(animFile);
		mAnimDB = anim;
		std::cout << "0 frame hip transformation:\n";
		auto mat = anim->mBoneTransforms[137][0];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				std::cout << mat[j][i] << " ";
			}
			std::cout << std::endl;
		}



		mMatchingDB = std::make_shared<MatchingDatabase>(MatchingDatabase::ConstructFromAnim(*anim));


		int frame = 530;
		std::cout << frame << " frame trajory\n";
		std::cout << VecToString(mMatchingDB->features[frame - 10].trajectory[0]) << std::endl;
		std::cout << VecToString(mMatchingDB->features[frame - 10].trajectory[1]) << std::endl;
		std::cout << VecToString(mMatchingDB->features[frame - 10].trajectory[2]) << std::endl;

		std::cout << frame << " frame fracing\n";
		std::cout << VecToString(mMatchingDB->features[frame - 10].facingDirection[0]) << std::endl;
		std::cout << VecToString(mMatchingDB->features[frame - 10].facingDirection[1]) << std::endl;
		std::cout << VecToString(mMatchingDB->features[frame - 10].facingDirection[2]) << std::endl;*/


	}

	void MotionMatchingController::Update(float dt)
	{
		//check if transition is occuring
		if (mMotionBlender.IsBlending())
		{
			auto blendResult = mMotionBlender.Blend(dt);
			if (blendResult.has_value())
			{
				mCurrentPose = blendResult.value();
			}
			return;
		}
		else
		{
			mRunningTime += dt;
			if (mRunningTime >= mAnimDB[mCurrentAnim]->GetDuration()) mRunningTime = 0.0f;
			mCurrentPose = mAnimDB[mCurrentAnim]->Interpolate(mRunningTime);
			mCurrentBoneVelocities = mAnimDB[mCurrentAnim]->mBoneGlobalTranslVelocities[static_cast<int>(mRunningTime * 30.0f)];
		}
	}

	void MotionMatchingController::Update(float dt, MatchingFeature const& f)
	{
		auto index = mMatchingDB->Query(f);

		float const timePerFrame = (1.0f / 30.0f);
		float indexTime = index.second * timePerFrame;

		if (index.first == mCurrentAnim && std::abs(mRunningTime - indexTime) <= 10.0f * timePerFrame) //check whether need transition
		{
			//no need to insert transition, continue to play
			Update(dt);
			return;
		}
		else
		{
			//insert a transition
			mMotionBlender.StartBlending(mCurrentPose, mAnimDB[index.first]->mBoneTransforms[index.second], 2.0f * timePerFrame, AnimBlendType::Inertialization);
			mRunningTime = indexTime;
		}
	}

	std::vector<std::string> const& MotionMatchingController::GetFeatureBones() const
	{
		return mMatchingDB->featureBones;
	}

	MatchingFeature MotionMatchingController::GenerateFeatureTemp()
	{
		MatchingFeature f;
		f.trajectory[0] = { 0.0f, 0.0f, 40.0f };
		f.trajectory[1] = { 0.0f, 0.0f, 80.0f };
		f.trajectory[2] = { 0.0f, 0.0f, 120.0f };
		f.facingDirection[0] = { 0.0f,1.0f,0.0f };
		f.facingDirection[1] = { 0.0f,1.0f,0.0f };
		f.facingDirection[2] = { 0.0f,1.0f,0.0f };

		//f.leftFootPosition;
		//f.rightFootPosition;

		//f.hipVelocity;
		//f.leftFootVelocity;
		//f.rightFootVelocity;

		return f;

	}
}

