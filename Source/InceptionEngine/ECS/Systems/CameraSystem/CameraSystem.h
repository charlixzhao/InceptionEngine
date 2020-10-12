
#pragma once

#include "ECS/Systems/SystemBase.h"
namespace inceptionengine
{
	class Renderer; 
	class CameraComponent;

	class CameraSystem : public SystemBase
	{
	public:
		CameraSystem(Renderer& renderer, ComponentsPool& componentsPool);

		void SetGameCamera(CameraComponent const& camera);

		void Update(float deltaTime);

	private:
		std::reference_wrapper<Renderer> mRenderer;
		CameraComponent const* mGameCamera = nullptr;
	};
}