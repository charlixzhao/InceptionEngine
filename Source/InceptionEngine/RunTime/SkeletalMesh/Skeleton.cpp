#include "IE_PCH.h"

#include "Skeleton.h"

namespace inceptionengine
{

	std::vector<Matrix4x4f> Skeleton::GetGlobalBindPose() const
	{
		std::vector<Matrix4x4f> bindPose;
		for (auto const& bone : mBones)
		{
			bindPose.push_back(bone.bindPose);
		}
		return bindPose;
	}

	std::vector<Matrix4x4f> Skeleton::GetLocalBindPose() const
	{
		auto const& worldBindPose = GetGlobalBindPose();
		std::vector<Matrix4x4f> localBindPose;
		localBindPose.resize(worldBindPose.size());
		for (auto const& bone : mBones)
		{
			if (bone.parentID == -1)
			{
				localBindPose[bone.ID] = worldBindPose[bone.ID];
			}
			else
			{
				localBindPose[bone.ID] = Inverse(worldBindPose[bone.parentID]) * worldBindPose[bone.ID];
			}
		}
		return localBindPose;
	}



}
