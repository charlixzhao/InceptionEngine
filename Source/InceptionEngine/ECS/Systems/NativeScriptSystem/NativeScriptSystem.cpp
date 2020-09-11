
#include "NativeScriptSystem.h"
#include "ECS/Entity/EntityComponentPool.hpp"
#include "ECS/Components/NativeScriptComponent/NativeScriptComponent.h"

namespace inceptionengine
{
	NativeScriptSystem::NativeScriptSystem(ComponentsPool& componentsPool)
		:SystemBase(componentsPool)
	{
	}
	void NativeScriptSystem::Start()
	{
		ComponentPool<NativeScriptComponent>* pool = mComponentsPool.GetComponentPool<NativeScriptComponent>();
		if (pool == nullptr)
		{
			throw std::runtime_error("");
		}
		auto& view = pool->GetComponentView();


		for (auto& component : view)
		{
			component.mScript->Begin();
		}
	}
	void NativeScriptSystem::End()
	{
	}
}

