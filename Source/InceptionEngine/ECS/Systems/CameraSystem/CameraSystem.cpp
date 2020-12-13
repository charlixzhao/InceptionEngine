#include "CameraSystem.h"
#include "RunTime/RHI/Renderer/Renderer.h"
#include "ECS/Components/CameraComponent/CameraComponent.h"

namespace inceptionengine
{
	CameraSystem::CameraSystem(Renderer& renderer, ComponentsPool& componentsPool)
		:mRenderer(renderer), SystemBase(componentsPool)
	{
		;
	}

	void CameraSystem::SetGameCamera(CameraComponent const& camera)
	{
		mGameCamera = &camera;
	}

	void CameraSystem::Update(float deltaTime)
	{
		if(mGameCamera != nullptr) mRenderer.get().SetCameraMatrix(mGameCamera->CameraMatrix());
	}
}

