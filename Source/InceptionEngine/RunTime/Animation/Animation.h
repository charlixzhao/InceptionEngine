
#pragma once

#include "AnimPose.h"

namespace inceptionengine
{
	struct Skeleton;


	struct Animation
	{

		float mDuration = 0.0f;
		std::string mName;
		std::string mPathToSkeleton;
		std::vector<std::vector<Matrix4x4f>> mBoneTransforms; //in lcl space

		std::vector<std::vector<Vec3f>> mBoneGlobalTranslVelocities;
		std::vector<std::vector<Vec3f>> mBoneLclTranslVelocities;
		std::vector<std::vector<Vec3f>> mBoneGlobalAngularVelocities; //represent in rotation vector form
		std::vector<std::vector<Vec3f>> mBoneLclAngularVelocities;

		std::shared_ptr<Skeleton> mSkeleton;

		AnimPose Interpolate(float time) const;

		float GetDuration() const { return mDuration; }

		static std::vector<Matrix4x4f> GetBonesGlobalTransforms(std::vector<Matrix4x4f> const& localTransforms, std::shared_ptr<Skeleton const> skeleton);

		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(mDuration, mName, mPathToSkeleton, mBoneTransforms, mBoneGlobalTranslVelocities,
				mBoneLclTranslVelocities, mBoneGlobalAngularVelocities, mBoneLclAngularVelocities);
		}

	};
}