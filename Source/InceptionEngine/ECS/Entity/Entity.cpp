
#include "Entity.h"

#include "ECS/World.h"

namespace inceptionengine
{
	Entity::Entity(EntityID entityID, World* pWorld, EntityFriend entityFriend)
		:mID(entityID), mWorldPtr(pWorld)
	{
	}

	/*
	No two entities will have same mID by design
	*/
	bool Entity::operator==(Entity const& other)
	{
		assert(mID != other.mID);
		return false;
	}
	
	EntityID Entity::GetID() const
	{
		return mID;
	}

	bool Entity::IsValid() const
	{
		return mID != InvalidID;
	}

	template<CComponent Component>
	Component& Entity::GetComponent() const
	{
		return mWorldPtr->mEntityComponentPool.GetComponent<Component>(mID);
	}

	template<CComponent Component, typename ...Arg>
	Component& Entity::AddComponent(Arg && ...args) const
	{
		if constexpr (std::is_same_v<Component, NativeScriptComponent>)
		{
			return mWorldPtr->mEntityComponentPool.AddComponent<NativeScriptComponent>(mID, mWorldPtr->GetSystem<NativeScriptComponent>(), *this);
		}
		else
		{
			return mWorldPtr->mEntityComponentPool.AddComponent<Component>(mID, mWorldPtr->GetSystem<Component>(), std::forward<Arg>(args)...);
		}
	}

	template<CComponent Component>
	bool Entity::HasComponent() const
	{
		return mWorldPtr->mEntityComponentPool.EntityHasComponent<Component>(mID);
	}

	template<CComponent Component>
	void Entity::DeleteComponent() const
	{
		mWorldPtr->mEntityComponentPool.DeleteComponent<Component>(mID);
	}


}

