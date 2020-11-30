#include "InceptionEngine.h"

#include "RunTime/RHI/Renderer/Renderer.h"

#include "RunTime/RHI/WindowHandler/WindowHandler.h"

#include "PathHelper.h"

#include <ctime>
#include <cstdlib>

#include <filesystem>

namespace inceptionengine
{
	InceptionEngine& InceptionEngine::GetInstance()
	{
		static InceptionEngine gEngine;
		return gEngine;
	}

	void InceptionEngine::SetGameDiretory(std::string const& dir)
	{
	}
	


	InceptionEngine::InceptionEngine()
	{
		srand(static_cast <unsigned> (time(0)));
		std::string enginePath = PathHelper::GetEnginePath(std::filesystem::current_path().string());
		PathHelper::SetEngineDirectory(enginePath);
		
		mRenderer = std::make_unique<Renderer>();

		mWindowHandler = std::make_unique<WindowHandler>();

		mWindowHandler->Initialize();

		mRenderer->Initialize(*mWindowHandler);
	}

	InceptionEngine::~InceptionEngine() = default;

	World* InceptionEngine::CreateWorld()
	{
		mWorld = std::make_unique<World>(*mRenderer);
		return mWorld.get();
	}

	World* InceptionEngine::LoadWorld(std::string const& worldFilePath)
	{
		return nullptr;
	}

	void InceptionEngine::PlayGame()
	{
		assert(mWorld != nullptr);

		GameStart();

		while (!mWindowHandler->WindowClosed())
		{
			//game loop
			float deltaTime = mWorldTimer.Count<Timer::Second>();

			mWindowHandler->PollEvents();

			mWorld->Simulate(deltaTime, mWindowHandler->GetAndClearPeripheralInput());

			mRenderer->DrawFrame();
		}

		GameEnd();
		
	}

	void InceptionEngine::GameStart()
	{
		mWorld->WorldStart();

		mWorldTimer.Reset();
	}


	void InceptionEngine::GameEnd()
	{
		mWorld->WorldEnd();

		mRenderer->Destroy();

		mWindowHandler->DestroyWindow();
	}
}

