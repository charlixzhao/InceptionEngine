#pragma once

#include "ECS/Systems/SystemBase.h"
namespace inceptionengine
{

	class NativeScriptSystem : public SystemBase
	{
	public:
		NativeScriptSystem(ComponentsPool& componentsPool);

		void Start();

		void End();

	private:
	};
}