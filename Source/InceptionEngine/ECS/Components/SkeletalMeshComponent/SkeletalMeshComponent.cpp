#include "IE_PCH.h"

#include "SkeletalMeshComponent.h"

#include "Serialization/Serializer.h"
#include "RunTime/Resource/ResourceManager.h"

#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"
#include "RunTime/SkeletalMesh/SkeletalMesh.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

#include "RunTime/Animation/AnimationController.h"
#include "RunTime/Animation/Animation.h"
#include "ECS/Components/AnimationComponent/AnimationComponent.h"

#include "ECS/World.h"
#include "ECS/Entity/Entity.h"
#include "ECS/Components/TransformComponent/TransformComponent.h"

#include "RunTime/Geometry/GeomGen.h"

namespace inceptionengine
{

	SkeletalMeshComponent::SkeletalMeshComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
		mSkeletalMeshInstance = std::make_unique<SkeletalMeshInstance>();
	}

	SkeletalMeshComponent::~SkeletalMeshComponent() = default;

	SkeletalMeshComponent::SkeletalMeshComponent(SkeletalMeshComponent&&) noexcept = default;

	void SkeletalMeshComponent::SetTexture(std::vector<std::string> const& textureFilePaths)
	{
		assert(mSkeletalMeshInstance->mSkeletalMesh != nullptr);
		assert(textureFilePaths.size() == mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.size());
		for (int i = 0; i < mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.size(); i++)
		{
			mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].texturePath = textureFilePaths[i];
		}
	}

	void SkeletalMeshComponent::CreateSocket(std::string const& socketName, std::string const& parentName, Matrix4x4f const& lclTransform)
	{
		assert(mSkeletalMeshInstance->mSkeletalMesh != nullptr && "no skeletal mesh in this sk mesh component");
		Skeleton& skeleton = *mSkeletalMeshInstance->mSkeletalMesh->mSkeleton;
		assert(skeleton.mSocketToIndexMap.find(socketName) == skeleton.mSocketToIndexMap.end() && "socket already exists");

		Skeleton::Socket socket;
		socket.parentID = skeleton.GetBoneID(parentName);
		assert(socket.parentID != -1);
		socket.name = socketName;
		socket.lclTransform = lclTransform;
		skeleton.mSocketToIndexMap.insert(std::pair(socketName, static_cast<int>(skeleton.mSockets.size())));
		skeleton.mSockets.push_back(socket);
	}

	int SkeletalMeshComponent::GetSocketParentID(std::string const& socketName)
	{
		int socketID = mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mSocketToIndexMap.at(socketName);
		return mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mSockets[socketID].parentID;
	}

	Matrix4x4f SkeletalMeshComponent::GetSocketLclTransform(std::string const& socketName)
	{
		int socketID = mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mSocketToIndexMap.at(socketName);
		return mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mSockets[socketID].lclTransform;
	}

	Matrix4x4f SkeletalMeshComponent::GetSocketGlobalTransform(std::string const& socketName)
	{
	
		Entity const& entity = mWorld.get().GetEntity(mEntityID);
		assert(entity.HasComponent<AnimationComponent>() && "Add AnimationComponent if you want to use socket");
		return entity.GetComponent<TransformComponent>().GetWorldTransform() * entity.GetComponent<AnimationComponent>().GetSocketRefTransformation(socketName);
	}

	
	void SkeletalMeshComponent::SetVisibility(bool visible)
	{
		for (int i = 0; i < NumOfRenderBuffers; i++)
		{
			for (auto& renderUnit : mSkeletalMeshInstance->mRenderUnits[i])
			{
				renderUnit.visible = visible;
			}
		}

		mVisible = visible;
	}

	void SkeletalMeshComponent::SetShaderPath(int submeshID, std::string const& vertexShader, std::string const& fragShader)
	{
		assert(mSkeletalMeshInstance->mSkeletalMesh != nullptr);
		if (submeshID == -1)
		{
			for (int i = 0; i < mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.size(); i++)
			{
				mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].shaderPath = { vertexShader, fragShader };
			}
		}
		else
		{
			mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[submeshID].shaderPath = { vertexShader, fragShader };
		}
	
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


	void SkeletalMeshComponent::SetPlane(float scale, float level, std::string const& texture)
	{
		auto plane = std::make_shared<SkeletalMesh>();
		plane->mSubMeshes.push_back(GeneratePlane(scale, level, texture));
		plane->mSkeleton = std::make_shared<Skeleton>();
		mSkeletalMeshInstance->mSkeletalMesh = plane;
		mSkeletalMeshInstance->InitializeRenderObjects();
	}

	void SkeletalMeshComponent::SetSphere(float radius, std::string const& texture)
	{
		auto sphere = std::make_shared<SkeletalMesh>();
		sphere->mSubMeshes.push_back(GenerateSphere(radius, texture));
		sphere->mSkeleton = std::make_shared<Skeleton>();
		mSkeletalMeshInstance->mSkeletalMesh = sphere;
		mSkeletalMeshInstance->InitializeRenderObjects();
	}

	void SkeletalMeshComponent::StartAddCube()
	{
		mSkeletalMeshInstance->mSkeletalMesh = std::make_shared<SkeletalMesh>();
		mSkeletalMeshInstance->mSkeletalMesh->mSkeleton = std::make_shared<Skeleton>();
	}


	int SkeletalMeshComponent::AddCube(float x, float y, float z, float r, Vec3f const& offset, int parent, std::string const& texture)
	{
		int cubeID = mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones.size();
		SubMesh cube = GenerateCuboid(x, y, z, texture);
		SubMesh sphere = GenerateSphere(r, "EngineResource/texture/grey.png");
		for (auto& v : cube.vertices)
		{
			v.affectedBonesID[0] += cubeID + AnimPoseOffsetInUBuffer;
			v.position[0] += r;
			v.position += Vec4f(offset, 0.0f);
		}
		for (auto& v : sphere.vertices)
		{
			v.affectedBonesID[0] += cubeID + AnimPoseOffsetInUBuffer;
			v.position += Vec4f(offset, 0.0f);
		}

		mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.push_back(sphere);
		mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.push_back(cube);

		Skeleton::Bone bone;
		bone.ID = cubeID;
		bone.parentID = parent;
		bone.bindPose = Translate(offset);
		bone.bindPoseInv = Translate(-offset);
		bone.lclRefPose = bone.bindPose;
		mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones.push_back(bone);
		

		//add to kinematics tree
		auto& animController = mWorld.get().GetEntity(mEntityID).GetComponent<AnimationComponent>().mAnimationController;
		animController->AddCuboidLink(x, y, z, r, parent, offset);

		return cubeID;	
	}

	void SkeletalMeshComponent::FinishAddCube()
	{
		mSkeletalMeshInstance->InitializeRenderObjects();
		mWorld.get().GetEntity(mEntityID).GetComponent<AnimationComponent>().mAnimationController->Initialize(mSkeletalMeshInstance->mSkeletalMesh->mSkeleton);
	}


}

