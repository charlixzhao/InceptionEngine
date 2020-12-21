
#include "TransformComponent.h"

#include "Serialization/Serializer.h"


namespace inceptionengine
{

	TransformComponent::TransformComponent() = default;


	Matrix4x4f TransformComponent::GetWorldTransform() const
	{
		//need to mutiply parent transform
		return Matrix4x4f(mLocalXAxis, mLocalYAxis, mLocalZAxis, mLocalPosition);
	}

	Matrix4x4f TransformComponent::GetLocalTransform() const
	{
		return Matrix4x4f(mLocalXAxis, mLocalYAxis, mLocalZAxis, mLocalPosition);
	}

	void TransformComponent::SetWorldTransform(Matrix4x4f const& t)
	{
		mLocalXAxis = t[0];
		mLocalYAxis = t[1];
		mLocalZAxis = t[2];
		mLocalPosition = t[3];
	}

	void TransformComponent::Rotate(Vec3f const& axis, float degree)
	{
		mLocalXAxis = inceptionengine::Rotate(mLocalXAxis, degree, axis);
		mLocalYAxis = inceptionengine::Rotate(mLocalYAxis, degree, axis);
		mLocalZAxis = inceptionengine::Rotate(mLocalZAxis, degree, axis);
	}

	void TransformComponent::RotateForwardVecTo(Vec3f const& direction)
	{
		Matrix4x4f rot = FromToRotation(mLocalZAxis, direction, mLocalYAxis);

		mLocalXAxis = rot * mLocalXAxis;
		assert(!std::isnan(mLocalXAxis[0]));
		mLocalYAxis = rot * mLocalYAxis;
		mLocalZAxis = rot * mLocalZAxis;

	}

	void TransformComponent::Update(float dt)
	{
		if (mRotationDuration > 0)
		{
			Matrix4x4f rot(1.0f);
			
			if (mRotationCountDown - dt < 0.0f)
			{
				//stop rotate
				rot = inceptionengine::Rotate(mRotationCountDown, mRotationAxis);
				mRotationDuration = 0.0f;
				mRotationCountDown = 0.0f;

			}
			else
			{
				//continue to rotate
				mRotationCountDown -= dt;
				float rads = (dt / mRotationDuration) * mRotatationRads;
				rot = inceptionengine::Rotate(rads, mRotationAxis);
			}

			mLocalXAxis = rot * mLocalXAxis;
			assert(!std::isnan(mLocalXAxis[0]));
			mLocalZAxis = rot * mLocalZAxis;
		}

	}

	/*
	Assume the rotation axis is local y
	*/
	void TransformComponent::RotateForwardVecToInDuration(Vec3f const& direction, float duration)
	{
		assert(DotProduct(mLocalYAxis, direction) < 0.0001f);
		mRotatationRads = RadsBetween(mLocalZAxis, direction);
		if (mRotatationRads < 0)
			mRotatationRads += 2 * PI;
		if (mRotatationRads > PI) 
			mRotatationRads =  2*PI - mRotatationRads;
		mRotationDuration = duration;
		mRotationCountDown = duration;
		mRotationToDirection = direction;
		mFowardVecBeginRotation = mLocalZAxis;
		mRightwardVecBeginRotation = mLocalXAxis;

		if (VecLength(direction - Vec3f(mLocalZAxis)) < 0.001f)
		{
			mRotationAxis = mLocalYAxis;
		}
		else if (VecLength(direction + Vec3f(mLocalZAxis)) < 0.001f)
		{
			mRotationAxis = -mLocalYAxis;
		}
		else
		{
			mRotationAxis = CrossProduct(NormalizeVec(Vec3f(mLocalZAxis)), NormalizeVec(direction));

		}
	}

}

