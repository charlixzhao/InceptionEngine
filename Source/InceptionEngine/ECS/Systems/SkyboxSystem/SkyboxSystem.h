#pragma once

#include <array>
#include <string>

namespace inceptionengine
{
	class Renderer;
	class SkyboxComponent;

	class SkyboxSystem
	{
	public:
		explicit SkyboxSystem(Renderer& renderer, SkyboxComponent& skybox);

		void SetSkybox(std::array<std::string, 6> const& texturePath);

		void Start();

		void End();

	private:
		void LoadSkyboxToDevice();

		void UnloadSkyboxFromDevice();
	
		void CreateRenderUnit();

	private:
		SkyboxComponent& mSkybox;
		Renderer& mRenderer;
	};
}