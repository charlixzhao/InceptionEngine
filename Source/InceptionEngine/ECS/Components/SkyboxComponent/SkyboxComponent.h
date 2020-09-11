
#pragma once

#include "ECS/Components/IRenderComponent/IRenderComponent.h"

#include "RunTime/SkeletalMesh/Vertex.h"

#include <array>
#include <vector>
#include <string>

namespace inceptionengine
{
	class SkyboxComponent : public IRenderComponent
	{
	public:
		std::vector<Vertex> GetVertices();

		std::vector<unsigned int> GetIndices();

	private:
		friend class SkyboxSystem;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) override;

	private:
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