#include "IE_PCH.h"


#include "Animation.h"
#include "AnimationController.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"

#include "RunTime/Resource/ResourceManager.h"
#include "ECS/Components/AnimationComponent/AnimStateMachine.h"
#include "ECS/Components/AnimationComponent/EventAnimPlaySetting.h"
#include "Animation.h"
#include "EventAnimController.h"
#include "IkController.h"
#include "MotionMatching/MotionMatchingController.h"


namespace inceptionengine
{

	AnimationController::AnimationController()
	{
		mEventAnimController = std::make_unique<EventAnimController>(*this);
		mIkController = std::make_unique<IkController>(*this);
		mMotionMatchingController = std::make_unique<MotionMatchingController>();
	}

	AnimationController::~AnimationController()
	{

	}

	void AnimationController::Initialize(std::shared_ptr<Skeleton const> skeleton)
	{
		mSkeleton = skeleton;
		mFinalPose = skeleton->GetLocalRefPose();
	}

	MatchingFeature GenerateFeature()
	{
		//float speed = 1.0f;
		//float time = 1.0f / 30.0f;
		MatchingFeature f;
		f.trajectory[0] = { 0.0f, 0.0f, 40.0f };
		f.trajectory[1] = { 0.0f, 0.0f, 80.0f };
		f.trajectory[2] = { 0.0f, 0.0f, 120.0f };
		f.facingDirection[0] = { 0.0f,0.0f,1.0f };
		f.facingDirection[1] = { 0.0f,0.0f,1.0f };
		f.facingDirection[2] = { 0.0f,0.0f,1.0f };

		
		
		//f.leftFootPosition;
		//f.rightFootPosition;

		//f.hipVelocity;
		//f.leftFootVelocity;
		//f.rightFootVelocity;

		return f;
	}

#define USE_MOTIONMATCHING 1

	bool AnimationController::Update(float deltaTime)
	{
		if (mStopAnim) return true;

#if USE_MOTIONMATCHING
		mFeatureQueryTimer += deltaTime;
		if (mFeatureQueryTimer >= mFeatureQueryInterval)
		{
			mFeatureQueryTimer = 0.0f;
			MatchingFeature f = GenerateFeature();
			std::vector<Matrix4x4f> globalTransform = Animation::GetBonesGlobalTransforms(mFinalPose, mSkeleton);
			Vec3f currentPosition = globalTransform[0][3];
			f.leftFootPosition = Vec3f(globalTransform[mSkeleton->GetBoneID("Model:LeftFoot")][3]) - currentPosition;
			f.rightFootPosition = Vec3f(globalTransform[mSkeleton->GetBoneID("Model:RightFoot")][3]) - currentPosition;
			f.leftFootVelocity = mBoneVelocities[mSkeleton->GetBoneID("Model:LeftFoot")];
			f.rightFootVelocity = mBoneVelocities[mSkeleton->GetBoneID("Model:RightFoot")];
			f.hipVelocity = mBoneVelocities[mSkeleton->GetBoneID("Model:Hips")];
			/*NEED VEL: in generating feature, need current vel*/

			mMotionMatchingController->Update(deltaTime, f);
			mFinalPose = mMotionMatchingController->GetCurrentPose();
			mFinalPose[0][3].x = mFinalPose[0][3].z = 0.0f;
			mBoneVelocities = mMotionMatchingController->GetCurrentBoneVelocities();
		}
		else
		{
			mMotionMatchingController->Update(deltaTime);
			mFinalPose = mMotionMatchingController->GetCurrentPose();
			mFinalPose[0][3].x = mFinalPose[0][3].z = 0.0f;
			mBoneVelocities = mMotionMatchingController->GetCurrentBoneVelocities();
		}

		return true;
#else
		if (mBlender.IsBlending())
		{
			auto blendedPose = mBlender.Blend(deltaTime);
			if (blendedPose.has_value())
			{
				mFinalPose = blendedPose.value();
			}
			else
			{
				//stop blending
			}
		}
		else if (mEventAnimController->IsPlayingAnimation())
		{
			mEventAnimController->Update(deltaTime);
			mFinalPose = mEventAnimController->GetCurrentPose();
		}
		else if (mAnimStateMachine != nullptr)
		{
			mAnimStateMachine->Update(deltaTime);
			mFinalPose = mAnimStateMachine->mFinalPose;
		}


		if (mAimIkBlender.first.IsBlending())
		{
			std::vector<Matrix4x4f> ikPose = mAimIkBlender.first.Blend(deltaTime, false).value();
			std::vector<int> const& aimIkMask = mIkController->GetAimIkChainBoneIDs();
			for (int i = 0; i < ikPose.size(); i++)
			{
				mFinalPose[aimIkMask[i]] = ikPose[i];
			}
		}
		else if (mAimIkBlender.second.IsBlending())
		{
			std::vector<Matrix4x4f> currentIkChainPose = mIkController->GetAimIkChainCurrentLclTransform();
			auto ikPoseOpt = mAimIkBlender.second.Blend(currentIkChainPose, deltaTime);
			if (ikPoseOpt.has_value())
			{
				std::vector<int> const& aimIkMask = mIkController->GetAimIkChainBoneIDs();
				for (int i = 0; i < ikPoseOpt.value().size(); i++)
				{
					mFinalPose[aimIkMask[i]] = ikPoseOpt.value()[i];
				}
			}
			//else the blend finish callback will be called to indicate IkController that aim ik is deactivated 
			
		}

		return true;
#endif

	}

