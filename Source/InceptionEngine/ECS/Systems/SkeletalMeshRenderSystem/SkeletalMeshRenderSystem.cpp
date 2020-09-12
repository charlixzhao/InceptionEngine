#include "SkeletalMeshRenderSystem.h"

#include "ECS/Entity/EntityComponentPool.hpp"
#include "ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"
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
			if (component.mLoadedToDevice == false && component.mSkeletalMeshInstance->mSkeletalMesh != nullptr)
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
			if (component.mLoadedToDevice == true && component.mSkeletalMeshInstance->mSkeletalMesh != nullptr)
			{
				UnloadSkeletalMeshFromDevice(component);

				component.mLoadedToDevice = false;
			}
		}
	}

	void SkeletalMeshRenderSystem::LoadSkeletalMeshToDevice(SkeletalMeshComponent& component)
	{
		mRenderer.CreateUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer);
		mRenderer.InitializeUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer,
										  GetArrayOfIdentity<Transform>(component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton.GetBoneNumber() + AnimPoseOffsetInUBuffer));

		auto numOfSubMesh = component.mSkeletalMeshInstance->mVertexBuffers.size();
		for (size_t i = 0; i < numOfSubMesh; i++)
		{
			mRenderer.CreateVertexBuffer(component.mSkeletalMeshInstance->mVertexBuffers[i], component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].vertices);

			mRenderer.CreateIndexBuffer(component.mSkeletalMeshInstance->mIndexBuffers[i], component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].indices);

			mRenderer.CreateTexture(component.mSkeletalMeshInstance->mTextures[i], component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].texturePath);

			mRenderer.CreateUniformBufferDescription(component.mSkeletalMeshInstance->mUniformBufferDescriptions[i],
													 component.mSkeletalMeshInstance->mUniformBuffer,
													 component.mSkeletalMeshInstance->mTextures[i]);

			mRenderer.CreatePipeline(component.mSkeletalMeshInstance->mPipelines[i],
									 component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].shaderPath, 
									 component.mSkeletalMeshInstance->mUniformBufferDescriptions[i]);
		}

		CreateRenderUnit(component);

		mRenderer.SubmitToDevice(component.mSkeletalMeshInstance.get());
	}

	void SkeletalMeshRenderSystem::UnloadSkeletalMeshFromDevice(SkeletalMeshComponent& component)
	{
		mRenderer.DestroyUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer);

		auto numOfSubMesh = component.mSkeletalMeshInstance->mVertexBuffers.size();
		for (size_t i = 0; i < numOfSubMesh; i++)
		{
			mRenderer.DestroyVertexBuffer(component.mSkeletalMeshInstance->mVertexBuffers[i]);

			mRenderer.DestroyIndexBuffer(component.mSkeletalMeshInstance->mIndexBuffers[i]);

			mRenderer.DestroyTexture(component.mSkeletalMeshInstance->mTextures[i]);

			mRenderer.DestroyUniformBufferDescription(component.mSkeletalMeshInstance->mUniformBufferDescriptions[i]);

			mRenderer.DestroyPipeline(component.mSkeletalMeshInstance->mPipelines[i]);
		}
	}

	void SkeletalMeshRenderSystem::CreateRenderUnit(SkeletalMeshComponent& component)
	{

		for (size_t bufferIndex = 0; bufferIndex < NumOfRenderBuffers; bufferIndex++)
		{
			for (size_t subpart = 0; subpart < component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes.size(); subpart++)
			{
				RenderUnit unit;
				unit.vertexBuffer = &component.mSkeletalMeshInstance->mVertexBuffers[subpart].vertexBuffer;
				unit.indexBuffer = component.mSkeletalMeshInstance->mIndexBuffers[subpart].indexBuffer;
				unit.numOfIndices = component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[subpart].indices.size();
				unit.pipeline = component.mSkeletalMeshInstance->mPipelines[subpart].pipeline;
				unit.pipelineLayout = component.mSkeletalMeshInstance->mPipelines[subpart].pipelineLayout;
				unit.descriptorSet = &component.mSkeletalMeshInstance->mUniformBufferDescriptions[subpart].descriptorSet[bufferIndex];
				component.mSkeletalMeshInstance->mRenderUnits[bufferIndex].push_back(std::move(unit));
			}
		}
	}


}

