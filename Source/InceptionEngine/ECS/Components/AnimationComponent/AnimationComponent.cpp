
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

	void AnimationComponent::HandReachTarget(Matrix4x4f const& endEffector)
	{
		SkeletalMeshInstance& skeletonMeshInstance = *mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance;
		mAnimationController->HandReachTarget(skeletonMeshInstance.mHandArmIkChain, endEffector);
	}

	void AnimationComponent::StopAnimation()
	{
		mAnimationController->StopAnimation();
	}

	void AnimationComponent::TestAxis()
	{
		SkeletalMeshInstance& skeletonMeshInstance = *mWorld.get().GetEntity(mEntityID).GetComponent<SkeletalMeshComponent>().mSkeletalMeshInstance;
		mAnimationController->TestAxis(skeletonMeshInstance.mHandArmIkChain);
	}

	bool AnimationComponent::IsPlayingAnimation()
	{
		return mAnimationController->IsPlayingAnimation();
	}

}