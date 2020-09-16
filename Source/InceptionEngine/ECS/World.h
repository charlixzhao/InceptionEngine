
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "Entity/EntityID.h"
#include "Entity/EntityFriend.h"
#include "ECS/ComponentSystemTypeTraits.h"


namespace inceptionengine
{
	class Entity;
	class Renderer;
	class ComponentsPool;
	class PeripheralInput;

	class IE_API World
	{
	public:
		World(Renderer& renderer);

		~World();

		World(World const&) = delete;

		World& operator = (World const&) = delete;

		World(World&&) = delete;

		World& operator = (World&&) = delete;

		EntityID CreateEntity();

		Entity const& GetEntity(EntityID entityID);

		void DeleteEntity(EntityID entityID);

		bool CheckValidEntityID(EntityID entityID);

		void SetSkybox(std::array<std::string, 6> const& texturePaths);

	private:
		/*
		PIMPL idiom
		*/
		class WorldImpl;

		std::unique_ptr<WorldImpl> mWorldImpl;

		EntityFriend const mEntityFriend = {};

	private:
		/*
		For engine to run the world
		*/
		friend class InceptionEngine;

		void WorldStart();

		void Simulate(float deltaTime, PeripheralInput&& keyInputs);

		void WorldEnd();

	private:
		/*
		For entity to get system and components pool
		*/
		friend class Entity;

		ComponentsPool& GetComponentsPools();

		template<typename Component>
		typename SystemType<Component>::Type& GetSystem();

	};



}


