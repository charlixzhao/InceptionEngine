#include "IE_PCH.h"

#include "Skeleton.h"

namespace inceptionengine
{

	std::vector<Matrix4x4f> Skeleton::GetGlobalBindPose() const
	{
		return std::vector<Matrix4x4f>();
	}

	std::vector<Matrix4x4f> Skeleton::GetLocalRefPose() const
	{
		std::vector<Matrix4x4f> refPose;
		for (auto const& bone : mBones)
		{
			refPose.push_back(bone.lclRefPose);
		}
		return refPose;
	}

	Skeleton::Bone const& Skeleton::GetBone(std::string const& boneName) const
	{
		return mBones[GetBoneID(boneName)];
	}



}
