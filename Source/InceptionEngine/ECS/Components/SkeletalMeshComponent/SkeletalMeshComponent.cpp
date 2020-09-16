
#include "SkeletalMeshComponent.h"
#include "Serialization/Serializer.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"

namespace inceptionengine
{

	SkeletalMeshComponent::SkeletalMeshComponent(SkeletalMeshRenderSystem& system)
		:mSystem(system)
	{
		mSkeletalMeshInstance = std::make_unique<SkeletalMeshInstance>();
	}

	SkeletalMeshComponent::~SkeletalMeshComponent() = default;

	SkeletalMeshComponent::SkeletalMeshComponent(SkeletalMeshComponent&&) noexcept = default;

	void SkeletalMeshComponent::SetMesh(std::string const& meshFilePath)
	{
		assert(false && "this function is not ready to use yet");
		
		/*
		mSkeletalMeshInstance.mSkeletalMesh = Serializer::Deserailize<SkeletalMesh>(meshFilePath);

		size_t numSubMeshes = mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes.size();
		InitializeRendererObjects(numSubMeshes);
		*/
	}

	void SkeletalMeshComponent::SetPlane()
	{
		Vertex v1 = {};
		v1.position = glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
		v1.texCoord = glm::vec3(0.0f, 0.0f, 0.0f);
		v1.vertexNormal = glm::vec3(0.0f, 1.0f, 0.0f);
		Vertex v2 = {};
		v2.position = glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
		v2.texCoord = glm::vec3(0.0f, 1.0f, 0.0f);
		v2.vertexNormal = glm::vec3(0.0f, 1.0f, 0.0f);
		Vertex v3 = {};
		v3.position = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		v3.texCoord = glm::vec3(1.0f, 1.0f, 0.0f);
		v3.vertexNormal = glm::vec3(0.0f, 1.0f, 0.0f);
		Vertex v4 = {};
		v4.position = glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		v4.texCoord = glm::vec3(1.0f, 0.0f, 0.0f);
		v4.vertexNormal = glm::vec3(0.0f, 1.0f, 0.0f);


		std::vector<Vertex> planeVertices = { v1, v2, v3, v4 };
		std::vector<uint32_t> planeIndices = { 0,1,2,2,3,0 };

		auto plane = std::make_shared<SkeletalMesh>();
		plane->mSubMeshes.resize(1);
		plane->mSubMeshes[0].vertices = planeVertices;
		plane->mSubMeshes[0].indices = planeIndices;
		plane->mSubMeshes[0].texturePath = "D:\\Inception\\Content\\Textures\\T_Grass.BMP";
		plane->mSubMeshes[0].shaderPath = 
		{ 
			"D:\\InceptionEngine\\EngineResource\\Shader\\spv\\vertex.spv",
			"D:\\InceptionEngine\\EngineResource\\Shader\\spv\\fragment.spv" 
		};

		//SetMesh(plane);

		mSkeletalMeshInstance->mSkeletalMesh = plane;

		size_t numSubMeshes = mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.size();

		InitializeRenderObjects(numSubMeshes);

		
	}

	void SkeletalMeshComponent::InitializeRenderObjects(size_t numOfSubMeshes)
	{
		mSkeletalMeshInstance->InitializeRenderObjects(numOfSubMeshes);
	}

	void SkeletalMeshComponent::SetMeshPose(AnimPose const& pose)
	{
	}
}

