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
#include "RunTime/Rigidbody/Dynamics/KinematicsTree.h"


namespace inceptionengine
{

	AnimationController::AnimationController()
	{
		mEventAnimController = std::make_unique<EventAnimController>(*this);
		mIkController = std::make_unique<IkController>(*this);
		mMotionMatchingController = std::make_unique<MotionMatchingController>();
		mKinematicsTree = std::make_unique<dynamics::KinematicsTree>();
	}

	AnimationController::~AnimationController()
	{

	}

	void AnimationController::Initialize(std::shared_ptr<Skeleton const> skeleton)
	{
		mSkeleton = skeleton;
		mFinalPose.boneLclTransforms = skeleton->GetLocalRefPose();
		mKtState = dynamics::Vec::Zero(mKinematicsTree->N * 7);
		for (int i = 0; i < mKinematicsTree->N; i++) mKtState[4 * i] = 1.0f;

		
		float theta = -PI / 4.0f;
		mKtState[4] = std::cos(theta);
		mKtState[4 + 3] = std::sin(theta);
		

		mKinematicsTree->InitializeComputationBuffer();
	}


#define USE_MOTIONMATCHING 0
#define USE_RIGIDBODY 1

	bool AnimationController::Update(float deltaTime)
	{
		if (mStopAnim) return true;

#if USE_MOTIONMATCHING
		if (! mMotionMatchingController->IsBlending())
		{
			mFeatureQueryTimer += deltaTime;
		}

		if (mFeatureQueryTimer >= mFeatureQueryInterval)
		{
			int constexpr FacingAxis = 1;
			mFeatureQueryTimer = 0.0f;
			std::vector<Matrix4x4f> globalTransform = Animation::GetBonesGlobalTransforms(mFinalPose.boneLclTransforms, mSkeleton);
			Vec3f currentPosition = globalTransform[0][3];

			MatchingFeature f = mMotionMatchingController->GenerateFeatureTemp(NormalizeVec(ProjectToXZ(globalTransform[0][FacingAxis])));
			for (auto const& featureBone : mMotionMatchingController->GetFeatureBones())
			{
				f.featureBonePos.push_back(Vec3f(globalTransform[mSkeleton->GetBoneID(featureBone)][3]) - currentPosition);
				f.featureBoneVels.push_back(mBoneVelocities[mSkeleton->GetBoneID(featureBone)]);
			}
	
			f.currentFacing = globalTransform[0][FacingAxis];

			mMotionMatchingController->Update(deltaTime, f);
			mFinalPose = mMotionMatchingController->GetCurrentPose();
			mFinalPose.boneLclTransforms[0][3].x = mFinalPose.boneLclTransforms[0][3].z = 0.0f;
			mBoneVelocities = mMotionMatchingController->GetCurrentBoneVelocities();
		}
		else
		{
			mMotionMatchingController->Update(deltaTime);
			mFinalPose = mMotionMatchingController->GetCurrentPose();
			mFinalPose.boneLclTransforms[0][3].x = mFinalPose.boneLclTransforms[0][3].z = 0.0f;
			mBoneVelocities = mMotionMatchingController->GetCurrentBoneVelocities();
		}

		return true;

#elif USE_RIGIDBODY
		for (auto& e : mTimerEvents)
		{
			e.remainingTime -= deltaTime;
			if (e.remainingTime <= 0.0f) e.End();
		}
		for (auto it = mTimerEvents.begin(); it != mTimerEvents.end();)
		{
			if (it->remainingTime <= 0.0f) it = mTimerEvents.erase(it);
			else it++;
		}

		
		//RK4
		dynamics::Vec k1 = mKinematicsTree->f(mKtState, {}, mExtForces) * deltaTime;
		dynamics::Vec k2 = mKinematicsTree->f((mKtState + 0.5f*k1), {}, mExtForces) * deltaTime;
		dynamics::Vec k3 = mKinematicsTree->f((mKtState+0.5f*k2), {}, mExtForces) * deltaTime;
		dynamics::Vec k4 = mKinematicsTree->f((mKtState+k3), {}, mExtForces) * deltaTime;

		dynamics::Vec ds = (k1 + 2 * k2 + 2 * k3 + k4) / 6.0f;
		for (auto& d : ds)
		{
			if (std::abs(d) <= 1e-8) d = 0.0f;
		}

		mKtState += ds;


		/*
		//RK2
		dynamics::Vec k1 = mKinematicsTree->f(mKtState, {}, mExtForces) * deltaTime;
		dynamics::Vec k2 = mKinematicsTree->f((mKtState + 0.5f * k1), {}, mExtForces) * deltaTime;

		mKtState += k2;*/

		NormalizeKtState();


		for (int i = 1; i < mKinematicsTree->Joints.size(); i++)
		{
			Matrix4x4f rot(1.0f);
			Matrix4x4f translate(1.0f);
			auto R = dynamics::Joint::E(mKtState.block<4, 1>(i * 4, 0), mKinematicsTree->Joints[i].Type).transpose();
			auto T = 100.0f * mKinematicsTree->Geoms[i].second;
			{
				rot[0][0] = R.col(0)[0];
				rot[0][1] = R.col(0)[1];
				rot[0][2] = R.col(0)[2];
				rot[1][0] = R.col(1)[0];
				rot[1][1] = R.col(1)[1];
				rot[1][2] = R.col(1)[2];
				rot[2][0] = R.col(2)[0];
				rot[2][1] = R.col(2)[1];
				rot[2][2] = R.col(2)[2];
				translate[3][0] = T[0];
				translate[3][1] = T[1];
				translate[3][2] = T[2];
			}


			mFinalPose.boneLclTransforms[i - 1] = translate * rot;
		}
		return true;
#else
		if (mBlender.IsBlending())
		{
			auto blendedPose = mBlender.Blend(deltaTime);
			if (blendedPose.has_value())
			{
				mFinalPose.boneLclTransforms = blendedPose.value();
			}
			else
			{
				//stop blending
			}
		}
		else if (mEventAnimController->IsPlayingAnimation())
		{
			mEventAnimController->Update(deltaTime);
			mFinalPose.boneLclTransforms = mEventAnimController->GetCurrentPose();
		}
		else if (mAnimStateMachine != nullptr)
		{
			mAnimStateMachine->Update(deltaTime);
			mFinalPose.boneLclTransforms = mAnimStateMachine->mFinalPose.boneLclTransforms;
		}


		if (mAimIkBlender.first.IsBlending())
		{
			std::vector<Matrix4x4f> ikPose = mAimIkBlender.first.Blend(deltaTime, false).value();
			std::vector<int> const& aimIkMask = mIkController->GetAimIkChainBoneIDs();
			for (int i = 0; i < ikPose.size(); i++)
			{
				mFinalPose.boneLclTransforms[aimIkMask[i]] = ikPose[i];
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
					mFinalPose.boneLclTransforms[aimIkMask[i]] = ikPoseOpt.value()[i];
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
			mBlender.StartBlending(mAnimStateMachine->mFinalPose.boneLclTransforms,
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

	void AnimationController::FlipFlopStopAnimation()
	{
		mStopAnim = !mStopAnim;
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
		return GetBoneModelTransform(mFinalPose.boneLclTransforms, parentID) * mSkeleton->mSockets[socketID].lclTransform;
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
		mFinalPose.boneLclTransforms[headID] = headLclTransform;*/

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
		mAimIkBlender.first.StartBlending(AnimPose(chainCurrentPose), AnimPose(chainSolvedIkPose), duration);


		/*
		std::vector<Matrix4x4f> ikPose = mIkController->ChainAimTo(targetPosition, eyeOffsetInHeadCoord);
		std::vector<int> const& aimIkMask = mIkController->GetAimIkChainBoneIDs();
		for (int i = 0; i < ikPose.size(); i++)
		{
			mFinalPose.boneLclTransforms[aimIkMask[i]] = ikPose[i];
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

	void AnimationController::SetInputControl(Vec3f const& input)
	{
		mMotionMatchingController->SetInputControl(input);
	}

	
	
	void AnimationController::AddCuboidLink(float x, float y, float z, float r, int parent, Vec3f const& offset)
	{
		mKinematicsTree->AddCuboidLink(dynamics::JointType::Sperical, x / 100.0f, y / 100.0f, z / 100.0f, r / 100.0f, parent + 1, (dynamics::Vec3d() << offset[0], offset[1], offset[2]).finished() / 100.0f);
		if (mExtForces.size() == 0) mExtForces.push_back(dynamics::SpatialForce::Zero());
		mExtForces.push_back(dynamics::SpatialForce::Zero());
	}

	void AnimationController::ApplyExtForce(int bodyID, Vec3f const& force, Vec3f const& location, float time)
	{
		Vec3f n = CrossProduct(location, force);
		dynamics::SpatialForce f = (dynamics::SpatialForce() << n[0], n[1], n[2], force[0], force[1], force[2]).finished();
		mExtForces[bodyID] = f;
		TimerEvent e;
		e.remainingTime = time;

		e.End = [this, bodyID]()
		{
			mExtForces[bodyID] = dynamics::SpatialForce::Zero();
		};
		mTimerEvents.push_back(e);

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



	void AnimationController::NormalizeKtState()
	{
		for (int i = 0; i < mKinematicsTree->N; i++)
		{
			mKtState.block<4, 1>(i * 4, 0).normalize();
		}
	}

	dynamics::Vec AnimationController::NormalizeState(dynamics::Vec const& vec)
	{
		dynamics::Vec v = vec;
		for (int i = 0; i < mKinematicsTree->N; i++)
		{
			v.block<4, 1>(i * 4, 0).normalize();
		}
		return v;
	}

	void AnimationController::TestAxis(IkChain const& ikChain)
	{
		int armID = ikChain.BoneIDs[1];
		mFinalPose.boneLclTransforms[armID] = Translate(mFinalPose.boneLclTransforms[armID][3]) * Rotate(PI / 6.0f, Vec3f(0.0f, 1.0f, 0.0f)) * Translate(-mFinalPose.boneLclTransforms[armID][3]) * mFinalPose.boneLclTransforms[armID];
		float x, y, z = 0;
		glm::extractEulerAngleXYZ(mFinalPose.boneLclTransforms[armID] * Translate(-mFinalPose.boneLclTransforms[armID][3]), x, y, z);
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
				currentChainPosition.push_back(GetBoneModelTransform(mFinalPose.boneLclTransforms, boneId)[3]);
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
				Matrix4x4f currentBoneGlobalTransform = GetBoneModelTransform(mFinalPose.boneLclTransforms, ikChain.BoneIDs[i]);
				Vec4f currentBonePosition = currentBoneGlobalTransform[3];
				Vec4f originalDirection = GetBoneModelTransform(mFinalPose.boneLclTransforms, ikChain.BoneIDs[i + 1])[3] - currentBonePosition;
				Vec4f desiredDirection = desiredPosition[i + 1] - currentBonePosition;
				Matrix4x4f deltaRotation = FromToRotation(originalDirection, desiredDirection);
				Matrix4x4f translateToOrigin = Translate(-currentBonePosition);
				Matrix4x4f translateBack = Translate(currentBonePosition);
				Matrix4x4f newCurrentboneGlobalTransform = translateBack * deltaRotation * translateToOrigin * currentBoneGlobalTransform;
				int parentID = mSkeleton->mBones[ikChain.BoneIDs[i]].parentID;
				if (parentID == -1) mFinalPose.boneLclTransforms[ikChain.BoneIDs[i]] = newCurrentboneGlobalTransform;
				else mFinalPose.boneLclTransforms[ikChain.BoneIDs[i]] = Inverse(GetBoneModelTransform(mFinalPose.boneLclTransforms, parentID)) * newCurrentboneGlobalTransform;
			}

			//apply joint constraint for shoulder bone, which is at index 0 in the ikChain
			Matrix4x4f shoulderLocalTransform = mFinalPose.boneLclTransforms[ikChain.BoneIDs[0]];
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
				mFinalPose.boneLclTransforms[ikChain.BoneIDs[0]] = Translate(Vec3f(shoulderTranslation)) * newRoatiton;
			}

			//apply hinge constraint for ankle bone, which is at index 1 in the ikChain
			float rotX, rotY, rotZ = 0;
			glm::extractEulerAngleXYZ(Translate(-mFinalPose.boneLclTransforms[ikChain.BoneIDs[1]][3]) * mFinalPose.boneLclTransforms[ikChain.BoneIDs[1]],
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
			//mFinalPose.boneLclTransforms[ikChain.BoneIDs[1]] =  Translate(mFinalPose.boneLclTransforms[ikChain.BoneIDs[1]][3]) * Rotate(rotY, Vec3f(0.0f, 1.0f, 0.0f));


		}



		//std::cout << "x is " << rotX << " y is " << glm::degrees(rotY) << "z is " << rotZ << std::endl;
		
	}
}

