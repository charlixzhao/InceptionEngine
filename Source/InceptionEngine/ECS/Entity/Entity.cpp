
#include "Entity.h"

#include "ECS/World.h"

namespace inceptionengine
{
	Entity::Entity(EntityID entityID, World* pWorld, EntityFriend entityFriend)
		:mID(entityID), mWorld(pWorld)
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
		return mWorld->mEntityComponentPool.GetComponent<Component>(mID);
	}

	template<CComponent Component, typename ...Arg>
	Component& Entity::AddComponent(Arg && ...args) const
	{
		if constexpr (std::is_same_v<Component, NativeScriptComponent>)
		{
			return mWorld->mEntityComponentPool.AddComponent<NativeScriptComponent>(mID, mWorld->GetSystem<NativeScriptComponent>(), *this);
		}
		else
		{
			return mWorld->mEntityComponentPool.AddComponent<Component>(mID, mWorld->GetSystem<Component>(), std::forward<Arg>(args)...);
		}
	}

	template<CComponent Component>
	bool Entity::HasComponent() const
	{
		return mWorld->mEntityComponentPool.EntityHasComponent<Component>(mID);
	}

	template<CComponent Component>
	void Entity::DeleteComponent() const
	{
		mWorld->mEntityComponentPool.DeleteComponent<Component>(mID);
	}


	/*
	AddComponent instantiation 
	*/
	template IE_API TransformComponent& Entity::AddComponent(int const&, int const&) const;
	template IE_API SkeletalMeshComponent& Entity::AddComponent() const;
	template IE_API NativeScriptComponent& Entity::AddComponent() const;
	template IE_API CameraComponent& Entity::AddComponent() const;

	/*
	GetComponent instantiation
	*/
	template IE_API TransformComponent& Entity::GetComponent() const;
	template IE_API SkeletalMeshComponent& Entity::GetComponent() const;
	template IE_API NativeScriptComponent& Entity::GetComponent() const;
	template IE_API CameraComponent& Entity::GetComponent() const;

	/*
	HasComponent instantiation
	*/
	template IE_API bool Entity::HasComponent<TransformComponent>() const;
	template IE_API bool Entity::HasComponent<SkeletalMeshComponent>() const;
	template IE_API bool Entity::HasComponent<NativeScriptComponent>() const;
	template IE_API bool Entity::HasComponent<CameraComponent>() const;

	/*
	DeleteComponent instantiation
	*/
	template IE_API void Entity::DeleteComponent<TransformComponent>() const;
	template IE_API void Entity::DeleteComponent<SkeletalMeshComponent>() const;
	template IE_API void Entity::DeleteComponent<NativeScriptComponent>() const;
	template IE_API void Entity::DeleteComponent<CameraComponent>() const;

}

