
#pragma once

#include "RunTime/RHI/Renderer/IRenderObject.h"

#include "RunTime/RHI/Renderer/Vertex.h"
#include "RunTime/RHI/Renderer/ShaderPath.h"

#include <array>
#include <vector>
#include <string>

namespace inceptionengine
{
	class SkyboxComponent : public IRenderObject
	{
	public:
		std::vector<Vertex> GetVertices();

		std::vector<unsigned int> GetIndices();

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) override;

		virtual RenderObejctID GetRenderObejctID() const override;

	private:
		friend class SkyboxSystem;

		RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

		ShaderPath const mShaderPath =
		{
			"D:\\InceptionEngine\\EngineResource\\Shader\\spv\\skyboxVertex.spv",
			"D:\\InceptionEngine\\EngineResource\\Shader\\spv\\skyboxFrag.spv"
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