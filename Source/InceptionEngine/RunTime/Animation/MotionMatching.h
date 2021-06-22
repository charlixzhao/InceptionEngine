#pragma once

#include "Animation.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

namespace inceptionengine
{
	namespace motionmatching
	{

		inline std::vector<Matrix4x4f> GetBoneModelTransforms(std::vector<Matrix4x4f> const& localFinalPose, std::shared_ptr<Skeleton const> skeleton)
		{
			std::vector<Matrix4x4f> globalFinalPose;
			globalFinalPose.resize(localFinalPose.size());
			for (auto const& bone : skeleton->mBones)
			{
				Matrix4x4f globalTransform = localFinalPose[bone.ID];
				int parentID = bone.parentID;
				while (parentID != 0)
				{
					globalTransform = localFinalPose[parentID] * globalTransform;
					parentID = skeleton->mBones[parentID].parentID;
				}
				globalFinalPose[bone.ID] = globalTransform;
			}

			return globalFinalPose;
		}


		struct Feature
		{
			std::array<Vec3f, 3> trajectory;  //10 frame, 20 frame, 30 frame
			std::array<Vec3f, 3> facingDirection;
			Vec3f leftFootPosition;
			Vec3f rightFootPosition;
			Vec3f leftFootVelocity;
			Vec3f rightFootVelocity;
			Vec3f hipVelocity;

			static float FeatureDistance(Feature const& f1, Feature const& f2)
			{
				return Distance(f1.trajectory[0], f2.trajectory[0]) 
					+ Distance(f1.trajectory[1], f2.trajectory[1]) 
					+ Distance(f1.trajectory[2], f2.trajectory[2])
					+ Distance(f1.facingDirection[0], f2.facingDirection[0]) 
					+ Distance(f1.facingDirection[1], f2.facingDirection[1]) 
					+ Distance(f1.facingDirection[2], f2.facingDirection[2])
					+ Distance(f1.leftFootPosition, f2.leftFootPosition) 
					+ Distance(f1.rightFootPosition, f2.rightFootPosition)
					+ Distance(f1.leftFootVelocity, f2.leftFootVelocity)
					+ Distance(f1.rightFootVelocity, f2.rightFootVelocity)
					+ Distance(f1.hipVelocity, f2.hipVelocity);
			}
		};

		struct Pose
		{
			std::vector<Vec3f> lclTranslations;
			std::vector<Quaternion4f> lclRotations;
			std::vector<Vec3f> lclTranslationVel;
			std::vector<Vec4f> lclRotationVel;
			Vec3f lclRootTranslationVel; // local to character facing dirction
			Vec4f lclRootRotationVel; // local to character facing direction
		};

		struct MatchingDB
		{
			std::vector<Feature> features;

			int Query(Feature const& f1)
			{
				float minCost = std::numeric_limits<float>().max();
				int argmin = -1;
				for (int i = 0; auto const& f : features)
				{
					if (Feature::FeatureDistance(f, f1) < minCost)
					{
						minCost = Feature::FeatureDistance(f, f1);
						argmin = i;
					}
					i++;
				}
				return argmin;
			}

			static MatchingDB ConstructFromAnim(Animation const& anim)
			{
				MatchingDB db;

				for (int i = 10; i < anim.mBoneTransforms.size() - 30; i++)
				{
					Feature f;

					std::vector<Matrix4x4f> currentPose = GetBoneModelTransforms(anim.mBoneTransforms[i], anim.mSkeleton);
					std::vector<Matrix4x4f> next10Pose = GetBoneModelTransforms(anim.mBoneTransforms[i + 10], anim.mSkeleton);
					std::vector<Matrix4x4f> next20Pose = GetBoneModelTransforms(anim.mBoneTransforms[i + 20], anim.mSkeleton);
					std::vector<Matrix4x4f> next30Pose = GetBoneModelTransforms(anim.mBoneTransforms[i + 30], anim.mSkeleton);
					std::vector<Matrix4x4f> prev10Pose = GetBoneModelTransforms(anim.mBoneTransforms[i - 10], anim.mSkeleton);

					f.trajectory[0] = next10Pose[0][3];
					f.trajectory[1] = next20Pose[0][3];
					f.trajectory[2] = next30Pose[0][3];
					f.facingDirection[0] = next10Pose[0][2];
					f.facingDirection[1] = next20Pose[0][2];
					f.facingDirection[2] = next30Pose[0][2];

					std::string leftFootName = "leftfoot";
					std::string rightFootName = "rightfoot";
					std::string hipName = "hip";
					float timeInterval = 10.0f / 30.0f;
					f.leftFootPosition = currentPose[anim.mSkeleton->GetBoneID(leftFootName)][3];
					f.rightFootPosition = currentPose[anim.mSkeleton->GetBoneID(rightFootName)][3];
					f.leftFootVelocity = (next10Pose[anim.mSkeleton->GetBoneID(leftFootName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(leftFootName)][3]) / (2 * timeInterval);
					f.rightFootVelocity = (next10Pose[anim.mSkeleton->GetBoneID(rightFootName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(rightFootName)][3]) / (2 * timeInterval);
					f.hipVelocity = (next10Pose[anim.mSkeleton->GetBoneID(hipName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(hipName)][3]) / (2 * timeInterval);
					
					db.features.push_back(f);
				}

				return db;
			}
		};
		
		struct AnimDB
		{
			std::vector<Pose> poses;
		};

		




	}
}
