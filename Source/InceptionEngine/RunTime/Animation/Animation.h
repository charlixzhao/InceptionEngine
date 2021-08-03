
#pragma once

namespace inceptionengine
{
	struct Skeleton;


	struct Animation
	{

		float mDuration = 0.0f;
		std::string mName;
		std::string mPathToSkeleton;
		std::vector<std::vector<Matrix4x4f>> mBoneTransforms;
		std::vector<std::vector<Vec3f>> mBoneVelocities;

		std::shared_ptr<Skeleton> mSkeleton;

		std::vector<Matrix4x4f> Interpolate(float time) const;

		float GetDuration() const { return mDuration; }

		static std::vector<Matrix4x4f> GetBonesGlobalTransforms(std::vector<Matrix4x4f> const& localTransforms, std::shared_ptr<Skeleton const> skeleton);

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mDuration, mName, mPathToSkeleton, mBoneTransforms, mBoneVelocities);
		}

	};
}