
#include "IE_PCH.h"

#include "TransformComponent.h"

#include "Serialization/Serializer.h"

#include "ECS/World.h"

#include "ECS/Entity/Entity.h"

#include "ECS/Components/AnimationComponent/AnimationComponent.h"


namespace inceptionengine
{

	TransformComponent::TransformComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
		;
	}


	Matrix4x4f TransformComponent::GetWorldTransform() const
	{
		Matrix4x4f parentGlobalTranform = Matrix4x4f(1.0f);
		//need to mutiply parent transform
		if (mAttachedToSocketEntityID != InvalidEntityID)
		{
			Entity const& attachedEntity = mWorld.get().GetEntity(mAttachedToSocketEntityID);
			parentGlobalTranform = attachedEntity.GetComponent<TransformComponent>().GetWorldTransform() * attachedEntity.GetComponent<AnimationComponent>().GetSocketRefTransformation(mAttachedToSocketName);
		}
		
		return parentGlobalTranform * GetLocalTransform();

	}

	Matrix4x4f TransformComponent::GetLocalTransform() const
	{
		return Matrix4x4f(mLocalXAxis, mLocalYAxis, mLocalZAxis, mLocalPosition) * Scale(mUniformScale, mUniformScale, mUniformScale);
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
			
			bool rotateFinish = false;
			if (mRotationCountDown - dt < 0.0f)
			{
				//stop rotate
				rot = inceptionengine::Rotate(mRotationCountDown, mRotationAxis);
				mRotationDuration = 0.0f;
				mRotationCountDown = 0.0f;
				rotateFinish = true;
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

			if (rotateFinish) 	mRotateFinishCallback();
		}

	}

	/*
	Assume the rotation axis is local y
	*/
	void TransformComponent::RotateForwardVecToInDuration(Vec3f const& direction, float duration, std::function<void()> finishCallback)
	{
		assert(DotProduct(mLocalYAxis, direction) < 0.0001f);
		mRotateFinishCallback = finishCallback;
		mRotatationRads = RadsBetween(mLocalZAxis, direction);
		mRotationAxis = CrossProduct(mLocalZAxis, direction);
		NormalizeRotation(mRotatationRads, mRotationAxis);
		if (std::abs(mRotatationRads) <= 0.001f || std::abs(mRotatationRads - PI / 2.0f) <= 0.001f)
			mRotationAxis = mLocalYAxis;


		mRotationDuration = duration;
		mRotationCountDown = duration;
		mRotationToDirection = direction;
		mFowardVecBeginRotation = mLocalZAxis;
		mRightwardVecBeginRotation = mLocalXAxis;

	}

	void TransformComponent::AttachToSocket(EntityID attachedToEntityID, std::string const& socketName)
	{
		mAttachedToSocketEntityID = attachedToEntityID;
		mAttachedToSocketName = socketName;
	}

	void TransformComponent::DetachFromSocket()
	{
		mAttachedToSocketEntityID = InvalidEntityID;
		mAttachedToSocketName = "";
	}

	Vec3f TransformComponent::GetGlobalForward() const
	{
		return GetWorldTransform()[2];
	}

	void TransformComponent::SetUniformScale(float scale)
	{
		mUniformScale = scale;
	}

	Vec3f TransformComponent::GetWorldPosition() const
	{
		return GetWorldTransform()[3];
	}

}

