
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

	void MatchingDatabaseBuilder::ConstructFromAnim(std::vector<std::string> const& animFiles,
		std::string const& dbSavePath, std::vector<std::string> const& featureBones)
	{
		MatchingDatabase db;

		db.animPaths.resize(animFiles.size());
		db.features.resize(animFiles.size());
		db.featureBones = featureBones;
		for (int animIndex = 0; animIndex < animFiles.size(); animIndex++)
		{
			printf("process anim %d\n", animIndex);
			std::string const& animFile = animFiles[animIndex];
			auto anim = Serializer::Deserailize<Animation>(PathHelper::GetAbsolutePath(animFile + ".ie_anim"));
			anim->mSkeleton = Serializer::Deserailize<Skeleton>(PathHelper::GetAbsolutePath(anim->mPathToSkeleton));
			assert(anim != nullptr);

			db.animPaths[animIndex] = animFile + ".ie_anim";

			int constexpr FacingAxis = 1;
			std::vector<std::vector<Matrix4x4f>> globalTransforms;
			globalTransforms.reserve(anim->mBoneTransforms.size());
			for (auto const& frame : anim->mBoneTransforms)
			{
				globalTransforms.push_back(GetBonesGlobalTransforms(frame, anim->mSkeleton));
			}

			//extract features
			for (int i = 0; i < anim->mBoneTransforms.size() - 10 * MatchingFeature::NPoints; i++)
			{
				MatchingFeature f{};

				std::vector<Matrix4x4f> const& currentPose = globalTransforms[i];
				Vec3f currentPosition = currentPose[0][3];
				f.currentFacing = currentPose[0][FacingAxis];
				for (int nPoint = 1; nPoint <= MatchingFeature::NPoints; nPoint++)
				{
					std::vector<Matrix4x4f> const& nextN0Pose = globalTransforms[i + 10 * nPoint];
					f.futureTrajectory[nPoint - 1] = Vec3f(nextN0Pose[0][3]) - currentPosition;
					f.futureFacing[nPoint - 1] = nextN0Pose[0][FacingAxis];
				}

				float constexpr timeInterval = 10.0f / 30.0f;
				for (auto const& featureBone : db.featureBones)
				{
					f.featureBonePos.push_back(Vec3f(currentPose[anim->mSkeleton->GetBoneID(featureBone)][3]) - currentPosition);
					f.featureBoneVels.push_back(anim->mBoneGlobalTranslVelocities[i][anim->mSkeleton->GetBoneID(featureBone)]);
				}

				db.features[animIndex].push_back(f);
			}
		}
		

		for (size_t point = 0; point < MatchingFeature::NPoints; point++)
		{
			std::vector<Vec3f> trajTemp;
			std::vector<Vec3f> facingTemp;
			for (auto const& anim : db.features)
			{
				for (auto const& f : anim)
				{
					trajTemp.push_back(f.futureTrajectory[point]);
					facingTemp.push_back(f.futureFacing[point]);
				}
			}
			db.trajectorySD[point] = ElementSqrt(Variance(trajTemp));
			db.facingDirectionSD[point] = ElementSqrt(Variance(facingTemp));
		}

		for (int featureBone = 0; featureBone < featureBones.size(); featureBone++)
		{
			std::vector<Vec3f> posTemp;
			std::vector<Vec3f> velTemp;
			for (auto const& anim : db.features)
			{
				for (auto const& f : anim)
				{
					posTemp.push_back(f.featureBonePos[featureBone]);
					velTemp.push_back(f.featureBoneVels[featureBone]);
				}
			}
			db.featureBonePosSDs.push_back(ElementSqrt(Variance(posTemp)));
			db.featureBoneVelSDs.push_back(ElementSqrt(Variance(velTemp)));
		}

		std::vector<Vec3f> facingTemp;
		for (auto const& anim : db.features)
		{
			for (auto const& f : anim)
			{
				facingTemp.push_back(f.currentFacing);
			}
		}
		db.currentFacingSD = ElementSqrt(Variance(facingTemp));

		assert(db.featureBonePosSDs.size() == featureBones.size());
		assert(db.featureBoneVelSDs.size() == featureBones.size());

		Serializer::Serailize<MatchingDatabase>(db, PathHelper::GetAbsolutePath(dbSavePath));

	}
}
