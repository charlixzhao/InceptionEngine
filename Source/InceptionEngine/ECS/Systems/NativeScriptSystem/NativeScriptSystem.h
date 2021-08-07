#pragma once

#include "ECS/Systems/SystemBase.h"
#include "ECS/Components/NativeScriptComponent/NativeScriptComponent.h"
#include "ECS/Components/NativeScriptComponent/NativeScript.h"
#include "RunTime/RHI/WindowHandler/PeripheralInput.h"
namespace inceptionengine
{

	class NativeScriptSystem : public SystemBase
	{
	public:
		NativeScriptSystem(ComponentsPool& componentsPool);

		void Start();

		void Update(std::queue<KeyInput> keyInputs, MouseDeltaPos mouseDeltaPos, JoyStick joystickInput, float dt);

		void End();

	private:
		void EngineKeyInputCallback(KeyInput keyInput);

		void ComponentDispatchKeyInput(NativeScriptComponent& component, KeyInput keyInput);
	};

}


