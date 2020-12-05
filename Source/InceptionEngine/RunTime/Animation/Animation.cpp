
#include "IE_PCH.h"
#include "Animation.h"

namespace inceptionengine
{

	std::vector<Matrix4x4f> BlendPose(std::vector<Matrix4x4f> const& pose1, std::vector<Matrix4x4f> const& pose2, float alpha)
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

	std::vector<Matrix4x4f> Animation::Interpolate(float time) const
	{
		if (time <= 0)
		{
			return mBoneTransforms[0];
		}

		int count = static_cast<int>((time * 30.0f));

		if(count >= mBoneTransforms.size() - 1)
			return mBoneTransforms.back();

		float countTime1 = static_cast<float>(count) * (1.0f / 30.0f);
		float countTime2 = countTime1 + (1.0f / 30.0f);
		assert(time >= countTime1 && time <= countTime2);
		float alpha = 1.0f - ((time - countTime1) / (1.0f / 30.0f)); //weights of the first pose

		return BlendPose(mBoneTransforms[count], mBoneTransforms[count + 1], alpha);
	}
}

