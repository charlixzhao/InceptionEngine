
#include "TransformComponent.h"

#include "Serialization/Serializer.h"


namespace inceptionengine
{

	TransformComponent::TransformComponent() = default;


	Matrix4x4f TransformComponent::GetWorldTransform() const
	{
		return mWorldTransform;
	}

}

