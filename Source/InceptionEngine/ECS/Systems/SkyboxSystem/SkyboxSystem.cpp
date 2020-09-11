
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
		mRenderer.CreateVertexBuffer(mSkybox.mVertexBuffer, mSkybox.GetVertices());
		mRenderer.CreateIndexBuffer(mSkybox.mIndexBuffer, mSkybox.GetIndices());
		mRenderer.CreateCubeMap(mSkybox.mCubMap, mSkybox.mTexturePathes);
		mRenderer.CreateUniformBuffer(mSkybox.mUniformBuffer);
		mRenderer.CreateSkyboxDataDescription(mSkybox.mUniformBufferDescription, mSkybox.mUniformBuffer, mSkybox.mCubMap);
		mRenderer.CreatePipeline(mSkybox.mPipeline, mSkybox.mShaderPath, mSkybox.mUniformBufferDescription);
		CreateRenderUnit();

		if (mSkybox.mRenderObejctID == Renderer::InvalidRenderObjectID)
		{
			mSkybox.mRenderObejctID = mRenderer.SubmitToDevice(&mSkybox);
		}
		
	}

	void SkyboxSystem::UnloadSkyboxFromDevice()
	{
		mRenderer.DestroyVertexBuffer(mSkybox.mVertexBuffer);
		mRenderer.DestroyIndexBuffer(mSkybox.mIndexBuffer);
		mRenderer.DestroyCubeMap(mSkybox.mCubMap);
		mRenderer.DestroyUniformBuffer(mSkybox.mUniformBuffer);
		mRenderer.DestroyUniformBufferDescription(mSkybox.mUniformBufferDescription);
		mRenderer.DestroyPipeline(mSkybox.mPipeline);
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

