#pragma once

#include "Math/Math.h"
#include <vector>

namespace inceptionengine
{

	struct BoneTransform
	{
		Vec4f Translation;
		Quaternion4f Rotation;
		Vec4f Scale;
	};

	using AnimPose = std::vector<BoneTransform>;

}