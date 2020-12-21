#pragma once

namespace inceptionengine
{
	struct MouseDeltaPos
	{
		MouseDeltaPos() = default;
		MouseDeltaPos(double dx, double dy) : deltaXPos(static_cast<float>(dx)), deltaYPos(static_cast<float>(dy)) { ; }

		float deltaXPos = 0.0f;
		float deltaYPos = 0.0f;
	};

	enum class KeyInputTypes
	{
		None,

		Keyboard_Q,
		Keyboard_W,
		Keyboard_E,
		Keyboard_R,
		Keyboard_T,
		Keyboard_Y,
		Keyboard_U,
		Keyboard_I,
		Keyboard_O,
		Keyboard_P,
		Keyboard_A,
		Keyboard_S,
		Keyboard_D,
		Keyboard_F,
		Keyboard_G,
		Keyboard_H,
		Keyboard_J,
		Keyboard_K,
		Keyboard_L,
		Keyboard_Z,
		Keyboard_X,
		Keyboard_C,
		Keyboard_V,
		Keyboard_B,
		Keyboard_N,
		Keyboard_M,

		Keyboard_1,
		Keyboard_2,
		Keyboard_3,
		Keyboard_4,
		Keyboard_5,
		Keyboard_6,
		Keyboard_7,
		Keyboard_8,
		Keyboard_9,
		Keyboard_0,
		Keyboard_Plus,
		Keyboard_Minus,
		Keyboard_Backspace,

		Keyboard_Space,
		Keyboard_Tab,
		Keyboard_Shift,
		Keyboard_Ctrl,
		Keyboard_Alt,
		Keyboard_CapsLock,

		Mouse_Left,
		Mouse_Middle,
		Mouse_Right,
		Mouse_Scroll
	};
}