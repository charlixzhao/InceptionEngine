#include "SkeletalMeshRenderSystem.h"

#include "ECS/Entity/EntityComponentPool.hpp"
#include "ECS/Components/SkeletalMeshComponent/SkeletalMeshComponent.h"
#include "RunTime/SkeletalMesh/SkeletalMeshInstance.h"
#include "RunTime/Animation/AnimationController.h"

#include "ECS/Systems/TransformSystem/TransformSystem.h"

#include "RunTime/RHI/Renderer/Renderer.h"




namespace inceptionengine
{
	SkeletalMeshRenderSystem::SkeletalMeshRenderSystem(Renderer& renderer, ComponentsPool& pool, TransformSystem const& transformSystem)
		:mRenderer(renderer), SystemBase(pool), mTransformSystem(transformSystem)
	{
	}

	void SkeletalMeshRenderSystem::Start()
	{
		ComponentPool<SkeletalMeshComponent>* pool = mComponentsPool.get().GetComponentPool<SkeletalMeshComponent>();
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
		auto const& map = mComponentsPool.get().GetComponentPool<SkeletalMeshComponent>()->GetEntityMap();
		auto& view = mComponentsPool.get().GetComponentPool<SkeletalMeshComponent>()->GetComponentView();

		
		for (auto const& pair : map)
		{
			auto& component = view[pair.second];

			if (component.mSkeletalMeshInstance != nullptr)
			{
				int boneNumber = 0;
				if (component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton != nullptr)
				{
					boneNumber = component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->GetBoneNumber();
				}
				auto uBufferMat = GetIdentityTransfromVector(boneNumber + AnimPoseOffsetInUBuffer);

				//set model transform
				uBufferMat[1] = mTransformSystem.get().GetEntityWorldTransform(pair.first);

				//set bone transform
				/*
				if (component.IsPlayingAnimation())
				{
					std::vector<Matrix4x4f> const& finalPose = component.mAnimationController->GetFinalPose();
					for (int bone = 0; bone < finalPose.size(); bone++)
					{
						uBufferMat[bone + 2] = finalPose[bone] * component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones[bone].bindPoseInv;
					}
				}*/

				std::vector<Matrix4x4f> const& localFinalPose = component.mAnimationController->GetFinalPose();
				std::vector<Matrix4x4f> globalFinalPose;
				globalFinalPose.resize(localFinalPose.size());
				for (auto const& bone : component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones)
				{
					Matrix4x4f globalTransform = localFinalPose[bone.ID];
					int parentID = bone.parentID;
					while (parentID != -1)
					{
						globalTransform = localFinalPose[parentID] * globalTransform;
						parentID = component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones[parentID].parentID;
					}
					globalFinalPose[bone.ID] = globalTransform;
				}


				for (int bone = 0; bone < globalFinalPose.size(); bone++)
				{
					uBufferMat[bone + 2] = globalFinalPose[bone] * component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->mBones[bone].bindPoseInv;
				}

				mRenderer.get().UpdateUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer, uBufferMat, nullptr);
			}
		}

	}

	void SkeletalMeshRenderSystem::End()
	{
		ComponentPool<SkeletalMeshComponent>* pool = mComponentsPool.get().GetComponentPool<SkeletalMeshComponent>();
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

		/*
		for testing, init ubuffer from pose
		*/
		int boneNumber = 0;
		if (component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton != nullptr)
		{
			boneNumber = component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton->GetBoneNumber();
		}
		auto uBufferMat = GetIdentityTransfromVector(boneNumber + AnimPoseOffsetInUBuffer);
		//for (int i = 0; i < boneNumber; i++)
		//{
			//uBufferMat[i + AnimPoseOffsetInUBuffer] = component.mSkeletalMeshInstance->mCurrentPose[i] * component.mSkeletalMeshInstance->mSkeletalMesh->mSkeleton.mBones[i].bindPoseInv;
		//}

		mRenderer.get().InitializeUniformBuffer(component.mSkeletalMeshInstance->mUniformBuffer, uBufferMat);

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

