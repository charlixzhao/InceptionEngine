
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
		std::shared_ptr<Skeleton> mSkeleton;


		std::vector<Matrix4x4f> Interpolate(float time) const;

		float GetDuration() const { return mDuration; }

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mDuration, mName, mPathToSkeleton, mBoneTransforms);
		}

	};
}