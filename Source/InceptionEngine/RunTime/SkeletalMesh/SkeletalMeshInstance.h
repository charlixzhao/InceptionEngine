#pragma once

#include "SkeletalMesh.h"
#include "../Animation/AnimPose.h"
#include "RunTime/RHI/Renderer/IRenderObject.h"


namespace inceptionengine
{
	struct IkChain
	{
		int Size() const { return BoneIDs.size(); }
		
		std::vector<unsigned int> BoneIDs;
	};

	class SkeletalMeshInstance final : public IRenderObject
	{
	public:
		SkeletalMeshInstance() = default;

		~SkeletalMeshInstance();

		virtual RenderObejctID GetRenderObejctID() const override;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) override;

		void InitializeRenderObjects();

	private:
		friend class SkeletalMeshComponent;
		friend class SkeletalMeshRenderSystem;

		std::shared_ptr<SkeletalMesh const> mSkeletalMesh = nullptr;

		std::vector<Matrix4x4f> mCurrentPose;

		RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

		IkChain mHandArmIkChain;

		std::vector<VertexBuffer> mVertexBuffers;

		std::vector<IndexBuffer> mIndexBuffers;
		
		std::vector<Texture> mTextures;;
		
		std::vector<Pipeline> mPipelines;
		
		UniformBuffer mUniformBuffer;
		
		std::vector<UniformBufferDescription> mUniformBufferDescriptions;

		std::array<std::vector<RenderUnit>, NumOfRenderBuffers> mRenderUnits = {};
	};
}