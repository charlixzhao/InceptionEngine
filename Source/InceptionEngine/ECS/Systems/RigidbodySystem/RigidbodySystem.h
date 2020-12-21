#pragma once

#include "ECS/Systems/SystemBase.h"

namespace inceptionengine
{

	class RigidbodySystem : public SystemBase
	{
	public:
		RigidbodySystem(ComponentsPool& componentsPool);

		void Start();

		void Update(float deltaTime);

	private:

	};
}