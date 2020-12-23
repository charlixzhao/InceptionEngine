#pragma once

namespace inceptionengine
{
	struct Skeleton
	{
	public:
		struct Bone
		{
			std::string name;
			int ID = -1;
			int parentID = -1;
			Matrix4x4f bindPose = {};
			Matrix4x4f bindPoseInv = {};
			Matrix4x4f lclRefPose = {};

			bool operator == (Bone const& other) const
			{
				return (name == other.name && ID == other.ID && parentID == other.parentID);//&& Distance2(bindPose, other.bindPose) < 0.00001 && Distance2(lclRefPose, other.lclRefPose) < 0.00001);
			}

			bool operator != (Bone const& other) const
			{
				return !(*this == other);
			}

			template<typename Archive>
			void serialize(Archive& archive)
			{
				archive(name, ID, parentID, bindPose, bindPoseInv, lclRefPose);
			}
		};

		struct Socket
		{
			int parentID = -1;
			std::string name;
			Matrix4x4f lclTransform = Matrix4x4f(1.0f);
		};

	public:
		std::string mName;
		std::unordered_map<std::string, int> mNameToIDMap;
		std::unordered_map<std::string, int> mSocketToIndexMap;
		std::vector<Bone> mBones;
		std::vector<Socket> mSockets;

		size_t GetBoneNumber() const { return mBones.size(); }

		int GetBoneID(std::string const& name) const 
		{ 
			return mNameToIDMap.find(name) == mNameToIDMap.end() ? -1 : mNameToIDMap.at(name); 
		}

		bool operator == (Skeleton const& other) const
		{
			if (mBones.size() != other.mBones.size()) return false;
			for (int i = 0; i < mBones.size(); i++)
			{
				if (mBones[i] != other.mBones[i])
				{
					return false;
				}
			}
			return true;
		}

		std::vector<Matrix4x4f> GetGlobalBindPose() const;

		std::vector<Matrix4x4f> GetLocalRefPose() const;

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mName, mNameToIDMap, mBones);
		}
		


	};
}
