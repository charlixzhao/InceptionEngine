
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

namespace inceptionengine
{
	class TransformSystem;

	class IE_API TransformComponent
	{
	public:
		TransformComponent();

		Matrix4x4f GetWorldTransform() const;

		Matrix4x4f GetLocalTransform() const;

		void SetWorldTransform(Matrix4x4f const& t);

		void Rotate(Vec3f const& axis, float degree);

		void RotateForwardVecTo(Vec3f const& direction);

		void RotateForwardVecToInDuration(Vec3f const& direction, float duration);
		



	private:
		friend class TransformSystem;
		friend class RigidbodySystem;
		void Update(float dt);
		
	private:
		Vec4f mLocalXAxis = { 1.0f, 0.0f, 0.0f, 0.0f };
		Vec4f mLocalYAxis = { 0.0f, 1.0f, 0.0f, 0.0f };
		Vec4f mLocalZAxis = { 0.0f, 0.0f, 1.0f, 0.0f };
		Vec4f mLocalPosition = { 0.0f, 0.0f, 0.0f, 1.0f };

		float mRotationDuration = 0.0f;
		float mRotationCountDown = 0.0f;
		float mRotatationRads = 0.0f;
		Vec3f mRotationToDirection;
		Vec4f mFowardVecBeginRotation;
		Vec4f mRightwardVecBeginRotation;
		Vec3f mRotationAxis;

	};
}
