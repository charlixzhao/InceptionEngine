
#pragma once


#include "IWorld.h"

/*
include entity and component storage
*/
#include "Entity/Entity.h"
#include "Entity/EntityComponentPool.hpp"
#include "ECS/ComponentSystemTypeTraits.h"

/*
include component of the world
*/
#include "ECS/Components/SkyboxComponent/SkyboxComponent.h"

/*
include all systems
*/
#include "ECS/Systems/Systems.h"

/*
include other untilities
*/
#include "Timer.hpp"

#include <vector>
#include <queue>
#include <array>
#include <string>

namespace inceptionengine
{

	class World : public IWorld
	{
	public:
		World(Renderer& renderer);

		~World() = default;

		World(World const&) = delete;

		World& operator = (World const&) = delete;

		EntityID CreateEntity();

		Entity const& GetEntity(EntityID entityID);

		void DeleteEntity(EntityID entityID);

		bool CheckValidEntityID(EntityID entityID);

		void SetSkybox(std::array<std::string, 6> const& texturePaths);

	private:
		friend class InceptionEngine;
		void WorldStart();

		void Simulate(float deltaTime);

		void WorldEnd();

	private:
		/*
		All entities in the world
		*/
		friend class Entity;

		std::vector<Entity> mEntities;

		std::queue<EntityID> mDeletedEntities;

		template<typename CComponent>
		typename SystemType<CComponent>::Type& GetSystem()
		{
			if constexpr (std::is_same_v<CComponent, SkeletalMeshComponent>)
			{
				return mSkeletalMeshRenderSystem;
			}
			else if constexpr (std::is_same_v<CComponent, TransformComponent>)
			{
				return mTransformSystem;
			}
			else if constexpr (std::is_same_v<CComponent, NativeScriptComponent>)
			{
				return mNativeScriptSystem;
			}
			else if constexpr (std::is_same_v<CComponent, CameraComponent>)
			{
				return mCameraSystem;
			}
		}


	private:
		void SystemsStart();

		void SystemsEnd();

	private:
		/*
		Storage for all components of entities
		*/
		ComponentsPool mEntityComponentPool;

	private:
		/*
		All components of the world. These are components unique in world. Other example includes sunlight, world fog, 
		and others
		*/
		SkyboxComponent mSkybox;

	private:
		/*
		Systems controlling components of entites
		*/
		SkeletalMeshRenderSystem mSkeletalMeshRenderSystem;
		TransformSystem mTransformSystem;
		NativeScriptSystem mNativeScriptSystem;
		CameraSystem mCameraSystem;

	private:
		/*
		Systems controlling components of world
		*/
		SkyboxSystem mSkyboxSystem;

	};



}


