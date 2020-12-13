
#include "Entity.h"

#include "ECS/World.h"

#include "ECS/Entity/EntityComponentPool.hpp"

namespace inceptionengine
{
	Entity::Entity(EntityID entityID, World& world, EntityFriend const & entityFriend)
		:mID(entityID), mWorld(world)
	{
	}

	Entity::Entity(Entity&& other)
		:mWorld(other.mWorld), mID(other.mID)
	{
		other.mID = Entity::InvalidID;
	}

	Entity& Entity::operator=(Entity&& other)
	{
		mID = other.mID;
		mWorld = other.mWorld;
		other.mID = Entity::InvalidID;
		return *this;
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

	World& Entity::GetWorld() const
	{
		return mWorld.get();
	}

	template<CComponent Component>
	Component& Entity::GetComponent() const
	{
		return mWorld.get().GetComponentsPools().GetComponent<Component>(mID);
	}

	template<CComponent Component, typename ...Arg>
	Component& Entity::AddComponent(Arg && ...args) const
	{
		if constexpr (std::is_same_v<Component, NativeScriptComponent> || 
					  std::is_same_v<Component, SkeletalMeshComponent> ||
					  std::is_same_v<Component, AnimationComponent>)
		{
			return mWorld.get().GetComponentsPools().AddComponent<Component>(mID, mID, mWorld);
		}
		else
		{
			return mWorld.get().GetComponentsPools().AddComponent<Component>(mID, std::forward<Arg>(args)...);
		}
	}

	template<CComponent Component>
	bool Entity::HasComponent() const
	{
		return mWorld.get().GetComponentsPools().EntityHasComponent<Component>(mID);
	}

	template<CComponent Component>
	void Entity::DeleteComponent() const
	{
		mWorld.get().GetComponentsPools().DeleteComponent<Component>(mID);
	}


	/*
	AddComponent instantiation 
	*/
	template IE_API TransformComponent& Entity::AddComponent() const;
	template IE_API SkeletalMeshComponent& Entity::AddComponent() const;
	template IE_API NativeScriptComponent& Entity::AddComponent() const;
	template IE_API CameraComponent& Entity::AddComponent() const;
	template IE_API AnimationComponent& Entity::AddComponent() const;

	/*
	GetComponent instantiation
	*/
	template IE_API TransformComponent& Entity::GetComponent() const;
	template IE_API SkeletalMeshComponent& Entity::GetComponent() const;
	template IE_API NativeScriptComponent& Entity::GetComponent() const;
	template IE_API CameraComponent& Entity::GetComponent() const;
	template IE_API AnimationComponent& Entity::GetComponent() const;
	/*
	HasComponent instantiation
	*/
	template IE_API bool Entity::HasComponent<TransformComponent>() const;
	template IE_API bool Entity::HasComponent<SkeletalMeshComponent>() const;
	template IE_API bool Entity::HasComponent<NativeScriptComponent>() const;
	template IE_API bool Entity::HasComponent<CameraComponent>() const;
	template IE_API bool Entity::HasComponent<AnimationComponent>() const;

	/*
	DeleteComponent instantiation
	*/
	template IE_API void Entity::DeleteComponent<TransformComponent>() const;
	template IE_API void Entity::DeleteComponent<SkeletalMeshComponent>() const;
	template IE_API void Entity::DeleteComponent<NativeScriptComponent>() const;
	template IE_API void Entity::DeleteComponent<CameraComponent>() const;
	template IE_API void Entity::DeleteComponent<AnimationComponent>() const;
}

