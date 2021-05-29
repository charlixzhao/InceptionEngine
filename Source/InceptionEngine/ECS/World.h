
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "Entity/EntityID.h"
#include "Entity/EntityFriend.h"
#include "ECS/ComponentSystemTypeTraits.h"

#include "ECS/Components/RigidbodyComponent/SphereTraceResult.h"
//Patch by fhh.
#ifndef __MSVCRT__
#include "../RunTime/Collision/CapsuleCollider.h"
#include "../RunTime/Animation/AnimInstance.h"
#include "../RunTime/Animation/EventAnimController.h"
#include "../RunTime/Animation/IkController.h"
#include "../RunTime/SkeletalMesh/SkeletalMeshInstance.h"

#endif

/*
include entity and component storage
*/
#include "Entity/Entity.h"
#include "Entity/EntityComponentPool.hpp"

/*
include component of the world
*/
#include "ECS/Components/SkyboxComponent/SkyboxComponent.h"

/*
include all systems
*/
#include "ECS/Systems/Systems.h"



namespace inceptionengine
{
	class Entity;
	class Renderer;
	class ComponentsPool;
	class PeripheralInput;
	class CameraComponent;
	class WindowHandler;




	class IE_API World
	{
	public:
		World(Renderer& renderer);

		~World();

		World(World const&) = delete;

		World& operator = (World const&) = delete;

		World(World&&) = delete;

		World& operator = (World&&) = delete;

		Entity const& CreateEntity();

		Entity const& GetEntity(EntityID entityID);

		void DeleteEntity(EntityID entityID);

		bool CheckValidEntityID(EntityID entityID);

		void SetSkybox(std::array<std::string, 6> const& texturePaths);

		void SetGameCamera(CameraComponent const& camera);

		void DrawAnimationTest();

		std::vector<SphereTraceResult> SphereTrace(Vec3f const& bottom, Vec3f const& top, float radius);

	private:
		/*
		PIMPL idiom
		*/
        class WorldImpl
            {
            public:
                WorldImpl(Renderer& renderer, World& world, EntityFriend const& entityFriend);

                Entity const& CreateEntity();

                Entity const& GetEntity(EntityID entityID);

                void DeleteEntity(EntityID entityID);

                bool CheckValidEntityID(EntityID entityID);

                void WorldStart();

                void Simulate(float deltaTime, PeripheralInput keyInputs);

                void WorldEnd();

                void SetSkybox(std::array<std::string, 6> const& texturePaths);

                void SetGameCamera(CameraComponent const& camera);

                void DrawAnimationTest();

                ComponentsPool& GetComponentsPool();

                std::vector<SphereTraceResult> SphereTrace(Vec3f const& bottom, Vec3f const& top, float radius);


            private:
                void SystemsStart();

                void SystemsEnd();

            private:
                std::reference_wrapper<EntityFriend const> mEntityFriend;

            private:
                std::reference_wrapper<World> mWorld;

            private:
                friend class Entity;

                std::vector<Entity> mEntities;

                std::queue<EntityID> mDeletedEntities;

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
                TransformSystem mTransformSystem;
                CameraSystem mCameraSystem;
                SkeletalMeshRenderSystem mSkeletalMeshRenderSystem;
                AnimationSystem mAnimationSystem;
                NativeScriptSystem mNativeScriptSystem;
                RigidbodySystem mRigidbodySystem;


            private:
                /*
                Systems controlling components of world
                */
                SkyboxSystem mSkyboxSystem;
            };

                std::unique_ptr<WorldImpl> mWorldImpl = nullptr;

                EntityFriend const mEntityFriend = {};

            private:
                /*
                For engine to run the world
                */
                friend class InceptionEngine;

                void WorldStart();

                void Simulate(float deltaTime, PeripheralInput keyInputs);

                void WorldEnd();

            private:
                /*
                For entity to get system and components pool
                */
                friend class Entity;

                ComponentsPool& GetComponentsPools();

            };



}


