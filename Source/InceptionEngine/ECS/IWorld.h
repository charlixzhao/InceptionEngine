
#pragma once

#include "EngineGlobals/EngineApiPrefix.h"

#include "Entity/EntityID.h"

#include <array>
#include <string>

namespace inceptionengine
{
	class IE_API IWorld
	{
	public:
		IWorld() = default;

		virtual ~IWorld() = default;

		IWorld(IWorld const&) = delete;

		IWorld& operator = (IWorld const&) = delete;

		IWorld(IWorld&&) = delete;

		IWorld& operator = (IWorld&&) = delete;

		virtual EntityID CreateEntity() = 0;

		virtual Entity const& GetEntity(EntityID entityID) = 0;

		virtual void DeleteEntity(EntityID entityID) = 0;

		virtual bool CheckValidEntityID(EntityID entityID) = 0;

		virtual void SetSkybox(std::array<std::string, 6> const& texturePaths) = 0;

	private:
		friend class InceptionEngine;

		virtual void WorldStart() = 0;

		virtual void Simulate(float deltaTime) = 0;

		virtual void WorldEnd() = 0;

	};
}
