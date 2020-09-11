#pragma once

#include "EngineGlobals/EngineApiPrefix.h"
#include "NativeScript.h"

#include <memory>
#include <type_traits>
namespace inceptionengine
{
	class Entity;

	class NativeScriptSystem;

	class IE_API NativeScriptComponent
	{
	public:
		NativeScriptComponent(NativeScriptSystem& system, Entity const& entity);

		template<typename T>
		void SetScript()
		{
			static_assert(std::is_base_of_v<NativeScript, T>);
			mScript = std::make_unique<T>(mEntity);
		}

	private:
		friend class NativeScriptSystem;

		NativeScriptSystem& mSystem;

		Entity const& mEntity;

		std::unique_ptr<NativeScript> mScript = nullptr;

	};
}
