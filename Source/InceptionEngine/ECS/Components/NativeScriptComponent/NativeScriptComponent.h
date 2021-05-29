#pragma once

#include "EngineGlobals/EngineApiPrefix.h"
#include "NativeScript.h"
#include "ECS/Entity/EntityID.h"
#ifndef __MSVCRT__
#include <memory>
#endif

namespace inceptionengine
{
	class Entity;
	class World;
	class windowHandler;

	class IE_API NativeScriptComponent
	{
	public:

		NativeScriptComponent(EntityID entityID, std::reference_wrapper<World> world);

		NativeScript* GetScript();

		template<typename T, typename ... Args>
		void SetScript(Args&& ... args)
		{
			static_assert(std::is_base_of_v<NativeScript, T>);
			mScript = std::make_unique<T>(mEntityID, mWorld, std::forward<Args>(args)...);
		}

		Entity const& GetEntity();

	private:
		friend class NativeScriptSystem;

		EntityID mEntityID;

		std::reference_wrapper<World> mWorld;

		std::unique_ptr<NativeScript> mScript = nullptr;

	};
}

