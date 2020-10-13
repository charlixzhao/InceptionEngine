
#include "TransformComponent.h"

#include "Serialization/Serializer.h"


namespace inceptionengine
{

	TransformComponent::TransformComponent(TransformSystem& system, int x, int y)
		:mSystem(system), mX(x), mY(y)
	{
	}


}

