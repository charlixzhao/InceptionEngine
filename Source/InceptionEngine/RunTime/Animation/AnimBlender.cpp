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

		if (blendingType == AnimBlendType::Inertialization)
		{
			ConstructInertializationParams(fromPose, toPose, blendingDuration);
		}
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
				return InertializedBlend(mCurrentBlendingTime);
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


	AnimPose AnimBlender::InertializedBlend(float t) const
	{
		AnimPose pose = mBlendFromPose;
		for (int i = 0; i < pose.Size(); i++)
		{

			Vec4f translation = AppendZero(mTranslParams[i].fromPos + mTranslParams[i].translVelDirection * mTranslParams[i].Evaluate(t));
			Quaternion4f rot = QuatFromAxisAngle(mRotParams[i].rotateAxis, mRotParams[i].Evaluate(t)) * mRotParams[i].fromRot;
			Vec4f scale = ExtractScale(mBlendFromPose.boneLclTransforms[i]);
			
			pose.boneLclTransforms[i] = Compose(translation, rot, scale);

			if (i == 1)
			{
				//printf("value is %f, with to_val %f\n", mRotParams[i].Evaluate(t), mRotParams[i].toVal_);
			}
		}

		return pose;
	}


	void AnimBlender::ConstructInertializationParams(AnimPose const& fromPose, AnimPose const& toPose, float t1)
	{
		mTranslParams.clear();
		mRotParams.clear();
		mTranslParams.resize(fromPose.Size());
		mRotParams.resize(toPose.Size());

		float maxBlendDuration = 0.0f;
		for (int i = 0; i < fromPose.Size(); i++)
		{
			{
				Vec3f fromPos = Vec3f(fromPose.boneLclTransforms[i][3]);
				Vec3f toPos = Vec3f(toPose.boneLclTransforms[i][3]);
				
				if (Distance(toPos, fromPos) > 0.00001f)
				{
					Vec3f translVel = fromPose.boneLclTranslVelocities[i];
					translVel = Project(translVel, toPos - fromPos);
					auto translParams = AnimBlender::InertializationParams(0.0f, Distance(fromPos, toPos),
						Sign(DotProduct(translVel, toPos - fromPos)) * VecLength(translVel), t1);
					translParams.translVelDirection = (Distance(toPos, fromPos) <= 0.00001f) ? Vec3f(0.0f, 0.0f, 0.0f) : NormalizeVec(toPos - fromPos);
					mTranslParams[i] = translParams;
				}

				mTranslParams[i].fromPos = fromPos;
				maxBlendDuration = std::max(mTranslParams[i].t1_, maxBlendDuration);
			}

			{
				Quaternion4f fromRot = ExtractRotation(fromPose.boneLclTransforms[i]);
				Quaternion4f toRot = ExtractRotation(toPose.boneLclTransforms[i]);
				Quaternion4f diff = QuatDiff(fromRot, toRot);
				//float angle = RotationAngle(diff);
				float angle = glm::angle(diff);
		
				if (angle > 0.00001f)
				{
					//Vec3f axis = RotationAxis(diff);
					Vec3f axis = glm::axis(diff);
					/*
					if (angle > PI)
					{
						angle = 2 * PI - angle;
						axis = -axis;
					}
					else if (angle < -PI)
					{
						angle = 2 * PI + angle;
						axis = -axis;
					}*/
					NormalizeRotation(angle, axis);
					Vec3f angularVelVec = fromPose.boneLclAngularVelocities[i];

					float angularVelMagn = VecLength(angularVelVec);
					if (angularVelMagn > 0.00001f)
					{
						Vec3f angularVelAxis = NormalizeVec(angularVelVec);
						angularVelMagn *= DotProduct(axis, angularVelAxis);
						//angularVelMagn = 2.0f * std::atanf((DotProduct(angularVelAxis, axis) / std::cosf(0.5f * angularVelMagn)));
						//std::cout << "angular vel is " << angularVelMagn << std::endl;
					}

					auto rotParams = AnimBlender::InertializationParams(0.0f, angle, angularVelMagn, t1);
					rotParams.rotateAxis = axis;
					mRotParams[i] = rotParams;
				}

				mRotParams[i].fromRot = fromRot;
				maxBlendDuration = std::max(mRotParams[i].t1_, maxBlendDuration);

			}

			mBlendingDuration = maxBlendDuration;
			//std::cout << "duration is " << mBlendingDuration << std::endl;
		}
	}



	AnimBlender::InertializationParams::InertializationParams(float fromVal, float toVal, float v0, float t1)
	{
		toVal_ = toVal;
		float x0 = fromVal - toVal;

		if (std::fabsf(x0) <= 0.00001f)
		{
			t1_ = 0.0f;
			return;
		}

		if (x0 * v0 > 0.0f)
			v0 = 0.0f;

		//if (v0 != 0.0f)
			//t1 = std::min(t1, -5.0f * x0 / v0);
				
		float a0 = (-8.0f * v0 * t1 - 20.0f * x0) / (std::powf(t1, 2));

		if (v0 * a0 > 0.0f)
			a0 = 0.0f;

		x0_ = x0;
		v0_ = v0;
		a0_ = a0;
		t1_ = t1;

		A_ = -(a0 * std::powf(t1, 2) + 6.0f * v0 * t1 + 12.0f * x0) / (2.0f * std::powf(t1, 5));
		B_ = (3.0f * a0 * std::powf(t1, 2) + 16.0f * v0 * t1 + 30.0f * x0) / (2.0f * std::powf(t1, 4));
		C_ = -(3.0f * a0 * std::powf(t1, 2) + 12.0f * v0 * t1 + 20.0f * x0) / (2.0f * std::powf(t1, 3));
	}


	float AnimBlender::InertializationParams::Evaluate(float t) const
	{
		if (t > t1_) return toVal_;
		return toVal_ + A_ * std::powf(t, 5) + B_ * std::powf(t, 4) + C_ * std::powf(t, 3)
			+ (a0_ / 2.0f) * std::powf(t, 2) + v0_ * t + x0_;
	}
}