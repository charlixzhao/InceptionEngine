#include "SkeletalMeshRenderSystem.h"

#include "ECS/Entity/EntityComponentPool.hpp"
#include "ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.h"
#include "RunTime/RHI/Renderer/Renderer.h"

namespace inceptionengine
{
	SkeletalMeshRenderSystem::SkeletalMeshRenderSystem(Renderer& renderer, ComponentsPool& pool)
		:mRenderer(renderer), SystemBase(pool)
	{
	}

	void SkeletalMeshRenderSystem::Start()
	{
		ComponentPool<SkeletalMeshComponent>* pool = mComponentsPool.GetComponentPool<SkeletalMeshComponent>();
		if (pool == nullptr)
			return;
		auto& view = pool->GetComponentView();
		for (auto& component : view)
		{
			if (component.mLoadedToDevice == false && component.mSkeletalMeshInstance.mSkeletalMesh != nullptr)
			{
				LoadSkeletalMeshToDevice(component);

				component.mLoadedToDevice = true;
			}
		}
	}

	void SkeletalMeshRenderSystem::Update(float deltaTime)
	{

	}

	void SkeletalMeshRenderSystem::End()
	{
		ComponentPool<SkeletalMeshComponent>* pool = mComponentsPool.GetComponentPool<SkeletalMeshComponent>();
		if (pool == nullptr)
			return;
		auto& view = pool->GetComponentView();
		for (auto& component : view)
		{
			if (component.mLoadedToDevice == true && component.mSkeletalMeshInstance.mSkeletalMesh != nullptr)
			{
				UnloadSkeletalMeshFromDevice(component);

				component.mLoadedToDevice = false;
			}
		}
	}

	void SkeletalMeshRenderSystem::LoadSkeletalMeshToDevice(SkeletalMeshComponent& component)
	{
		mRenderer.CreateUniformBuffer(component.mUniformBuffer);
		mRenderer.InitializeUniformBuffer(component.mUniformBuffer,
										  GetArrayOfIdentity<Transform>(component.mSkeletalMeshInstance.mSkeletalMesh->mSkeleton.GetBoneNumber() + AnimPoseOffsetInUBuffer));

		auto numOfSubMesh = component.mVertexBuffers.size();
		for (size_t i = 0; i < numOfSubMesh; i++)
		{
			mRenderer.CreateVertexBuffer(component.mVertexBuffers[i], component.mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes[i].vertices);

			mRenderer.CreateIndexBuffer(component.mIndexBuffers[i], component.mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes[i].indices);

			mRenderer.CreateTexture(component.mTextures[i], component.mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes[i].texturePath);

			mRenderer.CreateUniformBufferDescription(component.mUniformBufferDescriptions[i],
													 component.mUniformBuffer, 
													 component.mTextures[i]);

			mRenderer.CreatePipeline(component.mPipelines[i], 
									 component.mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes[i].shaderPath, 
									 component.mUniformBufferDescriptions[i]);
		}

		CreateRenderUnit(component);

		mRenderer.SubmitToDevice(&component);
	}

	void SkeletalMeshRenderSystem::UnloadSkeletalMeshFromDevice(SkeletalMeshComponent& component)
	{
		mRenderer.DestroyUniformBuffer(component.mUniformBuffer);

		auto numOfSubMesh = component.mVertexBuffers.size();
		for (size_t i = 0; i < numOfSubMesh; i++)
		{
			mRenderer.DestroyVertexBuffer(component.mVertexBuffers[i]);

			mRenderer.DestroyIndexBuffer(component.mIndexBuffers[i]);

			mRenderer.DestroyTexture(component.mTextures[i]);

			mRenderer.DestroyUniformBufferDescription(component.mUniformBufferDescriptions[i]);

			mRenderer.DestroyPipeline(component.mPipelines[i]);
		}
	}

	void SkeletalMeshRenderSystem::CreateRenderUnit(SkeletalMeshComponent& component)
	{

		for (size_t bufferIndex = 0; bufferIndex < NumOfRenderBuffers; bufferIndex++)
		{
			for (size_t subpart = 0; subpart < component.mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes.size(); subpart++)
			{
				RenderUnit unit;
				unit.vertexBuffer = &component.mVertexBuffers[subpart].vertexBuffer;
				unit.indexBuffer = component.mIndexBuffers[subpart].indexBuffer;
				unit.numOfIndices = component.mSkeletalMeshInstance.mSkeletalMesh->mSubMeshes[subpart].indices.size();
				unit.pipeline = component.mPipelines[subpart].pipeline;
				unit.pipelineLayout = component.mPipelines[subpart].pipelineLayout;
				unit.descriptorSet = &component.mUniformBufferDescriptions[subpart].descriptorSet[bufferIndex];
				component.mRenderUnits[bufferIndex].push_back(std::move(unit));
			}
		}
	}


}