	void AnimationController::PlayEventAnimation(EventAnimPlaySetting const& setting)
	{
		/*
		if (!mEventAnimController->IsPlayingAnimation())
		{
			mAnimStateMachine->BlendOutOfASM();

			//start a blending from ASM to EventAnim
			mBlender.StartBlending(mAnimStateMachine->mFinalPose,
								   gResourceMgr.GetResource<Animation>(setting.animFilePath)->Interpolate(0.0f),
								   setting.blendInDuration);
		}*/

		//if (mBlender.IsBlending()) mBlender.InterruptBlending();

		mAnimStateMachine->BlendOutOfASM();

		if (mBlender.IsBlending()) mBlender.InterruptBlending();

		mBlender.StartBlending(mFinalPose,
							   gResourceMgr.GetResource<Animation>(setting.animFilePath)->Interpolate(0.0f),
							   setting.blendInDuration);

		mEventAnimController->PlayEventAnimation(setting);
	}

	void AnimationController::StopAnimation()
	{
		mStopAnim = true;
	}

	bool AnimationController::IsPlayingEventAnimation() const
	{
		return mEventAnimController->IsPlayingAnimation();
	}

	void AnimationController::StartAnimStateMachine()
	{
		if (mAnimStateMachine != nullptr)
		{
			mAnimStateMachine->mCurrentState = mAnimStateMachine->mEntryState;
			mAnimStateMachine->mStates[mAnimStateMachine->mCurrentState].enterCallback();
		}
	}

	void AnimationController::EventAnimFinish(float blendOutDuration, std::function<void()> blendFinishCallback)
	{
		mAnimStateMachine->Restart();
		//start a blending from EventAnim to ASM
		mBlender.StartBlending(mFinalPose,
							   mAnimStateMachine->mFinalPose,
							   blendOutDuration,
							   AnimBlendType::Linear,
							   blendFinishCallback);
	}

	//in the coordinate of the model, not parent bone
	Matrix4x4f AnimationController::GetSocketRefTransformation(std::string const& socket)
	{
		int socketID = mSkeleton->mSocketToIndexMap.at(socket);
		int parentID = mSkeleton->mSockets[socketID].parentID;
		return GetBoneModelTransform(mFinalPose, parentID) * mSkeleton->mSockets[socketID].lclTransform;
	}

	float AnimationController::GetCurrentEventAnimTime() const
	{
		return mEventAnimController->GetCurrentEventAnimTime();
	}

	float AnimationController::GetCurrentEventAnimDuration() const
	{
		return mEventAnimController->GetCurrentEventAnimDuration();
	}

	void AnimationController::InsertEventAnimSpeedRange(float startRatio, float endRatio, float playSpeed)
	{
		mEventAnimController->InsertAnimSpeedRange(startRatio, endRatio, playSpeed);
	}

