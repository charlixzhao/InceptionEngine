
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

		if (index.first == mCurrentAnim && std::abs(static_cast<int>(mRunningTime * 30.0f) - index.second) <= 3) //check whether need transition
		{
			//no need to insert transition, continue to play
			Update(dt);
			return;
		}
		else
		{
			//insert a transition
			mMotionBlender.StartBlending(mCurrentPose, mAnimDB[index.first]->mBoneTransforms[index.second], 4.0f * timePerFrame, AnimBlendType::Inertialization);
			mRunningTime = indexTime;
			mCurrentAnim = index.first;
		}
	}

	std::vector<std::string> const& MotionMatchingController::GetFeatureBones() const
	{
		return mMatchingDB->featureBones;
	}

	/*
	x = v[0] * np.cos(theta) - v[1] * np.sin(theta)
		y = v[0] * np.sin(theta) + v[1] * np.cos(theta*/

	MatchingFeature MotionMatchingController::GenerateFeatureTemp(Vec3f const& currentFacing)
	{
		MatchingFeature f;
		float rads = 0.0f;
		if (VecLength(mInputControl) != 0.0f)
		{
			rads = RadsBetween(currentFacing, mInputControl);
			rads *= -Sign(CrossProduct(currentFacing, mInputControl)[1]);
		}
		for (int i = 1; i <= MatchingFeature::NPoints; i++)
		{
			float theta = static_cast<float>(i) * rads / static_cast<float>(MatchingFeature::NPoints);
			float x = currentFacing[0] * std::cos(theta) - currentFacing[2] * std::sin(theta);
			float z = currentFacing[0] * std::sin(theta) + currentFacing[2] * std::cos(theta);
			f.futureFacing[i - 1] = { x, 0.0f, z };
		}

		float speed = 30.0f;
		
		f.futureTrajectory[0] = f.futureFacing[0] * speed * DotProduct(f.futureFacing[0], mInputControl);
		for (int i = 1; i < MatchingFeature::NPoints; i++)
		{
			f.futureTrajectory[i] = f.futureTrajectory[i - 1] + f.futureFacing[i] * speed * DotProduct(f.futureFacing[i], mInputControl);
		}

		return f;

	}
	void MotionMatchingController::SetInputControl(Vec3f const& input)
	{
		mInputControl = input;
	}
}

