#pragma once

#include "SkeletalMesh.h"
#include "../Animation/AnimPose.h"

#include <memory>

namespace inceptionengine
{
	struct SkeletalMeshInstance
	{
		std::shared_ptr<SkeletalMesh const> mSkeletalMesh = nullptr;
		AnimPose mCurrentPose;
	};
}