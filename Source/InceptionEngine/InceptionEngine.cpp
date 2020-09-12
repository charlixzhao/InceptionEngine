#include "InceptionEngine.h"

#include "RunTime/RHI/Renderer/Renderer.h"

#include "RunTime/RHI/WindowHandler/WindowHandler.h"
#include "ECS/World.h"
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
		std::string currentPath = std::filesystem::current_path().string();
		
		mRenderer = std::make_unique<Renderer>();

		mWindowHandler = std::make_unique<WindowHandler>();

		mWindowHandler->Initialize();

		mRenderer->Initialize(*mWindowHandler);
	}

	InceptionEngine::~InceptionEngine() = default;

	IWorld* InceptionEngine::CreateWorld()
	{
		mWorld = std::make_unique<World>(*mRenderer);
		return mWorld.get();
	}

	IWorld* InceptionEngine::LoadWorld(std::string const& worldFilePath)
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

			mWindowHandler->PollEvents(); //InputEvents = mWindowHandler.PollEvents();

			mWorld->Simulate(deltaTime); //mWorld->Simulate(InputEvents deltaTime);

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

