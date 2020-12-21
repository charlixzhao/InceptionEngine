#include "IE_PCH.h"
#include "RigidbodySystem.h"

#include "ECS/Entity/EntityComponentPool.hpp"
#include "ECS/Components/RigidbodyComponent/RigidbodyComponent.h"
#include "ECS/Components/TransformComponent/TransformComponent.h"

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
			transformComponent.mLocalPosition += moveDirection * deltaTime;

			
		}
	}
}