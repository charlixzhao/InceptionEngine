
#include "IE_PCH.h"
#include "AnimationComponent.h"

#include "RunTime/Resource/ResourceManager.h"

#include "ECS/Entity/Entity.h"
#include "ECS/World.h"

#include "RunTime/Animation/Animation.h"
//#include "RunTime/Animation/AnimationController.h"

#include "RunTime/SkeletalMesh/SkeletalMesh.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"
#include "ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.h"

#include "ECS/Components/TransformComponent/TransformComponent.h"




namespace inceptionengine
{
	AnimationComponent::AnimationComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
		mAnimationController = std::make_unique<AnimationController>();
		if (mWorld.get().GetEntity(mEntityID).HasComponent<SkeletalMeshComponent>())
		{
			if (mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance->mSkeletalMesh != nullptr)
			{
				mAnimationController->Initialize(mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance->mSkeletalMesh->mSkeleton);
			}
		}
	}

	AnimationComponent::~AnimationComponent() = default;

	AnimationComponent::AnimationComponent(AnimationComponent&&) noexcept = default;
	/*
		:mWorld(other.mWorld), mEntityID(other.mEntityID)
	{
		mAnimationController = std::move(other.mAnimationController);
	}*/



	void AnimationComponent::PlayEventAnimation(EventAnimPlaySetting const& setting)
	{
		auto pAnimation = gResourceMgr.GetResource<Animation>(setting.animFilePath);
		assert(pAnimation != nullptr);
		SkeletalMeshInstance& skeletonMeshInstance = *mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance;
		assert(skeletonMeshInstance.mSkeletalMesh != nullptr && skeletonMeshInstance.mSkeletalMesh->mSkeleton == pAnimation->mSkeleton);

		mAnimationController->PlayEventAnimation(setting);
	}

	float AnimationComponent::GetCurrentEventAnimTime() const
	{
		return mAnimationController->GetCurrentEventAnimTime();
	}

	float AnimationComponent::GetCurrentEventAnimDuration() const
	{
		return  mAnimationController->GetCurrentEventAnimDuration();
	}

	void AnimationComponent::ChainAimToInDuration(Vec3f const& targetPosition, Vec3f const& eyeOffsetInHeadCoord, float duration)
	{
		mAnimationController->ChainAimToInDuration(mWorld.get().GetEntity(mEntityID).GetComponent<TransformComponent>().GetWorldTransform(),
												   targetPosition, 
												   eyeOffsetInHeadCoord, 
												   duration);
	}



	float AnimationComponent::GetCurrentEventAnimRatio() const
	{
		return mAnimationController->GetCurrentEventAnimTime() / mAnimationController->GetCurrentEventAnimDuration();
	}

	void AnimationComponent::HandReachTarget(Matrix4x4f const& endEffector)
	{
		SkeletalMeshInstance& skeletonMeshInstance = *mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance;
		mAnimationController->HandReachTarget(skeletonMeshInstance.mHandArmIkChain, endEffector);
	}

	void AnimationComponent::StopAnimation()
	{
		mAnimationController->StopAnimation();
	}

	void AnimationComponent::TestAimAxis()
	{
		mAnimationController->TestAimAxis();
	}

	void AnimationComponent::SetAimIkChain(std::vector<std::string> const& chainBoneNames,
										   std::vector<float> const& weights)
	{
		mAnimationController->SetAimIkChain(chainBoneNames, weights);
	}

	void AnimationComponent::TestAxis()
	{
		SkeletalMeshInstance& skeletonMeshInstance = *mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance;
		mAnimationController->TestAxis(skeletonMeshInstance.mHandArmIkChain);
	}

	bool AnimationComponent::IsPlayingEventAnimation() const
	{
		return mAnimationController->IsPlayingEventAnimation();
	}

	Matrix4x4f AnimationComponent::GetSocketRefTransformation(std::string const& socketName)
	{
		return mAnimationController->GetSocketRefTransformation(socketName);
	}

	void AnimationComponent::InsertEventAnimSpeedRangeRatio(float startRatio, float endRatio, float playSpeed)
	{
		mAnimationController->InsertEventAnimSpeedRange(startRatio, endRatio, playSpeed);
	}

	void AnimationComponent::InsertEventAnimSpeedRangeSecond(float startSecond, float endSecond, float playSpeed)
	{
		float startRatio = startSecond / GetCurrentEventAnimDuration();
		float endRatio = endSecond / GetCurrentEventAnimDuration();
		mAnimationController->InsertEventAnimSpeedRange(startRatio, endRatio, playSpeed);
	}


	bool AnimationComponent::IsAimIkActive() const
	{
		return mAnimationController->IsAimIkActive();
	}

	void AnimationComponent::DeactivateAimIk(float blendOutDuration)
	{
		mAnimationController->DeactivateAimIk(blendOutDuration);
	}

	int AnimationComponent::GetCurrentAsmActiveState() const
	{
		return mAnimationController->GetCurrentAsmActiveState();
	}

	float AnimationComponent::GetCurrentAsmActiveStateRunningSecond() const
	{
		return mAnimationController->GetCurrentAsmActiveStateRunningSecond();
	}
}