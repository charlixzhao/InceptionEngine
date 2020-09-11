
#pragma once
#include "EngineGlobals/EngineApiPrefix.h"
#include "EngineGlobals/EngineConcepts.h"
#include "EntityID.h"

#include <vector>

namespace inceptionengine
{
	class World;

	class Entity;

	//using EntityID = std::vector<Entity>::size_type;

	/*
	This class is used to transfer friendship to the vector in World so that we don't
	need to define customed vector allocator. This friendship transfer makes sure that
	only World can instantiate Entity so that user will not be able to create Entity 
	by their own. All any creation must go through World::CreateEntity().
	*/
	class EntityFriend
	{
	private:
		friend class World;
	};

	class IE_API Entity
	{
	public:
		static const EntityID InvalidID = std::numeric_limits<EntityID>::max();

		Entity() = delete;

		explicit Entity(EntityID entityID, World * pWorld, EntityFriend entityFriend);

		Entity(Entity const&) = delete;

		Entity& operator = (Entity const&) = delete;

		Entity(Entity&&) = default;

		Entity& operator = (Entity&&) = default;
		
		bool operator == (Entity const& other);

		EntityID GetID() const;

		bool IsValid() const;

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

		World* mWorldPtr = nullptr;
	};


	/*
	Add component specialization
	*/
	template IE_API TransformComponent& Entity::AddComponent(int const&, int const&) const;
	template IE_API SkeletalMeshComponent& Entity::AddComponent() const;
	template IE_API NativeScriptComponent& Entity::AddComponent() const;
	template IE_API CameraComponent& Entity::AddComponent() const;

	/*
	Get Component specialization
	*/
	template IE_API TransformComponent& Entity::GetComponent() const;
	template IE_API SkeletalMeshComponent& Entity::GetComponent() const;
	template IE_API NativeScriptComponent& Entity::GetComponent() const;
	template IE_API CameraComponent& Entity::GetComponent() const;

	/*
	Has Component specialization
	*/
	template IE_API bool Entity::HasComponent<TransformComponent>() const;
	template IE_API bool Entity::HasComponent<SkeletalMeshComponent>() const;
	template IE_API bool Entity::HasComponent<NativeScriptComponent>() const;
	template IE_API bool Entity::HasComponent<CameraComponent>() const;

	/*
	Delete component specialization
	*/
	template IE_API void Entity::DeleteComponent<TransformComponent>() const;
	template IE_API void Entity::DeleteComponent<SkeletalMeshComponent>() const;
	template IE_API void Entity::DeleteComponent<NativeScriptComponent>() const;
	template IE_API void Entity::DeleteComponent<CameraComponent>() const;

}