	void AnimationController::TestAimAxis()
	{
		/*
		int headID = mSkeleton->GetBoneID("Bip001 Head");
		Matrix4x4f headGlobalTransform = GetBoneModelTransform(headID);
		Vec3f axis = headGlobalTransform[1];
		Matrix4x4f rot = Rotate(glm::radians(30.0f), axis);

		Vec4f headPosition = headGlobalTransform[3];
		headGlobalTransform[3] = { 0.0f,0.0f,0.0f,1.0f };
		headGlobalTransform = rot * headGlobalTransform;
		headGlobalTransform[3] = headPosition;
		Matrix4x4f headLclTransform = Inverse(GetBoneModelTransform(mSkeleton->mBones[headID].parentID)) * headGlobalTransform;
		mFinalPose[headID] = headLclTransform;*/

		/*
		int headID = mSkeleton->GetBoneID("Bip001 Head");
		Matrix4x4f headGlobalTransform = GetBoneModelTransform(headID);
		Vec4f headToAim = NormalizeVec(mAimPosition - headGlobalTransform[3]);
		Vec3f poleNormal = CrossProduct(Vec3f(0.0f, 1.0f, 0.0f), headToAim);
		float dotResult = DotProduct(headGlobalTransform[0], 100.0f * poleNormal);
		assert(dotResult <= 0.001f);
		std::cout << "assertion success!\n";
		std::cout << "Upward vector of head is " << glm::to_string(headGlobalTransform[0]) << std::endl;
		std::cout<<"Aim direction is " << glm::to_string(headToAim) << std::endl;
		std::cout<<"Pole normal is " << glm::to_string(poleNormal) << std::endl;
		std::cout << "dot result is" << dotResult << std::endl;*/
	}

	
	void AnimationController::SetAimIkChain(std::vector<std::string> const& boneNames, std::vector<float> const& weights)
	{
		std::vector<int> boneIDs;
		for (auto const& name : boneNames)
		{
			boneIDs.push_back(mSkeleton->GetBoneID(name));
		}

		mIkController->SetAimIkChain(boneNames, boneIDs, weights);
	}

	void AnimationController::ActivateAimIk()
	{
		mIkController->ActivateAimIk();
	}

	void AnimationController::DeactivateAimIk(float blendOutDuration)
	{
		//cancel blending to ik pose if it is occuring

		if (mAimIkBlender.first.IsBlending()) mAimIkBlender.first.InterruptBlending();

		std::vector<Matrix4x4f> fromPose = mIkController->GetAimIkChainCurrentLclTransform();
		mIkController->DeactivateAimIk();
		mAimIkBlender.second.StartBlending(fromPose, std::vector<Matrix4x4f>(), blendOutDuration);

	}

	

	void AnimationController::ChainAimToInDuration(Matrix4x4f modelTransform, Vec3f const& targetPosition, Vec3f const& eyeOffsetInHeadCoord, float duration)
	{
		if (mAimIkBlender.second.IsBlending()) mAimIkBlender.second.InterruptBlending();

		std::vector<Matrix4x4f> chainCurrentPose = mIkController->GetAimIkChainCurrentLclTransform();
		std::vector<Matrix4x4f> chainSolvedIkPose = mIkController->ChainAimTo(modelTransform, targetPosition, eyeOffsetInHeadCoord);
		mIkController->ActivateAimIk();
		mAimIkBlender.first.StartBlending(chainCurrentPose, chainSolvedIkPose, duration);


		/*
		std::vector<Matrix4x4f> ikPose = mIkController->ChainAimTo(targetPosition, eyeOffsetInHeadCoord);
		std::vector<int> const& aimIkMask = mIkController->GetAimIkChainBoneIDs();
		for (int i = 0; i < ikPose.size(); i++)
		{
			mFinalPose[aimIkMask[i]] = ikPose[i];
		}*/
	}

	bool AnimationController::IsAimIkActive() const
	{
		return mIkController->IsAimIkActive();
	}

	int AnimationController::GetCurrentAsmActiveState() const
	{
		if (IsPlayingEventAnimation() || mAnimStateMachine->IsTransiting()) return -1;
		return mAnimStateMachine->mCurrentState;
	}

	float AnimationController::GetCurrentAsmActiveStateRunningSecond() const
	{
		if (GetCurrentAsmActiveState() == -1) return -1.0f;
		else
		{
			return mAnimStateMachine->mStates[GetCurrentAsmActiveState()].runningTime;
		}
	}

	void AnimationController::InsertAnimNotify(AnimNotify const& notify)
	{
		mEventAnimController->InsertAnimNotify(notify);
	}

	void AnimationController::SetMatchingDatabase(std::string const& filePath)
	{

		mMotionMatchingController->LoadMatchingDatabase(filePath);
		
	}


	

