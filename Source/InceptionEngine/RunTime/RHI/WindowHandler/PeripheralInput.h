
#pragma once

#include "ECS/Components/NativeScriptComponent/KeyInputTypes.h"

namespace inceptionengine
{



	struct KeyInput
	{
		KeyInput() = default;
		KeyInput(KeyInputTypes t, bool b) : inputKey(t), press(b) { ; }

		KeyInputTypes inputKey = KeyInputTypes::None;
		bool press = false;
	};

	struct PeripheralInput
	{
		std::queue<KeyInput> keyInputs;

		MouseDeltaPos mouseDeltaPos;

		JoyStick joystickInput;

		void Clear() 
		{ 
			std::queue<KeyInput> empty;
			keyInputs.swap(empty);
			mouseDeltaPos.deltaXPos = mouseDeltaPos.deltaYPos = 0.0f; 
		}
	};


}
