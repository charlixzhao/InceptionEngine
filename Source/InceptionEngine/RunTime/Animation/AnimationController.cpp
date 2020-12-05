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

	void AnimationController::TestAxis(IkChain const& ikChain)
	{
		int armID = ikChain.BoneIDs[1];
		mFinalPose[armID] = Translate(mFinalPose[armID][3]) * Rotate(PI / 6.0f, Vec3f(0.0f, 1.0f, 0.0f)) * Translate(-mFinalPose[armID][3]) * mFinalPose[armID];
		float x, y, z = 0;
		glm::extractEulerAngleXYZ(mFinalPose[armID] * Translate(-mFinalPose[armID][3]), x, y, z);
		std::cout << "x is " << x << " y is " << glm::degrees(y) << "z is " << z << std::endl;
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
				break;

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
				Matrix4x4f translateToOrigin = Translate(-currentBonePosition);
				Matrix4x4f translateBack = Translate(currentBonePosition);
				Matrix4x4f newCurrentboneGlobalTransform = translateBack * deltaRotation * translateToOrigin * currentBoneGlobalTransform;
				int parentID = mSkeleton->mBones[ikChain.BoneIDs[i]].parentID;
				if (parentID == -1) mFinalPose[ikChain.BoneIDs[i]] = newCurrentboneGlobalTransform;
				else mFinalPose[ikChain.BoneIDs[i]] = Inverse(GetBoneGlobalTransform(parentID)) * newCurrentboneGlobalTransform;
			}

			//apply joint constraint for shoulder bone, which is at index 0 in the ikChain
			Matrix4x4f shoulderLocalTransform = mFinalPose[ikChain.BoneIDs[0]];
			Vec4f shoulderTranslation = shoulderLocalTransform[3];
			shoulderLocalTransform[3] = { 0.0f,0.0f,0.0f,1.0f };
			Quaternion4f shoulderRotation = RotToQuat(shoulderLocalTransform);
			Vec3f rotationAxis = RotationAxis(shoulderRotation);
			float rotationAngle = RotationAngle(shoulderRotation);

			rotationAngle = std::fmod(rotationAngle, 2 * PI);
			if (rotationAngle > PI)
				rotationAngle -= 2 * PI;
			if (rotationAngle < -PI)
				rotationAngle += 2 * PI;

			float constexpr limit = PI / 2.0f;
			if (std::abs(rotationAngle) > limit)
			{
				Matrix4x4f newRoatiton = Rotate(Sign(rotationAngle) * limit, rotationAxis);
				mFinalPose[ikChain.BoneIDs[0]] = Translate(Vec3f(shoulderTranslation)) * newRoatiton;
			}

			//apply hinge constraint for ankle bone, which is at index 1 in the ikChain
			float rotX, rotY, rotZ = 0;
			glm::extractEulerAngleXYZ(Translate(-mFinalPose[ikChain.BoneIDs[1]][3]) * mFinalPose[ikChain.BoneIDs[1]],
									  rotX, rotY, rotZ);
			switch (ikChain.ChainType)
			{
				case IkChain::IkChainType::RightArmHand: rotY = std::max(0.0f, rotY); break;
				case IkChain::IkChainType::LeftArmHand: rotY = std::min(0.0f, rotY); break;
				default: throw std::runtime_error("");
			}
			
			//std::cout << "x is " << rotX << " y is " << glm::degrees(rotY) << "z is " << rotZ << std::endl;
			mFinalPose[ikChain.BoneIDs[1]] =  Translate(mFinalPose[ikChain.BoneIDs[1]][3]) * Rotate(rotY, Vec3f(0.0f, 1.0f, 0.0f));


		}



		//std::cout << "x is " << rotX << " y is " << glm::degrees(rotY) << "z is " << rotZ << std::endl;
		
	}
}

