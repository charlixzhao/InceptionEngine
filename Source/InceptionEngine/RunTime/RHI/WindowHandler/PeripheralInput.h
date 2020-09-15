
#pragma once

#include "ECS/Components/NativeScriptComponent/KeyInputTypes.h"
#include <queue>

namespace inceptionengine
{

	struct MouseDeltaPos
	{
		MouseDeltaPos() = default;
		MouseDeltaPos(double dx, double dy) : deltaXPos(dx), deltaYPos(dy) { ; }

		float deltaXPos = 0.0f;
		float deltaYPos = 0.0f;
	};

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

		void Clear() { keyInputs.swap(std::queue<KeyInput>()); mouseDeltaPos.deltaXPos = mouseDeltaPos.deltaYPos = 0.0; }
	};


}
