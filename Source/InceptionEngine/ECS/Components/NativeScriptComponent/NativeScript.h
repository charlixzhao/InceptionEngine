#pragma once

/*
This class is used in native script component, we write it here, but in the
Runtime, because user need to inherite the class to use native script component,
so this class will be exposed to the dll
*/

#include "EngineGlobals/EngineApiPrefix.h"

namespace inceptionengine
{
	class Entity;

	class IE_API NativeScript
	{
	public:
		NativeScript(Entity const& entity);

		virtual ~NativeScript() = default;

		void Begin();
		void Tick();
		void Destroy();

	protected:
		Entity const& mEntity;

	private:
		virtual void OnBegin();
		virtual void OnTick();
		virtual void OnDestroy();
		
	};
}
