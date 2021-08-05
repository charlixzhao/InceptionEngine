
#include "MatchingDatabaseBuilder.h"

#include "Math/Math.h"
#include "PathHelper.h"
#include "Serialization/Serializer.h"

#include "RunTime/Animation/Animation.h"
#include "RunTime/Animation/MotionMatching/MatchingDatabase.h"
#include "RunTime/Animation/MotionMatching/MatchingFeature.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

#include <cassert>

#define STDDEV(field) \
{ \
std::vector<Vec3f> temp; \
for (auto const& f : db.features) \
{ \
	temp.push_back(f.field); \
} \
db.field##SD = ElementSqrt(Variance(temp)); \
}

#define STDDEV_N(field, n) \
{ \
std::vector<Vec3f> temp; \
for (auto const& f : db.features) \
{ \
	temp.push_back(f.field[n]); \
} \
db.field##SD[n] = ElementSqrt(Variance(temp)); \
}


namespace inceptionengine::animeditor
{
	std::vector<Matrix4x4f> GetBonesGlobalTransforms(std::vector<Matrix4x4f> const& localTransforms, std::shared_ptr<Skeleton const> skeleton)
	{
		
		std::vector<Matrix4x4f> globalFinalPose;
		globalFinalPose.resize(localTransforms.size());
		for (auto const& bone : skeleton->mBones)
		{
			Matrix4x4f globalTransform = localTransforms[bone.ID];
			int parentID = bone.parentID;
			while (parentID != -1)
			{
				globalTransform = localTransforms[parentID] * globalTransform;
				parentID = skeleton->mBones[parentID].parentID;
			}
			globalFinalPose[bone.ID] = globalTransform;
		}

		return globalFinalPose;
	}

	void MatchingDatabaseBuilder::ConstructFromAnim(std::string const& animFile, std::string const& dbSavePath)
	{
		auto anim = Serializer::Deserailize<Animation>(PathHelper::GetAbsolutePath(animFile + ".ie_anim"));
		anim->mSkeleton = Serializer::Deserailize<Skeleton>(PathHelper::GetAbsolutePath(anim->mPathToSkeleton));
		assert(anim != nullptr);

		MatchingDatabase db;
		db.animPaths.push_back(animFile + ".ie_anim");

		int constexpr FacingAxis = 1;
		std::vector<std::vector<Matrix4x4f>> globalTransforms;
		globalTransforms.reserve(anim->mBoneTransforms.size());
		for (auto const& frame : anim->mBoneTransforms)
		{
			globalTransforms.push_back(GetBonesGlobalTransforms(frame, anim->mSkeleton));
		}

		//extract features
		for (int i = 0; i < anim->mBoneTransforms.size() - 30; i++)
		{
			MatchingFeature f{};

			std::vector<Matrix4x4f> const& currentPose = globalTransforms[i]; 
			std::vector<Matrix4x4f> const& next10Pose = globalTransforms[i + 10];
			std::vector<Matrix4x4f> const& next20Pose = globalTransforms[i + 20];
			std::vector<Matrix4x4f> const& next30Pose = globalTransforms[i + 30];

			Vec3f currentPosition = currentPose[0][3];
			f.trajectory[0] = Vec3f(next10Pose[0][3]) - currentPosition;
			f.trajectory[1] = Vec3f(next20Pose[0][3]) - currentPosition;
			f.trajectory[2] = Vec3f(next30Pose[0][3]) - currentPosition;

			f.facingDirection[0] = next10Pose[0][FacingAxis];
			f.facingDirection[1] = next20Pose[0][FacingAxis];
			f.facingDirection[2] = next30Pose[0][FacingAxis];

			std::string leftFootName = "Model:LeftFoot";
			std::string rightFootName = "Model:RightFoot";
			std::string hipName = "Model:Hips";
			float constexpr timeInterval = 10.0f / 30.0f;
			f.leftFootPosition = Vec3f(currentPose[anim->mSkeleton->GetBoneID(leftFootName)][3]) - currentPosition;
			f.rightFootPosition = Vec3f(currentPose[anim->mSkeleton->GetBoneID(rightFootName)][3]) - currentPosition;

			/*NEED VEL: during compute in building matching db*/
			f.hipVelocity = anim->mBoneGlobalTranslVelocities[i][anim->mSkeleton->GetBoneID(hipName)];
			f.leftFootVelocity = anim->mBoneGlobalTranslVelocities[i][anim->mSkeleton->GetBoneID(leftFootName)];
			f.rightFootVelocity = anim->mBoneGlobalTranslVelocities[i][anim->mSkeleton->GetBoneID(rightFootName)];

			db.features.push_back(f);
		}

		//extract normalize statistics
		STDDEV_N(trajectory, 0); STDDEV_N(trajectory, 1); STDDEV_N(trajectory, 2);
		STDDEV_N(facingDirection, 0); STDDEV_N(facingDirection, 1); STDDEV_N(facingDirection, 2);
		STDDEV(leftFootPosition);
		STDDEV(rightFootPosition);
		STDDEV(leftFootVelocity);
		STDDEV(rightFootVelocity);
		STDDEV(hipVelocity);


		Serializer::Serailize<MatchingDatabase>(db, PathHelper::GetAbsolutePath(dbSavePath));

	}
}
