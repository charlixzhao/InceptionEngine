#pragma once

#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	struct SphereTraceResult
	{
		EntityID entityID = InvalidEntityID;
		Vec3f hitPosition = {};
	};
}

