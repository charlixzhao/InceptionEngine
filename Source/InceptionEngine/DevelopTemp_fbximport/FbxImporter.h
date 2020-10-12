#pragma once

#include "RunTime/SkeletalMesh/SkeletalMesh.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "RunTime/Animation/Animation.h"

namespace inceptionengine::fbximport
{
	struct ImportScene
	{
		SkeletalMesh mesh;
		Animation animation;
	};

	void Import(std::string const& filePath, ImportScene& importScene);

}