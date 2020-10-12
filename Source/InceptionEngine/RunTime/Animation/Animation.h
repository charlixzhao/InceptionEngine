
#pragma once

namespace inceptionengine
{
	class Skeleton;

	class Animation
	{
	public:
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mDuration, mName, mBoneTransforms);
		}


		std::vector<Matrix4x4f> Interpolate(float time) const;

		float GetDuration() const { return mDuration; }

	//private:
		//Skeleton* mSkeleton = nullptr;

		float mDuration = 0.0f;
		std::string mName;
		std::vector<std::vector<Matrix4x4f>> mBoneTransforms;

	};
}