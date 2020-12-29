#pragma once

#include "ECS/Systems/SystemBase.h"
#include "ECS/Components/RigidbodyComponent/SphereTraceResult.h"

namespace inceptionengine
{

	class RigidbodySystem : public SystemBase
	{
	public:
		RigidbodySystem(ComponentsPool& componentsPool);

		void Start();

		void Update(float deltaTime);

		std::vector<SphereTraceResult> SphereTrace(Vec3f const& bottom, Vec3f const& top, float radius);

	private:

	};
}