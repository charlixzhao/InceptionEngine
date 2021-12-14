
#pragma once

#include "RunTime/RHI/Renderer/IRenderObject.h"

#include "RunTime/RHI/Renderer/Vertex.h"
#include "RunTime/RHI/Renderer/ShaderPath.h"

namespace inceptionengine
{
	class SkyboxComponent : public IRenderObject
	{
	public:
		std::vector<Vertex> GetVertices() const;

		std::vector<unsigned int> GetIndices() const;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) override;

		virtual RenderObejctID GetRenderObejctID() const override;

	private:
		friend class SkyboxSystem;

		RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

		ShaderPath const mShaderPath =
		{
			"EngineResource/shader/spv/skyboxVertex.spv",
			"EngineResource/shader/spv/skyboxFrag.spv"
		};

		std::array<std::string, 6> mTexturePathes;

		VertexBuffer mVertexBuffer;
		IndexBuffer mIndexBuffer;
		CubeMap mCubMap;;
		Pipeline mPipeline;
		UniformBuffer mUniformBuffer;
		UniformBufferDescription mUniformBufferDescription;

		std::array<std::vector<RenderUnit>, NumOfRenderBuffers> mRenderUnits = {};
		
	};
}