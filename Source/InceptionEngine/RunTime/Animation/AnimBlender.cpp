#include "IE_PCH.h"
#include "AnimBlender.h"

namespace inceptionengine
{
	void AnimBlender::StartBlending(std::vector<Matrix4x4f> const& fromPose, std::vector<Matrix4x4f> const& toPose, float blendingDuration, AnimBlendType blendingType)
	{
		mBlendFromPose = fromPose;
		mBlendToPose = toPose;
		mBlendingDuration = blendingDuration;
		mBlendingType = blendingType;
		mCurrentBlendingTime = 0.0f;
	}

	std::optional<std::vector<Matrix4x4f>> AnimBlender::Blend(float dt)
	{
		mCurrentBlendingTime += dt;

		if (mCurrentBlendingTime > mBlendingDuration)
		{
			mCurrentBlendingTime = mBlendingDuration = 0.0f;
			return std::nullopt;
		}
		else
		{
			float alpha = 1.0f - mCurrentBlendingTime / mBlendingDuration;
			return BlendPose(alpha);
		}
	}

	bool AnimBlender::IsBlending() const
	{
		return mBlendingDuration > 0.0f;
	}

	void AnimBlender::InterruptBlending()
	{
		mCurrentBlendingTime = mBlendingDuration = 0.0f;
	}

	std::vector<Matrix4x4f> AnimBlender::BlendPose(float alpha) const
	{
		std::vector<Matrix4x4f> result;
		result.resize(mBlendFromPose.size());
		for (int i = 0; i < mBlendFromPose.size(); i++)
		{
			Vec4f translation1;
			Quaternion4f rotation1;
			Vec4f scale1;

			Vec4f translation2;
			Quaternion4f rotation2;
			Vec4f scale2;

			Decompose(mBlendFromPose[i], translation1, rotation1, scale1);
			Decompose(mBlendToPose[i], translation2, rotation2, scale2);
			Vec4f translation = LinearInterpolate(translation1, translation2, alpha);
			Quaternion4f rotation = SLerp(rotation1, rotation2, alpha);
			Vec4f scale = LinearInterpolate(scale1, scale2, alpha);
			Matrix4x4f transformation = Compose(translation, rotation, scale);
			result[i] = transformation;
		}
		return result;
	}
}