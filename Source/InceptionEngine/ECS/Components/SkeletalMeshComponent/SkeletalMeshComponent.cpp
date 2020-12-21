
#include "SkeletalMeshComponent.h"
#include "Serialization/Serializer.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"
#include "RunTime/Resource/ResourceManager.h"
#include "RunTime/Animation/AnimationController.h"
#include "RunTime/Animation/Animation.h"
#include "ECS/Components/AnimationComponent/AnimationComponent.h"

#include "ECS/World.h"
#include "ECS/Entity/Entity.h"

namespace inceptionengine
{

	SkeletalMeshComponent::SkeletalMeshComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
		mSkeletalMeshInstance = std::make_unique<SkeletalMeshInstance>();
		//mAnimationController = std::make_unique<AnimationController>();
	}

	SkeletalMeshComponent::~SkeletalMeshComponent() = default;

	SkeletalMeshComponent::SkeletalMeshComponent(SkeletalMeshComponent&&) noexcept = default;


	void SkeletalMeshComponent::SetTriangle()
	{
		Vertex v1 = {};
		v1.position = glm::vec4(85.9174652f, -33.0414543f, -0.677608311f, 1.0f);
		v1.texCoord = glm::vec3(RandFloat(0.0f,1.0f), RandFloat(0.0f, 1.0f), 0.0f);
		Vertex v2 = {};
		v2.position = glm::vec4(85.3564911f, -33.2652740f, -0.67933947f, 1.0f);
		v2.texCoord = glm::vec3(RandFloat(0.0f, 1.0f), RandFloat(0.0f, 1.0f), 0.0f);
		Vertex v3 = {};
		v3.position = glm::vec4(85.2866364f, -32.6977081f, -0.674987137f, 1.0f);
		v3.texCoord = glm::vec3(RandFloat(0.0f, 1.0f), RandFloat(0.0f, 1.0f), 0.0f);

		std::vector<Vertex> triangleVertices = { v1, v2, v3 };
		std::vector<uint32_t> triangleIndices = { 0,1,2 };

		auto triangle = std::make_shared<SkeletalMesh>();
		triangle->mSubMeshes.resize(1);
		triangle->mSubMeshes[0].vertices = triangleVertices;
		triangle->mSubMeshes[0].indices = triangleIndices;
		triangle->mSubMeshes[0].texturePath = "StandAloneResource\\T_Grass.BMP";
		triangle->mSubMeshes[0].shaderPath =
		{
			"EngineResource\\shader\\spv\\vertex.spv",
			"EngineResource\\shader\\spv\\fragment.spv"
		};

		mSkeletalMeshInstance->mSkeletalMesh = triangle;

		mSkeletalMeshInstance->InitializeRenderObjects();
	}

	

	void SkeletalMeshComponent::SetMesh(std::string const& filePath)
	{
		std::shared_ptr<SkeletalMesh> pMesh = gResourceMgr.GetResource<SkeletalMesh>(filePath);

		assert(pMesh != nullptr);
		mSkeletalMeshInstance->mSkeletalMesh = pMesh;

		mSkeletalMeshInstance->InitializeRenderObjects();

		/*
		* mSkeletalMeshInstance->mHandArmIkChain.ChainType = IkChain::IkChainType::LeftArmHand;
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftArm"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftForeArm"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftHand"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftHandMiddle1"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftHandMiddle2"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftHandMiddle3"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("LeftHandMiddle4"));*/


		
		/*
mSkeletalMeshInstance->mHandArmIkChain.ChainType = IkChain::IkChainType::RightArmHand;
mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("RightArm"));
mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("RightForeArm"));
mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("RightHand"));*/


		
		mSkeletalMeshInstance->mHandArmIkChain.ChainType = IkChain::IkChainType::RightArmHand;
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("UpperArm_R"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("Forearm_R"));
		mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("Hand_R"));
		//mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("Index_R_1"));
		//mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("Index_R_2"));
		//mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("RightHandMiddle3"));
		//mSkeletalMeshInstance->mHandArmIkChain.BoneIDs.push_back(pMesh->mSkeleton->GetBoneID("RightHandMiddle4"));
	
		if (mWorld.get().GetEntity(mEntityID).HasComponent<AnimationComponent>())
		{
			mWorld.get().GetEntity(mEntityID).GetComponent<AnimationComponent>().mAnimationController->Initialize(mSkeletalMeshInstance->mSkeletalMesh->mSkeleton);
		}
		
	}


	void SkeletalMeshComponent::SetPlane(float scale)
	{
		Vertex v1 = {};
		v1.position = Vec4f(scale, 0.0f, scale, 1.0f);
		v1.texCoord =Vec3f(0.0f, 0.0f, 0.0f);
		v1.normal = Vec3f(100.0f, 80.0f, 500.0f) - Vec3f(v1.position[0], v1.position[1], v1.position[2]);
		Vertex v2 = {};
		v2.position = Vec4f(-scale, 0.0f, scale, 1.0f);
		v2.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		v2.normal = Vec3f(100.0f, 80.0f, 500.0f) - Vec3f(v2.position[0], v2.position[1], v2.position[2]);
		Vertex v3 = {};
		v3.position = Vec4f(-scale, 0.0f, -scale, 1.0f);
		v3.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		v3.normal = Vec3f(100.0f, 80.0f, 500.0f) - Vec3f(v3.position[0], v3.position[1], v3.position[2]);
		Vertex v4 = {};
		v4.position = Vec4f(scale, 0.0f, -scale, 1.0f);
		v4.texCoord = Vec3f(1.0f, 0.0f, 0.0f);
		v4.normal = Vec3f(100.0f, 80.0f, 500.0f) - Vec3f(v4.position[0], v4.position[1], v4.position[2]);


		std::vector<Vertex> planeVertices = { v1, v2, v3, v4 };
		std::vector<uint32_t> planeIndices = { 0,2,1,2,0,3 };

		auto plane = std::make_shared<SkeletalMesh>();
		plane->mSubMeshes.resize(1);
		plane->mSubMeshes[0].vertices = planeVertices;
		plane->mSubMeshes[0].indices = planeIndices;
		plane->mSubMeshes[0].texturePath = "StandAloneResource\\T_Grass.BMP";
		plane->mSubMeshes[0].shaderPath = 
		{ 
			"EngineResource\\shader\\spv\\vertex.spv",
			"EngineResource\\shader\\spv\\fragment.spv" 
		};
		plane->mSkeleton = std::make_shared<Skeleton>();
		mSkeletalMeshInstance->mSkeletalMesh = plane;


		mSkeletalMeshInstance->InitializeRenderObjects();
	}


	void SkeletalMeshComponent::SetMeshPose(AnimPose const& pose)
	{
	}
}

