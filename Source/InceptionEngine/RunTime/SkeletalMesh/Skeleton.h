#pragma once

namespace inceptionengine
{
	class Skeleton
	{
	public:
		struct Bone
		{
			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(name, ID, parentID, bindPose, bindPoseInv);
			}

			std::string name;
			int ID = -1;
			int parentID = -1;
			Matrix4x4f bindPose = {};
			Matrix4x4f bindPoseInv = {};
		};

	public:

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mName, mNameToIDMap, mBones);
		}


		size_t GetBoneNumber() const { return mBones.size(); }

		int GetBoneID(std::string const& name) const 
		{ 
			return mNameToIDMap.find(name) == mNameToIDMap.end() ? -1 : mNameToIDMap.at(name); 
		}

		std::vector<Matrix4x4f> GetBindPose();

	//private:
		friend class SkeletalMesh;
		
		std::string mName;
		std::unordered_map<std::string, int> mNameToIDMap;
		std::vector<Bone> mBones;

	};
}
