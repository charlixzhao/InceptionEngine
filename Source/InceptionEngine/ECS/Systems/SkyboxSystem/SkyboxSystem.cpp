
#include "SkyboxSystem.h"

#include "ECS/Components/SkyboxComponent/SkyboxComponent.h"
#include "RunTime/RHI/Renderer/Renderer.h"

namespace inceptionengine
{
	SkyboxSystem::SkyboxSystem(Renderer& renderer, SkyboxComponent& mSkybox) :
		mRenderer(renderer), mSkybox(mSkybox)
	{

	}

	void SkyboxSystem::SetSkybox(std::array<std::string, 6> const& texturePath)
	{
		mSkybox.mTexturePathes = texturePath;

		LoadSkyboxToDevice();
	}

	void SkyboxSystem::Start()
	{
		
	}

	void SkyboxSystem::End()
	{
		if (mSkybox.mRenderObejctID != Renderer::InvalidRenderObjectID)
		{
			UnloadSkyboxFromDevice();
		}
	}
	void SkyboxSystem::LoadSkyboxToDevice()
	{
		mRenderer.get().CreateVertexBuffer(mSkybox.mVertexBuffer, mSkybox.GetVertices());
		mRenderer.get().CreateIndexBuffer(mSkybox.mIndexBuffer, mSkybox.GetIndices());
		mRenderer.get().CreateCubeMap(mSkybox.mCubMap, mSkybox.mTexturePathes);
		mRenderer.get().CreateUniformBuffer(mSkybox.mUniformBuffer);
		mRenderer.get().CreateSkyboxDataDescription(mSkybox.mUniformBufferDescription, mSkybox.mUniformBuffer, mSkybox.mCubMap);
		mRenderer.get().CreatePipeline(mSkybox.mPipeline, mSkybox.mShaderPath, mSkybox.mUniformBufferDescription);
		CreateRenderUnit();

		if (mSkybox.mRenderObejctID == Renderer::InvalidRenderObjectID)
		{
			mSkybox.mRenderObejctID = mRenderer.get().SubmitToDevice(&mSkybox);
		}
		
	}

	void SkyboxSystem::UnloadSkyboxFromDevice()
	{
		mRenderer.get().DestroyVertexBuffer(mSkybox.mVertexBuffer);
		mRenderer.get().DestroyIndexBuffer(mSkybox.mIndexBuffer);
		mRenderer.get().DestroyCubeMap(mSkybox.mCubMap);
		mRenderer.get().DestroyUniformBuffer(mSkybox.mUniformBuffer);
		mRenderer.get().DestroyUniformBufferDescription(mSkybox.mUniformBufferDescription);
		mRenderer.get().DestroyPipeline(mSkybox.mPipeline);
		//mRenderer.delete actor
	}

	void SkyboxSystem::CreateRenderUnit()
	{
		for (size_t bufferIndex = 0; bufferIndex < NumOfRenderBuffers; bufferIndex++)
		{
			RenderUnit unit;
			unit.vertexBuffer = &mSkybox.mVertexBuffer.vertexBuffer;
			unit.indexBuffer = mSkybox.mIndexBuffer.indexBuffer;
			unit.numOfIndices = mSkybox.GetIndices().size();
			unit.pipeline = mSkybox.mPipeline.pipeline;
			unit.pipelineLayout = mSkybox.mPipeline.pipelineLayout;
			unit.descriptorSet = &mSkybox.mUniformBufferDescription.descriptorSet[bufferIndex];
			mSkybox.mRenderUnits[bufferIndex].push_back(std::move(unit));
		}
	}
}

