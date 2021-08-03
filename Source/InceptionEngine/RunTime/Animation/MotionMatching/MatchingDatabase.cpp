#include "IE_PCH.h"
#include "MatchingDatabase.h"
#include "RunTime/Animation/Animation.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

namespace inceptionengine
{
	int MatchingDatabase::Query(MatchingFeature const& f1)
	{
		float minCost = std::numeric_limits<float>().max();
		int argmin = -1;
		for (int i = 0; auto const& f : features)
		{
			float cost = FeatureDistance(f, f1);
			if (cost < minCost)
			{
				minCost = cost;
				argmin = i;
			}
			i++;
		}

		printf("argmin is %d with cost %f\n", argmin, minCost);

		return argmin;
	}

	float MatchingDatabase::FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2)
	{
		return Distance(f1.trajectory[0] / trajectorySD[0], f2.trajectory[0] / trajectorySD[0])
			+ Distance(f1.trajectory[1] / trajectorySD[1], f2.trajectory[1] / trajectorySD[1])
			+ Distance(f1.trajectory[2] / trajectorySD[2], f2.trajectory[2] / trajectorySD[2])
			+ Distance(f1.facingDirection[0] / facingDirectionSD[0], f2.facingDirection[0] / facingDirectionSD[0])
			+ Distance(f1.facingDirection[1] / facingDirectionSD[1], f2.facingDirection[1] / facingDirectionSD[1])
			+ Distance(f1.facingDirection[2] / facingDirectionSD[2], f2.facingDirection[2] / facingDirectionSD[2])
			+ Distance(f1.leftFootPosition / leftFootPositionSD, f2.leftFootPosition / leftFootPositionSD)
			+ Distance(f1.rightFootPosition / rightFootPositionSD, f2.rightFootPosition / rightFootPositionSD)
			+ Distance(f1.leftFootVelocity / leftFootVelocitySD, f2.leftFootVelocity / leftFootVelocitySD)
			+ Distance(f1.rightFootVelocity / rightFootVelocitySD, f2.rightFootVelocity / rightFootVelocitySD)
			+ Distance(f1.hipVelocity / hipVelocitySD, f2.hipVelocity / hipVelocitySD);
	}

	MatchingDatabase MatchingDatabase::ConstructFromAnim(Animation const& anim)
	{
		MatchingDatabase db;

		int constexpr FacingAxis = 1;
		std::vector<std::vector<Matrix4x4f>> globalTransforms;
		globalTransforms.reserve(anim.mBoneTransforms.size());
		for (auto const& frame : anim.mBoneTransforms)
		{
			globalTransforms.push_back(Animation::GetBonesGlobalTransforms(frame, anim.mSkeleton));
		}

		for (int i = 0; i < anim.mBoneTransforms.size() - 30; i++)
		{
			MatchingFeature f{};

			std::vector<Matrix4x4f> const& currentPose = globalTransforms[i]; //GetBoneGlobalTransforms(anim.mBoneTransforms[i], anim.mSkeleton);
			std::vector<Matrix4x4f> const& next10Pose = globalTransforms[i + 10];//GetBoneGlobalTransforms(anim.mBoneTransforms[i + 10], anim.mSkeleton);
			std::vector<Matrix4x4f> const& next20Pose = globalTransforms[i + 20];//GetBoneGlobalTransforms(anim.mBoneTransforms[i + 20], anim.mSkeleton);
			std::vector<Matrix4x4f> const& next30Pose = globalTransforms[i + 30];//GetBoneGlobalTransforms(anim.mBoneTransforms[i + 30], anim.mSkeleton);
			//std::vector<Matrix4x4f> const& prev10Pose = globalTransforms[i - 10];//GetBoneGlobalTransforms(anim.mBoneTransforms[i - 10], anim.mSkeleton);

			//Vec3f currentPosition = ProjectToXZ(currentPose[0][3]); 
			//f.trajectory[0] = ProjectToXZ(next10Pose[0][3]) - currentPosition;
			//f.trajectory[1] = ProjectToXZ(next20Pose[0][3]) - currentPosition;
			//f.trajectory[2] = ProjectToXZ(next30Pose[0][3]) - currentPosition;

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
			float timeInterval = 10.0f / 30.0f;
			f.leftFootPosition = Vec3f(currentPose[anim.mSkeleton->GetBoneID(leftFootName)][3]) - currentPosition;
			f.rightFootPosition = Vec3f(currentPose[anim.mSkeleton->GetBoneID(rightFootName)][3]) - currentPosition;

			/*NEED VEL: during compute in building matching db*/
			//f.leftFootVelocity = (next10Pose[anim.mSkeleton->GetBoneID(leftFootName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(leftFootName)][3]) / (2 * timeInterval);
			//f.rightFootVelocity = (next10Pose[anim.mSkeleton->GetBoneID(rightFootName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(rightFootName)][3]) / (2 * timeInterval);
			//f.hipVelocity = (next10Pose[anim.mSkeleton->GetBoneID(hipName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(hipName)][3]) / (2 * timeInterval);

			db.features.push_back(f);
		}

		return db;
	}
}

