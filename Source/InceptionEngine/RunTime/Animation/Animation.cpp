
#include "IE_PCH.h"
#include "Animation.h"
#include "AnimPose.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

namespace inceptionengine
{
	std::vector<Matrix4x4f> LinearInterpolate(std::vector<Matrix4x4f> const& pose1, std::vector<Matrix4x4f> const& pose2, float alpha)
	{
		std::vector<Matrix4x4f> result;
		result.resize(pose1.size());
		for (int i = 0; i < pose1.size(); i++)
		{
			Vec4f translation1;
			Quaternion4f rotation1;
			Vec4f scale1;

			Vec4f translation2;
			Quaternion4f rotation2;
			Vec4f scale2;

			Decompose(pose1[i], translation1, rotation1, scale1);
			Decompose(pose2[i], translation2, rotation2, scale2);
			Vec4f translation = LinearInterpolate(translation1, translation2, alpha);
			Quaternion4f rotation = SLerp(rotation1, rotation2, alpha);
			Vec4f scale = LinearInterpolate(scale1, scale2, alpha);
			Matrix4x4f transformation = Compose(translation, rotation, scale);
			result[i] = transformation;
		}
		return result;
	}

	AnimPose Animation::Interpolate(float time) const
	{
		if (time <= 0)
		{
			return AnimPose(mBoneTransforms[0], mBoneLclTranslVelocities[0], mBoneGlobalTranslVelocities[0],
				mBoneLclAngularVelocities[0], mBoneGlobalAngularVelocities[0]);
		}

		time = std::fmodf(time, mDuration);

		int count = static_cast<int>(time * 30.0f);

		if(count >= mBoneTransforms.size() - 1)
			return AnimPose(mBoneTransforms.back(), mBoneLclTranslVelocities.back(), mBoneGlobalTranslVelocities.back(),
				mBoneLclAngularVelocities.back(), mBoneGlobalAngularVelocities.back());

		float countTime1 = static_cast<float>(count) / 30.0f;
		float countTime2 = countTime1 + (1.0f / 30.0f);
		assert(time >= countTime1 && time <= countTime2);
		float alpha = 1.0f - ((time - countTime1) / (1.0f / 30.0f)); //weights of the first pose

		return AnimPose(LinearInterpolate(mBoneTransforms[count], mBoneTransforms[count + 1], alpha),
			mBoneLclTranslVelocities[count], mBoneGlobalTranslVelocities[count],
			mBoneLclAngularVelocities[count], mBoneGlobalAngularVelocities[count]);
	}

	std::vector<Matrix4x4f> Animation::GetBonesGlobalTransforms(std::vector<Matrix4x4f> const& localTransforms, std::shared_ptr<Skeleton const> skeleton)
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



}

