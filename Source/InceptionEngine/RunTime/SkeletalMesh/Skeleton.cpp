#include "IE_PCH.h"

#include "Skeleton.h"

namespace inceptionengine
{

	std::vector<Matrix4x4f> Skeleton::GetBindPose()
	{
		std::vector<Matrix4x4f> bindPose;
		for (auto const& bone : mBones)
		{
			bindPose.push_back(bone.bindPose);
		}
		return bindPose;
	}



}
