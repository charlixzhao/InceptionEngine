
#include "NativeScriptSystem.h"
#include "ECS/Entity/EntityComponentPool.hpp"

#include "RunTime/KeyInputCallbackRegistry.h"

namespace inceptionengine
{
	NativeScriptSystem::NativeScriptSystem(ComponentsPool& componentsPool)
		:SystemBase(componentsPool)
	{
	}
	void NativeScriptSystem::Start()
	{
		ComponentPool<NativeScriptComponent>* pool = mComponentsPool.get().GetComponentPool<NativeScriptComponent>();
		if (pool == nullptr)
		{
			throw std::runtime_error("");
		}
		auto& view = pool->GetComponentView();


		for (auto& component : view)
		{
			component.mScript->OnBegin();
		}
	}

	void NativeScriptSystem::Update(std::queue<KeyInput> keyInputs, MouseDeltaPos mouseDeltaPos, float dt)
	{
		ComponentPool<NativeScriptComponent>* pool = mComponentsPool.get().GetComponentPool<NativeScriptComponent>();

		auto& view = pool->GetComponentView();

		for (auto& component : view)
		{
			component.mScript->OnTick(dt);
			component.mScript->OnMouseDeltaPos(mouseDeltaPos);
		}

		while (!keyInputs.empty())
		{
			KeyInput keyInput = keyInputs.front();
			keyInputs.pop();

			EngineKeyInputCallback(keyInput);
			
			for (auto& component : view)
			{
				ComponentDispatchKeyInput(component, keyInput);
			}
		}

	}

	void NativeScriptSystem::End()
	{
	}

	void NativeScriptSystem::EngineKeyInputCallback(KeyInput keyInput)
	{
	}


#define OnPeripheralInput(X) component.mScript->mKeyInputCallbackRegistry->OnPeripheralInput_##X(keyInput.press)

	void NativeScriptSystem::ComponentDispatchKeyInput(NativeScriptComponent& component, KeyInput keyInput)
	{
		switch (keyInput.inputKey)
		{
			case KeyInputTypes::Keyboard_Q: OnPeripheralInput(Keyboard_Q); break;
			case KeyInputTypes::Keyboard_W: OnPeripheralInput(Keyboard_W); break;
			case KeyInputTypes::Keyboard_E: OnPeripheralInput(Keyboard_E); break;
			case KeyInputTypes::Keyboard_R: OnPeripheralInput(Keyboard_R); break;
			case KeyInputTypes::Keyboard_T: OnPeripheralInput(Keyboard_T); break;
			case KeyInputTypes::Keyboard_Y: OnPeripheralInput(Keyboard_Y); break;
			case KeyInputTypes::Keyboard_U: OnPeripheralInput(Keyboard_U); break;
			case KeyInputTypes::Keyboard_I: OnPeripheralInput(Keyboard_I); break;
			case KeyInputTypes::Keyboard_O: OnPeripheralInput(Keyboard_O); break;
			case KeyInputTypes::Keyboard_P: OnPeripheralInput(Keyboard_P); break;
			case KeyInputTypes::Keyboard_A: OnPeripheralInput(Keyboard_A); break;
			case KeyInputTypes::Keyboard_S: OnPeripheralInput(Keyboard_S); break;
			case KeyInputTypes::Keyboard_D: OnPeripheralInput(Keyboard_D); break;
			case KeyInputTypes::Keyboard_F: OnPeripheralInput(Keyboard_F); break;
			case KeyInputTypes::Keyboard_G: OnPeripheralInput(Keyboard_G); break;
			case KeyInputTypes::Keyboard_H: OnPeripheralInput(Keyboard_H); break;
			case KeyInputTypes::Keyboard_J: OnPeripheralInput(Keyboard_J); break;
			case KeyInputTypes::Keyboard_K: OnPeripheralInput(Keyboard_K); break;
			case KeyInputTypes::Keyboard_L: OnPeripheralInput(Keyboard_L); break;
			case KeyInputTypes::Keyboard_Z: OnPeripheralInput(Keyboard_Z); break;
			case KeyInputTypes::Keyboard_X: OnPeripheralInput(Keyboard_X); break;
			case KeyInputTypes::Keyboard_C: OnPeripheralInput(Keyboard_C); break;
			case KeyInputTypes::Keyboard_V: OnPeripheralInput(Keyboard_V); break;
			case KeyInputTypes::Keyboard_B: OnPeripheralInput(Keyboard_B); break;
			case KeyInputTypes::Keyboard_N: OnPeripheralInput(Keyboard_N); break;
			case KeyInputTypes::Keyboard_M: OnPeripheralInput(Keyboard_M); break;

			case KeyInputTypes::Keyboard_1: OnPeripheralInput(Keyboard_1); break;
			case KeyInputTypes::Keyboard_2: OnPeripheralInput(Keyboard_2); break;
			case KeyInputTypes::Keyboard_3: OnPeripheralInput(Keyboard_3); break;
			case KeyInputTypes::Keyboard_4: OnPeripheralInput(Keyboard_4); break;
			case KeyInputTypes::Keyboard_5: OnPeripheralInput(Keyboard_5); break;
			case KeyInputTypes::Keyboard_6: OnPeripheralInput(Keyboard_6); break;
			case KeyInputTypes::Keyboard_7: OnPeripheralInput(Keyboard_7); break;
			case KeyInputTypes::Keyboard_8: OnPeripheralInput(Keyboard_8); break;
			case KeyInputTypes::Keyboard_9: OnPeripheralInput(Keyboard_9); break;
			case KeyInputTypes::Keyboard_0: OnPeripheralInput(Keyboard_0); break;
			case KeyInputTypes::Keyboard_Plus: OnPeripheralInput(Keyboard_Plus); break;
			case KeyInputTypes::Keyboard_Minus: OnPeripheralInput(Keyboard_Minus); break;

			case KeyInputTypes::Keyboard_Tab: OnPeripheralInput(Keyboard_Tab); break;
			case KeyInputTypes::Keyboard_CapsLock: OnPeripheralInput(Keyboard_CapsLock); break;
			case KeyInputTypes::Keyboard_Shift: OnPeripheralInput(Keyboard_Shift); break;
			case KeyInputTypes::Keyboard_Ctrl: OnPeripheralInput(Keyboard_Ctrl); break;
			case KeyInputTypes::Keyboard_Alt: OnPeripheralInput(Keyboard_Alt); break;
			case KeyInputTypes::Keyboard_Space: OnPeripheralInput(Keyboard_Space); break;
			case KeyInputTypes::Keyboard_Backspace: OnPeripheralInput(Keyboard_Backspace); break;

			case KeyInputTypes::Mouse_Left: OnPeripheralInput(Mouse_Left); break;
			case KeyInputTypes::Mouse_Middle: OnPeripheralInput(Mouse_Middle); break;
			case KeyInputTypes::Mouse_Right: OnPeripheralInput(Mouse_Right); break;
			case KeyInputTypes::Mouse_Scroll: OnPeripheralInput(Mouse_Scroll); break;


			default:
			{
				throw std::runtime_error("No such key exist!");
				break;
			}
		}
	}


	
}

