#include "IE_PCH.h"
#include "RigidbodySystem.h"

#include "ECS/Entity/EntityComponentPool.hpp"
#include "ECS/Components/RigidbodyComponent/RigidbodyComponent.h"
#include "ECS/Components/TransformComponent/TransformComponent.h"

#include "RunTime/Collision/CapsuleCollider.h"

namespace inceptionengine
{
	RigidbodySystem::RigidbodySystem(ComponentsPool& componentsPool)
		:SystemBase(componentsPool)
	{
	}
	void RigidbodySystem::Start()
	{
	}

	void RigidbodySystem::Update(float deltaTime)
	{
		auto const& map = mComponentsPool.get().GetComponentPool<RigidbodyComponent>()->GetEntityMap();
		auto& rigidbodyCompooentView = mComponentsPool.get().GetComponentPool<RigidbodyComponent>()->GetComponentView();

		for (auto const& pair : map)
		{
			EntityID entityID = pair.first;

			auto& rigidbodyComponent = rigidbodyCompooentView[pair.second];
			auto& transformComponent = mComponentsPool.get().GetComponent<TransformComponent>(entityID);
			Vec4f moveDirection = transformComponent.GetLocalTransform() * rigidbodyComponent.mVelocity;
			Vec4f originPosition = transformComponent.mLocalPosition;

			transformComponent.mLocalPosition += moveDirection * deltaTime;
			for (auto const& otherRigidbody : rigidbodyCompooentView)
			{
				if (rigidbodyComponent.mEntityID != otherRigidbody.mEntityID)
				{
					auto detection = rigidbodyComponent.DetectCapsuleCollision(otherRigidbody);
					if (detection.first && detection.second)
					{
						transformComponent.mLocalPosition = originPosition;
					}
				}
			}


			
		}
	}

	std::vector<SphereTraceResult> RigidbodySystem::SphereTrace(Vec3f const& bottom, Vec3f const& top, float radius)
	{
		std::vector<SphereTraceResult> result;

		auto& rigidbodyCompooentView = mComponentsPool.get().GetComponentPool<RigidbodyComponent>()->GetComponentView();
		for (auto const& rigidBody : rigidbodyCompooentView)
		{
			if (rigidBody.mCapsuleCollider != nullptr)
			{
				Matrix4x4f transform = mComponentsPool.get().GetComponent<TransformComponent>(rigidBody.mEntityID).GetWorldTransform();
				Vec3f rigidbodyBottom = transform * Vec4f(rigidBody.mCapsuleCollider->bottom, 1.0f);
				Vec3f rigidbodyTop = transform * Vec4f(rigidBody.mCapsuleCollider->top, 1.0f);
				float rigidbodyRadius = rigidBody.mCapsuleCollider->radius;
				if (CapsuleCollider::DetectCollision(bottom, top, rigidbodyBottom, rigidbodyTop, radius, rigidbodyRadius))
				{
					SphereTraceResult res;
					res.entityID = rigidBody.mEntityID;
					result.push_back(res);
				}
			}
		}

		return result;

	}
}