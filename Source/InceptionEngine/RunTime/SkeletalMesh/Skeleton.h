#pragma once

#include "Math/Math.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace inceptionengine
{
	class Skeleton
	{
	public:
		size_t GetBoneNumber() const { return mBones.size(); }
		int GetBoneID(std::string const& name) const { return mNameToIDMap.at(name); }

	private:
		friend class SkeletalMesh;
		
		struct Bone
		{
			std::string Name;
			int ID = -1;
			int ParentID = -1;
			Matrix4x4f BindPose;
			Matrix4x4f BindPoseInv;
		};

	private:
		std::string mName;
		std::unordered_map<std::string, int> mNameToIDMap;
		std::vector<Bone> mBones;

	};
}
