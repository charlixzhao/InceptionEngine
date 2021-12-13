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

namespace inceptionengine
{

	void ComputeNormals(std::vector<Vertex>& vertices, std::vector<uint32_t> const& indices);

	SkeletalMeshComponent::SkeletalMeshComponent(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
		mSkeletalMeshInstance = std::make_unique<SkeletalMeshInstance>();
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
		Vertex v1 = {};
		v1.position = Vec4f(scale, level, scale, 1.0f);
		v1.texCoord =Vec3f(0.0f, 0.0f, 0.0f);
		Vertex v2 = {};
		v2.position = Vec4f(-scale, level, scale, 1.0f);
		v2.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		Vertex v3 = {};
		v3.position = Vec4f(-scale, level, -scale, 1.0f);
		v3.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		Vertex v4 = {};
		v4.position = Vec4f(scale, level, -scale, 1.0f);
		v4.texCoord = Vec3f(1.0f, 0.0f, 0.0f);

		std::vector<Vertex> planeVertices = { v1, v2, v3, v4 };
		std::vector<uint32_t> planeIndices = { 0,2,1,2,0,3 };

		ComputeNormals(planeVertices, planeIndices);

		auto plane = std::make_shared<SkeletalMesh>();
		plane->mSubMeshes.resize(1);
		plane->mSubMeshes[0].vertices = planeVertices;
		plane->mSubMeshes[0].indices = planeIndices;
		plane->mSubMeshes[0].texturePath = "StandAloneResource/T_Ground.jpg";
		plane->mSubMeshes[0].shaderPath = 
		{ 
			"EngineResource/shader/spv/vertex.spv",
			"EngineResource/shader/spv/highlight1.spv" 
		};
		plane->mSkeleton = std::make_shared<Skeleton>();
		mSkeletalMeshInstance->mSkeletalMesh = plane;

		mSkeletalMeshInstance->InitializeRenderObjects();
	}

	void SkeletalMeshComponent::StartAddCube()
	{
		mSkeletalMeshInstance->mSkeletalMesh = std::make_shared<SkeletalMesh>();
		mSkeletalMeshInstance->mSkeletalMesh->mSkeleton = std::make_shared<Skeleton>();
	}


	void ComputeNormals(std::vector<Vertex>& vertices, std::vector<uint32_t> const& indices)
	{
		for (int i = 0; i < indices.size(); i += 3)
		{
			// Get the face normal
			auto vector1 = vertices[indices[i + 1]].position - vertices[indices[i]].position;
			auto vector2 = vertices[indices[i + 2]].position - vertices[indices[i]].position;
			auto faceNormal = CrossProduct(vector1, vector2);
			faceNormal = NormalizeVec(faceNormal);

			// Add the face normal to the 3 vertices normal touching this face
			vertices[indices[i]].normal += faceNormal;
			vertices[indices[i + 1]].normal += faceNormal;
			vertices[indices[i + 2]].normal += faceNormal;
		}

		// Normalize vertices normal
		for (auto& v : vertices) v.normal = NormalizeVec(v.normal);
	}


	SubMesh CananicalCube(float x, float y, float z)
	{
		Vertex v0 = {};
		v0.position = Vec4f(0.0f, y / 2.0f, -z / 2.0f, 1.0f);
		v0.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		v0.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v0.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v1 = {};
		v1.position = Vec4f(0.0f, y / 2.0f, z / 2.0f, 1.0f);
		v1.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		v1.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v1.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v2 = {};
		v2.position = Vec4f(0.0f, -y / 2.0f, z / 2.0f, 1.0f);
		v2.texCoord = Vec3f(1.0f, 0.0f, 0.0f);
		v2.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v2.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v3 = {};
		v3.position = Vec4f(0.0f, -y / 2.0f, -z / 2.0f, 1.0f);
		v3.texCoord = Vec3f(0.0f, 0.0f, 0.0f);
		v3.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v3.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v4 = {};
		v4.position = Vec4f(x, y / 2.0f, -z / 2.0f, 1.0f);
		v4.texCoord = Vec3f(0.0f, 1.0f, 0.0f);
		v4.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v4.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v5 = {};
		v5.position = Vec4f(x, y / 2.0f, z / 2.0f, 1.0f);
		v5.texCoord = Vec3f(1.0f, 1.0f, 0.0f);
		v5.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v5.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v6 = {};
		v6.position = Vec4f(x, -y / 2.0f, z / 2.0f, 1.0f);
		v6.texCoord = Vec3f(1.0f, 0.0f, 0.0f);
		v6.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v6.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);
		Vertex v7 = {};
		v7.position = Vec4f(x, -y / 2.0f, -z / 2.0f, 1.0f);
		v7.texCoord = Vec3f(0.0f, 0.0f, 0.0f);
		v7.affectedBonesID = Vec4ui(0 + AnimPoseOffsetInUBuffer, 0, 0, 0);
		v7.boneWeights = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);



		std::vector<Vertex> cubeVertices = { v0, v1, v2, v3, v4,v5,v6,v7 };
		std::vector<uint32_t> cubeIndices = { 3,2,1,1,0,3,0,1,5,5,4,0,1,2,6,6,5,1,0,4,7,7,3,0,6,2,3,3,7,6,4,5,6,6,7,4 };
		
		//assign normals
		ComputeNormals(cubeVertices, cubeIndices);

		SubMesh c;
		c.vertices = cubeVertices;
		c.indices = cubeIndices;
		return c;
	}

	int SkeletalMeshComponent::AddCube(float x, float y, float z, Vec3f const& offset, int parent, std::string const& texture)
	{
		
		int cubeID = mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.size();
		SubMesh cube = CananicalCube(x, y, z);
		for (auto& v : cube.vertices)
		{
			v.affectedBonesID[0] += cubeID;
			v.position += Vec4f(offset, 0.0f);
		}

		cube.texturePath = texture;
		cube.shaderPath =
		{
			"EngineResource/shader/spv/vertex.spv",
			"EngineResource/shader/spv/fragment.spv"
		};

		mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.push_back(cube);

		Skeleton::Bone bone;
		bone.ID = cubeID;
		bone.parentID = parent;
		bone.bindPose = Translate(offset);
		bone.bindPoseInv = Translate(-offset);
		bone.lclRefPose = bone.bindPose;
		mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones.push_back(bone);
		
		return cubeID;
		
	}

	void SkeletalMeshComponent::FinishAddCube()
	{
		mSkeletalMeshInstance->InitializeRenderObjects();
	}


}

