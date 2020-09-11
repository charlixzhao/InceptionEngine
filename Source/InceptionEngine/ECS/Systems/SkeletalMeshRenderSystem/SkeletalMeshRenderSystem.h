#pragma once

#include "ECS/Systems/SystemBase.h"

namespace inceptionengine
{
	class Renderer;
	class ComponentsPool;
	class SkeletalMeshComponent;

	class SkeletalMeshRenderSystem final : public SystemBase
	{
	public:
		explicit SkeletalMeshRenderSystem(Renderer& renderer, ComponentsPool& pool);

		void Start();

		void Update(float deltaTime);
		
		void End();
		

	private:
		void LoadSkeletalMeshToDevice(SkeletalMeshComponent& component);
		void UnloadSkeletalMeshFromDevice(SkeletalMeshComponent& component);
		void CreateRenderUnit(SkeletalMeshComponent& component);

	private:
		Renderer& mRenderer;

	};
}