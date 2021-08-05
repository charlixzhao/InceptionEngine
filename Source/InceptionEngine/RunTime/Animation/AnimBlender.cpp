#include "IE_PCH.h"
#include "AnimBlender.h"

namespace inceptionengine
{
	void AnimBlender::StartBlending(AnimPose const& fromPose,
		AnimPose const& toPose,
		float blendingDuration,
		AnimBlendType blendingType,
		std::function<void()> blendFinishCallback)
	{
		mBlendFromPose = fromPose;
		mBlendToPose = toPose;
		mBlendingDuration = blendingDuration;
		mBlendingType = blendingType;
		mCurrentBlendingTime = 0.0f;
		mBlendFinishCallback = blendFinishCallback;
	}

	std::optional<AnimPose> AnimBlender::Blend(float dt, bool indicateStop)
	{
		mCurrentBlendingTime += dt;

		if (mCurrentBlendingTime > mBlendingDuration)
		{
			if (indicateStop)
			{
				mCurrentBlendingTime = mBlendingDuration = 0.0f;
				mBlendFinishCallback();
				return std::nullopt;
			}
			else
			{
				return mBlendToPose;
			}

		}
		else
		{
			switch (mBlendingType)
			{
			case AnimBlendType::Linear: 
			{
				float alpha = 1.0f - mCurrentBlendingTime / mBlendingDuration;
				return LinearBlend(alpha);
			}
			case AnimBlendType::Inertialization: 
			{
				break;
			}
			default: throw std::runtime_error("un supported blending type");
			}

		}
	}

	std::optional<AnimPose> AnimBlender::Blend(AnimPose const& blendToPose, float dt, bool indicateStop)
	{
		if (blendToPose.Size() != mBlendFromPose.Size())
		{
			throw std::runtime_error("inconsistent blend pose\n");
		}
		mBlendToPose = blendToPose;
		return Blend(dt, indicateStop);
	}

	bool AnimBlender::IsBlending() const
	{
		return mBlendingDuration > 0.0f;
	}

	void AnimBlender::InterruptBlending()
	{
		mCurrentBlendingTime = mBlendingDuration = 0.0f;
	}

	AnimPose AnimBlender::LinearBlend(float alpha) const
	{
		std::vector<Matrix4x4f> boneTransforms;
		boneTransforms.resize(mBlendFromPose.Size());
		for (int i = 0; i < mBlendFromPose.Size(); i++)
		{
			Vec4f translation1;
			Quaternion4f rotation1;
			Vec4f scale1;

			Vec4f translation2;
			Quaternion4f rotation2;
			Vec4f scale2;

			Decompose(mBlendFromPose.boneLclTransforms[i], translation1, rotation1, scale1);
			Decompose(mBlendToPose.boneLclTransforms[i], translation2, rotation2, scale2);
			Vec4f translation = LinearInterpolate(translation1, translation2, alpha);
			Quaternion4f rotation = SLerp(rotation1, rotation2, alpha);
			Vec4f scale = LinearInterpolate(scale1, scale2, alpha);
			Matrix4x4f transformation = Compose(translation, rotation, scale);
			boneTransforms[i] = transformation;
		}
		AnimPose result = mBlendFromPose;
		result.boneLclTransforms = boneTransforms;
		return result;
	}
}