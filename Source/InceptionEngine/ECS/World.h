
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "Entity/EntityID.h"
#include "Entity/EntityFriend.h"
#include "ECS/ComponentSystemTypeTraits.h"

#include "ECS/Components/RigidbodyComponent/SphereTraceResult.h"

#include "Math/Spatial.h"

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
		class WorldImpl;

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


