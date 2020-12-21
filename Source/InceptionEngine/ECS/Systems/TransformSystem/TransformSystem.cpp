#include "TransformSystem.h"
#include "ECS/Entity/EntityComponentPool.hpp"

namespace inceptionengine
{
	TransformSystem::TransformSystem(ComponentsPool& componentsPool)
		:SystemBase(componentsPool)
	{
	}

	Matrix4x4f TransformSystem::GetEntityWorldTransform(EntityID id) const
	{
		return mComponentsPool.get().GetComponent<TransformComponent>(id).GetWorldTransform();
	}

	void TransformSystem::Update(float dt)
	{
		auto& view = mComponentsPool.get().GetComponentPool<TransformComponent>()->GetComponentView();
		for (auto& component : view)
		{
			component.Update(dt);
		}
	}


}