	Matrix4x4f AnimationController::GetBoneModelTransform(std::vector<Matrix4x4f> const& lclPose, int boneID) const
	{
		Matrix4x4f globalTransform = lclPose[boneID];
		auto const& bone = mSkeleton->mBones[boneID];
		int parentID = bone.parentID;
		while (parentID != -1)
		{
			globalTransform = lclPose[parentID] * globalTransform;
			parentID = mSkeleton->mBones[parentID].parentID;
		}
		return globalTransform;
	}

	Matrix4x4f AnimationController::GetBoneGlobalTransform(Matrix4x4f const& modelTransform, std::vector<Matrix4x4f> const& lclPose, int boneID) const
	{
		return modelTransform * GetBoneModelTransform(lclPose, boneID);
	}

	void AnimationController::TestAxis(IkChain const& ikChain)
	{
		int armID = ikChain.BoneIDs[1];
		mFinalPose[armID] = Translate(mFinalPose[armID][3]) * Rotate(PI / 6.0f, Vec3f(0.0f, 1.0f, 0.0f)) * Translate(-mFinalPose[armID][3]) * mFinalPose[armID];
		float x, y, z = 0;
		glm::extractEulerAngleXYZ(mFinalPose[armID] * Translate(-mFinalPose[armID][3]), x, y, z);
		std::cout << "x is " << x << " y is " << glm::degrees(y) << "z is " << z << std::endl;
	}

	bool AnimationController::IsBlendingOccuring() const
	{
		return mAnimStateMachine->IsTransiting() || mEventAnimController->IsBlending();
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
				currentChainPosition.push_back(GetBoneModelTransform(mFinalPose, boneId)[3]);
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
				Matrix4x4f currentBoneGlobalTransform = GetBoneModelTransform(mFinalPose, ikChain.BoneIDs[i]);
				Vec4f currentBonePosition = currentBoneGlobalTransform[3];
				Vec4f originalDirection = GetBoneModelTransform(mFinalPose, ikChain.BoneIDs[i + 1])[3] - currentBonePosition;
				Vec4f desiredDirection = desiredPosition[i + 1] - currentBonePosition;
				Matrix4x4f deltaRotation = FromToRotation(originalDirection, desiredDirection);
				Matrix4x4f translateToOrigin = Translate(-currentBonePosition);
				Matrix4x4f translateBack = Translate(currentBonePosition);
				Matrix4x4f newCurrentboneGlobalTransform = translateBack * deltaRotation * translateToOrigin * currentBoneGlobalTransform;
				int parentID = mSkeleton->mBones[ikChain.BoneIDs[i]].parentID;
				if (parentID == -1) mFinalPose[ikChain.BoneIDs[i]] = newCurrentboneGlobalTransform;
				else mFinalPose[ikChain.BoneIDs[i]] = Inverse(GetBoneModelTransform(mFinalPose, parentID)) * newCurrentboneGlobalTransform;
			}

			//apply joint constraint for shoulder bone, which is at index 0 in the ikChain
			Matrix4x4f shoulderLocalTransform = mFinalPose[ikChain.BoneIDs[0]];
			Vec4f shoulderTranslation = shoulderLocalTransform[3];
			shoulderLocalTransform[3] = { 0.0f,0.0f,0.0f,1.0f };
			Quaternion4f shoulderRotation = RotToQuat(shoulderLocalTransform);
			Vec3f rotationAxis = RotationAxis(shoulderRotation);
			float rotationAngle = RotationAngle(shoulderRotation);

			rotationAngle = std::fmodf(rotationAngle, 2 * PI);
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

			rotY = std::fmodf(rotY, 2 * PI);
			if (rotY > PI)
				rotY -= 2 * PI;
			if (rotY < -PI)
				rotY += 2 * PI;

			rotY = std::clamp(rotY, -PI / 4.0f, PI / 4.0f);
			/*switch (ikChain.ChainType)
			{
				case IkChain::IkChainType::RightArmHand: rotY = std::max(0.0f, rotY); break;
				case IkChain::IkChainType::LeftArmHand: rotY = std::min(0.0f, rotY); break; 
				default: throw std::runtime_error("");
			}*/
			
			//std::cout << "x is " << rotX << " y is " << glm::degrees(rotY) << "z is " << rotZ << std::endl;
			//mFinalPose[ikChain.BoneIDs[1]] =  Translate(mFinalPose[ikChain.BoneIDs[1]][3]) * Rotate(rotY, Vec3f(0.0f, 1.0f, 0.0f));


		}



		//std::cout << "x is " << rotX << " y is " << glm::degrees(rotY) << "z is " << rotZ << std::endl;
		
	}
}

