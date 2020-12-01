#include "IE_PCH.h"


#include "Animation.h"
#include "AnimationController.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"

#include "RunTime/Resource/ResourceManager.h"

namespace inceptionengine
{

	AnimationController::AnimationController()
	{
	}

	AnimationController::~AnimationController()
	{

	}

	void AnimationController::Initialize(std::shared_ptr<Skeleton const> skeleton)
	{
		mSkeleton = skeleton;
		mFinalPose = skeleton->GetLocalBindPose();
	}

	bool AnimationController::Update(float deltaTime)
	{
		if (mCurrentAnimation != nullptr)
		{
			mCurrentTime += deltaTime;
			mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->GetDuration());
			mFinalPose = mCurrentAnimation->Interpolate(mCurrentTime);
			return true;
		}
		return false;

	}

	void AnimationController::PlayAnimation(std::shared_ptr<Animation const> pAnimation)
	{
		std::cout << "animation start!\n";
		mCurrentAnimation = pAnimation;
		mCurrentTime = 0.0f;

	}
	void AnimationController::StopAnimation()
	{
		mFinalPose = mCurrentAnimation->mSkeleton->GetLocalBindPose();
		mCurrentAnimation = nullptr;
		mCurrentTime = 0.0f;
	}

	bool AnimationController::IsPlayingAnimation()
	{
		return mCurrentAnimation != nullptr;
	}

	Matrix4x4f AnimationController::GetBoneGlobalTransform(int boneID)
	{
		Matrix4x4f globalTransform = mFinalPose[boneID];
		auto const& bone = mSkeleton->mBones[boneID];
		int parentID = bone.parentID;
		while (parentID != -1)
		{
			globalTransform = mFinalPose[parentID] * globalTransform;
			parentID = mSkeleton->mBones[parentID].parentID;
		}
		return globalTransform;
	}

	void AnimationController::HandReachTarget(IkChain const& ikChain, Matrix4x4f const& endEffector)
	{
		/*
		Do FABIK here
		*/

		for (int iteration = 0; iteration < 20; iteration++)
		{

			Vec4f targetPosition = endEffector[3];
			std::vector<Vec4f> currentChainPosition;
			for (auto boneId : ikChain.BoneIDs)
			{
				currentChainPosition.push_back(GetBoneGlobalTransform(boneId)[3]);
			}
			Vec4f base = currentChainPosition[0];
			int chainSize = ikChain.Size();
			std::vector<float> boneLength;
			boneLength.push_back(0.0f);
			for (int i = 1; i < chainSize; i++)
			{
				boneLength.push_back(VecLength(currentChainPosition[i] - currentChainPosition[i - 1]));
			}

			if (VecLength(currentChainPosition[chainSize - 1] - targetPosition) < 1)
				return;

			std::vector<Vec4f> desiredPosition = currentChainPosition;

			//check threshold
			if (VecLength(currentChainPosition[chainSize - 1] - targetPosition) < 1)
				break;

			//backward iteration
			desiredPosition[chainSize - 1] = targetPosition;
			for (int i = chainSize - 1; i >= 1; i--)
			{
				Vec4f direction = NormalizeVec(desiredPosition[i - 1] - desiredPosition[i]);
				Vec4f offset = boneLength[i] * direction;
				desiredPosition[i - 1] = desiredPosition[i] + offset;
			}

			//forward iteration
			desiredPosition[0] = base;
			for (int i = 1; i < chainSize; i++)
			{
				Vec4f direction = NormalizeVec(desiredPosition[i] - desiredPosition[i - 1]);
				Vec4f offset = boneLength[i] * direction;
				desiredPosition[i] = desiredPosition[i - 1] + offset;
			}

			//align bones
			for (int i = 0; i < chainSize - 1; i++)
			{
				Matrix4x4f currentBoneGlobalTransform = GetBoneGlobalTransform(ikChain.BoneIDs[i]);
				Vec4f currentBonePosition = currentBoneGlobalTransform[3];
				Vec4f originalDirection = GetBoneGlobalTransform(ikChain.BoneIDs[i + 1])[3] - currentBonePosition;
				Vec4f desiredDirection = desiredPosition[i + 1] - currentBonePosition;
				Matrix4x4f deltaRotation = FromToRotation(originalDirection, desiredDirection);
				Matrix4x4f translateToOrigin = Translation(-currentBonePosition);
				Matrix4x4f translateBack = Translation(currentBonePosition);
				Matrix4x4f newCurrentboneGlobalTransform = translateBack * deltaRotation * translateToOrigin * currentBoneGlobalTransform;
				int parentID = mSkeleton->mBones[ikChain.BoneIDs[i]].parentID;
				if (parentID == -1) mFinalPose[ikChain.BoneIDs[i]] = newCurrentboneGlobalTransform;
				else mFinalPose[ikChain.BoneIDs[i]] = Inverse(GetBoneGlobalTransform(parentID)) * newCurrentboneGlobalTransform;

			}

			//apply joint constraint for shoulder bone, which at index 0 in the ikChain
			Matrix4x4f shoulderLocalTransform = mFinalPose[ikChain.BoneIDs[0]];
			Vec4f shoulderTranslation = shoulderLocalTransform[3];
			shoulderLocalTransform[3] = { 0.0f,0.0f,0.0f,1.0f };
			Quaternion4f shoulderRotation = glm::quat_cast(shoulderLocalTransform);
			Vec3f rotationAxis = NormalizeVec(Vec3f(shoulderRotation.x, shoulderRotation.y, shoulderRotation.z));
			float rotationAngle = 2 * glm::acos(shoulderRotation.w);

			rotationAngle = std::fmod(rotationAngle, 2 * PI);
			if (rotationAngle > PI)
				rotationAngle -= 2 * PI;
			if (rotationAngle < -PI)
				rotationAngle += 2 * PI;

			float constexpr limit = PI / 2.0f;

			if (std::abs(rotationAngle) > limit)
			{
				Matrix4x4f newRoatiton = glm::rotate(Sign(rotationAngle) * limit, rotationAxis);
				mFinalPose[ikChain.BoneIDs[0]] = glm::translate(Vec3f(shoulderTranslation)) * newRoatiton;
			}

		}

		







		
	}
}

