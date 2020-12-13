
#include "NativeScriptComponent.h"
#include "ECS/World.h"

namespace inceptionengine
{
	NativeScriptComponent::NativeScriptComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
	}

	Entity const& NativeScriptComponent::GetEntity()
	{
		return mWorld.get().GetEntity(mEntityID);
	}

	NativeScript* NativeScriptComponent::GetScript()
	{
		return mScript.get();
	}

}
