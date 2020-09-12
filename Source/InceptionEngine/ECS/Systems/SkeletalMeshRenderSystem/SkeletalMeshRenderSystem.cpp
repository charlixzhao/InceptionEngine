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
		mRenderer.get().CreateUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer);
		mRenderer.get().InitializeUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer,
										  GetArrayOfIdentity<Transform>(component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton.GetBoneNumber() + AnimPoseOffsetInUBuffer));

		auto numOfSubMesh = component.mSkeletalMeshInstance->mVertexBuffers.size();
		for (size_t i = 0; i < numOfSubMesh; i++)
		{
			mRenderer.get().CreateVertexBuffer(component.mSkeletalMeshInstance->mVertexBuffers[i], component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].vertices);

			mRenderer.get().CreateIndexBuffer(component.mSkeletalMeshInstance->mIndexBuffers[i], component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].indices);

			mRenderer.get().CreateTexture(component.mSkeletalMeshInstance->mTextures[i], component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].texturePath);

			mRenderer.get().CreateUniformBufferDescription(component.mSkeletalMeshInstance->mUniformBufferDescriptions[i],
													 component.mSkeletalMeshInstance->mUniformBuffer,
													 component.mSkeletalMeshInstance->mTextures[i]);

			mRenderer.get().CreatePipeline(component.mSkeletalMeshInstance->mPipelines[i],
									 component.mSkeletalMeshInstance->mSkeletalMesh->mSubMeshes[i].shaderPath, 
									 component.mSkeletalMeshInstance->mUniformBufferDescriptions[i]);
		}

		CreateRenderUnit(component);

		mRenderer.get().SubmitToDevice(component.mSkeletalMeshInstance.get());
	}

	void SkeletalMeshRenderSystem::UnloadSkeletalMeshFromDevice(SkeletalMeshComponent& component)
	{
		mRenderer.get().DestroyUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer);

		auto numOfSubMesh = component.mSkeletalMeshInstance->mVertexBuffers.size();
		for (size_t i = 0; i < numOfSubMesh; i++)
		{
			mRenderer.get().DestroyVertexBuffer(component.mSkeletalMeshInstance->mVertexBuffers[i]);

			mRenderer.get().DestroyIndexBuffer(component.mSkeletalMeshInstance->mIndexBuffers[i]);

			mRenderer.get().DestroyTexture(component.mSkeletalMeshInstance->mTextures[i]);

			mRenderer.get().DestroyUniformBufferDescription(component.mSkeletalMeshInstance->mUniformBufferDescriptions[i]);

			mRenderer.get().DestroyPipeline(component.mSkeletalMeshInstance->mPipelines[i]);
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

