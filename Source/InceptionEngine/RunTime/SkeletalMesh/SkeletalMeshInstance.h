#pragma once

#include "SkeletalMesh.h"
#include "../Animation/AnimPose.h"
#include "RunTime/RHI/Renderer/IRenderObject.h"


namespace inceptionengine
{
	class SkeletalMeshInstance final : public IRenderObject
	{
	public:
		SkeletalMeshInstance() = default;

		~SkeletalMeshInstance();

		virtual RenderObejctID GetRenderObejctID() const override;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) override;

		void InitializeRenderObjects(size_t numOfSubMeshes);

	private:
		friend class SkeletalMeshComponent;
		friend class SkeletalMeshRenderSystem;

		std::shared_ptr<SkeletalMesh const> mSkeletalMesh = nullptr;

		AnimPose mCurrentPose;

		RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

		std::vector<VertexBuffer> mVertexBuffers;

		std::vector<IndexBuffer> mIndexBuffers;
		
		std::vector<Texture> mTextures;;
		
		std::vector<Pipeline> mPipelines;
		
		UniformBuffer mUniformBuffer;
		
		std::vector<UniformBufferDescription> mUniformBufferDescriptions;

		std::array<std::vector<RenderUnit>, NumOfRenderBuffers> mRenderUnits = {};
	};
}