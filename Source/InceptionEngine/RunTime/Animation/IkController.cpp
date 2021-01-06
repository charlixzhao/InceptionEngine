#include "IE_PCH.h"
#include "IkController.h"
#include "AnimationController.h"

#include "RunTime/SkeletalMesh/Skeleton.h"

namespace inceptionengine
{
	IkController::IkController(std::reference_wrapper<AnimationController> controller)
		:mAnimationController(controller)
	{
	}
	IkController::~IkController() = default;

	void IkController::SetAimIkChain(std::vector<std::string> const& boneNames, std::vector<int> const& boneIDs, std::vector<float> const& weights)
	{

		mAimIkChain.boneNames = boneNames;
		mAimIkChain.boneID = boneIDs;
		mAimIkChain.weights = weights;

		//chain validation
		for (int i = 1; i < mAimIkChain.boneID.size(); i++)
		{
			assert(mAnimationController.get().mSkeleton->mBones[mAimIkChain.boneID[i]].parentID == mAimIkChain.boneID[i - 1]);
		}
	}

	std::vector<int> const& IkController::GetAimIkChainBoneIDs() const
	{
		return mAimIkChain.boneID;
	}

	std::vector<Matrix4x4f> IkController::GetAimIkChainCurrentLclTransform() const
	{
		std::vector<Matrix4x4f> result;
		for (int boneID : mAimIkChain.boneID)
		{
			result.push_back(mAnimationController.get().mFinalPose[boneID]);
		}
		return result;
	}

	void IkController::ActivateAimIk()
	{
		mAimIkActive = true;
	}

	void IkController::DeactivateAimIk()
	{
		mAimIkActive = false;
	}

	bool IkController::IsAimIkActive() const
	{
		return mAimIkActive;
	}


	std::vector<Matrix4x4f> IkController::ChainAimTo(Matrix4x4f modelTransform, Vec3f const& targetPosition, Vec3f const& eyeOffsetInHeadCoord) const
	{
	
		Matrix4x4f headGlobalTransform = mAnimationController.get().GetBoneGlobalTransform(modelTransform, mAnimationController.get().mFinalPose, mAimIkChain.boneID.back());

		Vec4f eyePosition = headGlobalTransform[3] +
			eyeOffsetInHeadCoord[0] * NormalizeVec(headGlobalTransform[0]) +
			eyeOffsetInHeadCoord[1] * NormalizeVec(headGlobalTransform[1]) +
			eyeOffsetInHeadCoord[2] * NormalizeVec(headGlobalTransform[2]);

		Vec4f currentForward = NormalizeVec(headGlobalTransform[1]);

		Vec4f eyeToTarget = NormalizeVec(Vec4f(targetPosition, 1.0f) - eyePosition);

		std::vector<Matrix4x4f> result;
		if (RadsBetween(currentForward, eyeToTarget) <= 0.1f)
		{
			for (int boneID : mAimIkChain.boneID)
			{
				result.push_back(mAnimationController.get().mFinalPose[boneID]);
			}
			return result;
		}

		std::vector<Matrix4x4f> currentPose = mAnimationController.get().mFinalPose;

		int chainSize = mAimIkChain.boneID.size();
		Vec4f eyeGlobalPosition = mAnimationController.get().GetBoneGlobalTransform(modelTransform, currentPose, mAimIkChain.boneID.back()) * Vec4f(eyeOffsetInHeadCoord, 1.0f);
		for (int i = 0; i < chainSize - 1; i++)
		{
			Matrix4x4f boneLclTransform = BoneAimTo(modelTransform,
													currentPose,
													mAimIkChain.boneID[i],
													targetPosition,
													AffineInverse(mAnimationController.get().GetBoneGlobalTransform(modelTransform, currentPose, mAimIkChain.boneID[i])) * eyeGlobalPosition,
													mAimIkChain.weights[i]);
			result.push_back(std::move(boneLclTransform));
		}

		Matrix4x4f headLclTrasnform = BoneAimTo(modelTransform,
												currentPose,
												mAimIkChain.boneID.back(),
												targetPosition,
												eyeOffsetInHeadCoord,
												1.0);
		result.push_back(headLclTrasnform);
		return result;
	}


	Matrix4x4f IkController::BoneAimTo(Matrix4x4f modelTransform, std::vector<Matrix4x4f>& currentPose, int boneID, Vec3f const& targetPosition, Vec3f const& offset, float weight) const
	{
		Matrix4x4f boneGlobalTransform = mAnimationController.get().GetBoneGlobalTransform(modelTransform, currentPose, boneID);

		Vec4f eyePosition = boneGlobalTransform[3] +
			offset[0] * NormalizeVec(boneGlobalTransform[0]) +
			offset[1] * NormalizeVec(boneGlobalTransform[1]) +
			offset[2] * NormalizeVec(boneGlobalTransform[2]);


		Vec4f currentForward = NormalizeVec(boneGlobalTransform[1]);
		Vec4f currentUp = NormalizeVec(boneGlobalTransform[0]);

		Vec4f eyeToTarget = NormalizeVec(Vec4f(targetPosition, 1.0f) - eyePosition);
		Matrix4x4f alignForward = FromToRotation(currentForward, eyeToTarget);
		Quaternion4f qalignForwardQuat(alignForward);
		float alignForwardRads = RotationAngle(qalignForwardQuat);
		Vec3f alignForwardAxis = RotationAxis(qalignForwardQuat);
		NormalizeRotation(alignForwardRads, alignForwardAxis);
		alignForward = Rotate(weight * alignForwardRads, alignForwardAxis);

		Vec4f rotatedUp = alignForward * currentUp;
		Vec3f refNormal = CrossProduct(rotatedUp, eyeToTarget);
		Vec3f poleNormal = CrossProduct(Vec3f(0.0f, 1.0f, 0.0f), eyeToTarget);
		float alignUpRads = RadsBetween(refNormal, poleNormal);
		Vec3f alignUpAxis = eyeToTarget;
		NormalizeRotation(alignUpRads, alignUpAxis);

		Matrix4x4f alignUp(1.0f);
		Matrix4x4f alignUp1 = Rotate(alignUpRads, alignUpAxis);
		Matrix4x4f alignUp2 = Rotate(alignUpRads, -alignUpAxis);
		if (std::abs(DotProduct(alignUp1 * rotatedUp, poleNormal)) <= 0.001f)
		{
			alignUp = alignUp1;
		}
		else if (std::abs(DotProduct(alignUp2 * rotatedUp, poleNormal)) <= 0.001f)
		{
			alignUp = alignUp2;
		}
		else
		{
			throw std::runtime_error("Impossible math result");
		}

		Matrix4x4f rot = alignUp * alignForward;

		Vec4f boneTranslation = boneGlobalTransform[3];
		boneGlobalTransform[3] = { 0.0f,0.0f,0.0f,1.0f };
		boneGlobalTransform = rot * boneGlobalTransform;
		boneGlobalTransform[3] = boneTranslation;
		int parentID = mAnimationController.get().mSkeleton->mBones[boneID].parentID;
		assert(parentID != -1);

		Matrix4x4f boneLclTransform = AffineInverse(mAnimationController.get().GetBoneGlobalTransform(modelTransform, currentPose, parentID)) * boneGlobalTransform;
		
		currentPose[boneID] = boneLclTransform;
		return boneLclTransform;
	}
}