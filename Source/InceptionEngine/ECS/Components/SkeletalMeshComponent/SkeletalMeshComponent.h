#pragma once

#include "EngineGlobals/EngineApiPrefix.h"
#include "ECS/Components/IRenderComponent/IRenderComponent.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"
#include "RunTime/Animation/AnimPose.h"
#include "EngineGlobals/RenderGlobals.h"

#include <string>
#include <array>
#include <vector>

namespace inceptionengine
{

	class SkeletalMeshRenderSystem;
	
	class IE_API SkeletalMeshComponent : public IRenderComponent
	{
	public:
		explicit SkeletalMeshComponent(SkeletalMeshRenderSystem& system);

		void SetMesh(std::string const& meshFilePath);

		void SetMesh(std::shared_ptr<SkeletalMesh> skeletalMesh);

		void SetMeshPose(AnimPose const& pose);

		void SetPlane();

	private:
		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) override;
		void InitializeRenderObjects(size_t numOfSubMeshes);

	private:
		friend class SkeletalMeshRenderSystem;

		SkeletalMeshRenderSystem& mSystem;

		SkeletalMeshInstance mSkeletalMeshInstance;

		bool mLoadedToDevice = false;

		std::vector<VertexBuffer> mVertexBuffers;
		std::vector<IndexBuffer> mIndexBuffers;
		std::vector<Texture> mTextures;;
		std::vector<Pipeline> mPipelines;
		UniformBuffer mUniformBuffer;
		std::vector<UniformBufferDescription> mUniformBufferDescriptions;

		std::array<std::vector<RenderUnit>, NumOfRenderBuffers> mRenderUnits = {};
	};
}
