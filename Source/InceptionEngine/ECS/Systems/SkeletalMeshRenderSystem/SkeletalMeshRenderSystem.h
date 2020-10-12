#pragma once

#include "ECS/Systems/SystemBase.h"
#include "ECS/Entity/EntityID.h"

namespace inceptionengine
{
	class Renderer;
	class ComponentsPool;
	class SkeletalMeshComponent;
	class AnimationSystem;


	class SkeletalMeshRenderSystem final : public SystemBase
	{
	public:
		explicit SkeletalMeshRenderSystem(Renderer& renderer, ComponentsPool& pool, AnimationSystem& animationSystem);

		void Start();

		void Update(float deltaTime);
		
		void End();
		
	private:
		void LoadSkeletalMeshToDevice(SkeletalMeshComponent& component);
		void UnloadSkeletalMeshFromDevice(SkeletalMeshComponent& component);
		void CreateRenderUnit(SkeletalMeshComponent& component);

		bool NeedToUpdateUBuffer(EntityID id);
	private:
		std::reference_wrapper<Renderer> mRenderer;
		std::reference_wrapper<AnimationSystem> mAnimationSystem;
	};
}