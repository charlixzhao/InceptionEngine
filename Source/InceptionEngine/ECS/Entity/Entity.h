
#pragma once
#include "EngineGlobals/EngineApiPrefix.h"
#include "EngineGlobals/EngineConcepts.h"
#include "EntityID.h"
#include "EntityFriend.h"


namespace inceptionengine
{
	class World;

	class Entity;

	class IE_API Entity
	{
	public:
		static const EntityID InvalidID = std::numeric_limits<EntityID>::max();

		Entity() = delete;

		explicit Entity(EntityID entityID, World& world, EntityFriend const& entityFriend);

		Entity(Entity const&) = delete;

		Entity& operator = (Entity const&) = delete;

		Entity(Entity&& other);

		Entity& operator = (Entity&& other);
		
		bool operator == (Entity const& other);

		EntityID GetID() const;

		bool IsValid() const;

		World& GetWorld() const;

		template<CComponent Component, typename ... Arg>
		Component& AddComponent(Arg && ...args) const;

		template<CComponent Component>
		Component& GetComponent() const;

		template<CComponent Component>
		bool HasComponent() const;

		template<CComponent Component>
		void DeleteComponent() const;

	private:
		friend class World; //World can modify mID, which is used when the entity is going to be deleted

		EntityID mID = InvalidID;

		std::reference_wrapper<World> mWorld;
	};


	/*
	AddComponent declaration
	*/
	extern template IE_API TransformComponent& Entity::AddComponent(int const&, int const&) const;
	extern template IE_API SkeletalMeshComponent& Entity::AddComponent() const;
	extern template IE_API NativeScriptComponent& Entity::AddComponent() const;
	extern template IE_API CameraComponent& Entity::AddComponent() const;
	extern template IE_API AnimationComponent& Entity::AddComponent() const;
	extern template IE_API RigidbodyComponent& Entity::AddComponent() const;

	/*
	GetComponent declaration
	*/
	extern template IE_API TransformComponent& Entity::GetComponent() const;
	extern template IE_API SkeletalMeshComponent& Entity::GetComponent() const;
	extern template IE_API NativeScriptComponent& Entity::GetComponent() const;
	extern template IE_API CameraComponent& Entity::GetComponent() const;
	extern template IE_API AnimationComponent& Entity::GetComponent() const;
	extern template IE_API RigidbodyComponent& Entity::GetComponent() const;

	/*
	HasComponent declaration
	*/
	extern template IE_API bool Entity::HasComponent<TransformComponent>() const;
	extern template IE_API bool Entity::HasComponent<SkeletalMeshComponent>() const;
	extern template IE_API bool Entity::HasComponent<NativeScriptComponent>() const;
	extern template IE_API bool Entity::HasComponent<CameraComponent>() const;
	extern template IE_API bool Entity::HasComponent<AnimationComponent>() const;
	extern template IE_API bool Entity::HasComponent<RigidbodyComponent>() const;

	/*
	DeleteComponent declaration
	*/
	extern template IE_API void Entity::DeleteComponent<TransformComponent>() const;
	extern template IE_API void Entity::DeleteComponent<SkeletalMeshComponent>() const;
	extern template IE_API void Entity::DeleteComponent<NativeScriptComponent>() const;
	extern template IE_API void Entity::DeleteComponent<CameraComponent>() const;
	extern template IE_API void Entity::DeleteComponent<AnimationComponent>() const;
	extern template IE_API void Entity::DeleteComponent<RigidbodyComponent>() const;
}






