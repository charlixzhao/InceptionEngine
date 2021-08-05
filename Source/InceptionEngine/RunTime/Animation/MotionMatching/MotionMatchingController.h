#pragma once
#include "RunTime/Animation/AnimBlender.h"
#include "MatchingFeature.h"
#include "RunTime/Animation/AnimPose.h"

namespace inceptionengine
{
	struct MatchingFeature;
	struct MatchingDatabase;
	struct Animation;

	class MotionMatchingController
	{
	public:
		MotionMatchingController();

		~MotionMatchingController();

		AnimPose GetCurrentPose() const { return mCurrentPose; }
		std::vector<Vec3f> GetCurrentBoneVelocities() const { return mCurrentBoneVelocities; }

		void LoadMatchingDatabase(std::string const& filePath);

		//continue to play on the current anim
		void Update(float dt);

		//new feature arrive, check if transition need to be insert
		void Update(float dt, MatchingFeature const& f);

		//not impl yet
		MatchingFeature GenerateFeatureTemp();

	private:
		std::shared_ptr<MatchingDatabase> mMatchingDB = nullptr;
		std::shared_ptr<Animation> mAnimDB = nullptr;
		AnimPose mCurrentPose;
		std::vector<Vec3f> mCurrentBoneVelocities;
		int mCurrentIndex = -1;
		float mRunningTime = 530.0f / 30.0f;
		AnimBlender mMotionBlender;

	};
}
