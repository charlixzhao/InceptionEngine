
#pragma once

/*
stuff in pch that should be exposed to users
*/

#include "Math/Math.h"
#include <functional>
#include <string>
#include <array>

#include "EngineGlobals/EngineApiPrefix.h"

/*
Header for users
*/



#include "ECS/World.h"

#include "ECS/Entity/Entity.h"

#include "ECS/Components/Components.h"

#include "Timer.hpp"

namespace inceptionengine
{

	class WindowHandler;
	class Renderer;

	class IE_API InceptionEngine
	{
	public:
		static InceptionEngine& GetInstance();

		void SetGameDiretory(std::string const& dir);

		World* CreateWorld();

		World* LoadWorld(std::string const& worldFilePath);

		void PlayGame();

		void SetMouseVisibility(bool visibility);

	private:
		InceptionEngine();

		~InceptionEngine();

		InceptionEngine(InceptionEngine const&) = delete;

		InceptionEngine& operator = (InceptionEngine&) = delete;

		InceptionEngine(InceptionEngine&&) = delete;

		InceptionEngine& operator = (InceptionEngine&&) = delete;

		void GameStart();

		void GameEnd();

	private:
		Timer mWorldTimer;

		/*
		We use pointer for window handler and renderer so that we don't need to incldue header for these,
		so user will not be able to get defination of window handler and renderer.
		*/
		std::unique_ptr<WindowHandler> mWindowHandler = nullptr;

		std::unique_ptr<Renderer> mRenderer = nullptr;

		std::unique_ptr<World> mWorld = nullptr;
	};
}
