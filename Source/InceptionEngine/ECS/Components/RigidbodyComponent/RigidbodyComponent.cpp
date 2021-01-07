#include "IE_PCH.h"
#include "RigidbodyComponent.h"
#include "RunTime/Collision/CapsuleCollider.h"

#include "ECS/Entity/Entity.h"
#include "ECS/World.h"
#include "ECS/Components/TransformComponent/TransformComponent.h"

namespace inceptionengine
{
	RigidbodyComponent::RigidbodyComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{

	}

	RigidbodyComponent::~RigidbodyComponent() = default;

	RigidbodyComponent::RigidbodyComponent(RigidbodyComponent&&) noexcept = default;

	std::pair<bool, bool> RigidbodyComponent::DetectCapsuleCollision(RigidbodyComponent const& other)
	{
		if (mCapsuleCollider == nullptr || other.mCapsuleCollider == nullptr)
		{
			return { false, false };
		}
		else
		{
			Matrix4x4f tranform1 = mWorld.get().GetEntity(mEntityID).GetComponent<TransformComponent>().GetWorldTransform();
			Vec3f bottom1 = tranform1 * Vec4f(mCapsuleCollider->bottom, 1.0f);
			Vec3f top1 = tranform1 * Vec4f(mCapsuleCollider->top, 1.0f);

			Matrix4x4f tranform2 = mWorld.get().GetEntity(other.mEntityID).GetComponent<TransformComponent>().GetWorldTransform();
			Vec3f bottom2 = tranform2 * Vec4f(other.mCapsuleCollider->bottom, 1.0f);
			Vec3f top2 = tranform2 * Vec4f(other.mCapsuleCollider->top, 1.0f);
			bool collisionOccur = CapsuleCollider::DetectCollision(bottom1, top1,
																   bottom2, top2,
																   mCapsuleCollider->radius, other.mCapsuleCollider->radius);
			bool blocking = mCapsuleCollider->blocking && other.mCapsuleCollider->blocking;
			return { collisionOccur, blocking };
		}

		
	}

	float RigidbodyComponent::GetSpeed() const
	{
		return VecLength(mVelocity);
	}
	Vec3f RigidbodyComponent::GetVelocity() const
	{
		return mVelocity;
	}
	Vec3f RigidbodyComponent::GetAngularVelocity() const
	{
		return mAngulerVelocity;
	}
	void RigidbodyComponent::SetVelocity(Vec3f const& v)
	{
		mVelocity = Vec4f(v[0], v[1], v[2], 0.0f);
	}
	void RigidbodyComponent::SetCollider(ColliderType colliderType)
	{
		if (colliderType == ColliderType::Capsule)
		{
			mCapsuleCollider = std::make_unique<CapsuleCollider>();
		}
	}
	void RigidbodyComponent::SetCapsuleColliderProperties(Vec3f const& bottom, Vec3f const& top, float radius)
	{
		assert(mCapsuleCollider != nullptr && "add collider first");
		assert(Distance(bottom, top) > radius && "length cannot be larger than radius");
		mCapsuleCollider->bottom = bottom;
		mCapsuleCollider->top = top;
		mCapsuleCollider->radius = radius;
	}
	void RigidbodyComponent::SetCapsuleColliderBlocking(bool blocking)
	{
		assert(mCapsuleCollider != nullptr);
		mCapsuleCollider->blocking = blocking;
	}

	float RigidbodyComponent::GetCapsuleRadius() const
	{
		return mCapsuleCollider->radius;
	}
}