#pragma once

/*
This class is used in native script component, we write it here, but in the
Runtime, because user need to inherite the class to use native script component,
so this class will be exposed to the dll
*/

#include "EngineGlobals/EngineApiPrefix.h"

#include "KeyInputTypes.h"

#include <memory>
#include <functional>

namespace inceptionengine
{
	class Entity;

	struct KeyInputCallbackRegistry;

	class IE_API NativeScript
	{
	public:
		NativeScript(std::reference_wrapper<Entity const> entity);

		virtual ~NativeScript();

		void BindKeyInputCallback(KeyInputTypes keyType, std::function<void(bool)> callback);

	protected:
		std::reference_wrapper<Entity const> mEntity;

	private:
		/*
		System actually calls OnBegin and callback in KeyInputCallbackRegistry
		*/
		friend class NativeScriptSystem;

		/*
		Component exposes a function "BindKeyInputCallback" that register key input callback
		*/
		friend class NativeScriptComponent;

		virtual void OnBegin() { ; }

		virtual void OnTick() { ; }

		virtual void OnDestroy() { ; }

		std::unique_ptr<KeyInputCallbackRegistry> mKeyInputCallbackRegistry;

	};

}
