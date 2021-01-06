
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"
#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	class TransformSystem;
	class World;
	struct Socket;

	class IE_API TransformComponent
	{
	public:
		TransformComponent(EntityID entityID, std::reference_wrapper<World> world);

		Matrix4x4f GetWorldTransform() const;

		Matrix4x4f GetLocalTransform() const;

		void SetWorldTransform(Matrix4x4f const& t);

		void Rotate(Vec3f const& axis, float degree);

		void RotateForwardVecTo(Vec3f const& direction);

		void RotateForwardVecToInDuration(Vec3f const& direction, float duration);
		
		void AttachToSocket(EntityID attachedToEntityID, std::string const& socketName);

		void DetachFromSocket();

		Vec3f GetGlobalForward() const;


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

		std::reference_wrapper<World> mWorld;
		EntityID mEntityID = InvalidEntityID;

		EntityID mAttachedToSocketEntityID = InvalidEntityID;
		std::string mAttachedToSocketName;
	};
}
