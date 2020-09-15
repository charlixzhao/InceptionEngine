#pragma once

#include <functional>

#define DefineKeyInputCallback(X) std::function<void(bool)> OnPeripheralInput_##X = [](bool press) {; };

namespace inceptionengine
{
	struct KeyInputCallbackRegistry
	{
		DefineKeyInputCallback(Keyboard_Q);
		DefineKeyInputCallback(Keyboard_W);
		DefineKeyInputCallback(Keyboard_E);
		DefineKeyInputCallback(Keyboard_R);
		DefineKeyInputCallback(Keyboard_T);
		DefineKeyInputCallback(Keyboard_Y);
		DefineKeyInputCallback(Keyboard_U);
		DefineKeyInputCallback(Keyboard_I);
		DefineKeyInputCallback(Keyboard_O);
		DefineKeyInputCallback(Keyboard_P);
		DefineKeyInputCallback(Keyboard_A);
		DefineKeyInputCallback(Keyboard_S);
		DefineKeyInputCallback(Keyboard_D);
		DefineKeyInputCallback(Keyboard_F);
		DefineKeyInputCallback(Keyboard_G);
		DefineKeyInputCallback(Keyboard_H);
		DefineKeyInputCallback(Keyboard_J);
		DefineKeyInputCallback(Keyboard_K);
		DefineKeyInputCallback(Keyboard_L);
		DefineKeyInputCallback(Keyboard_Z);
		DefineKeyInputCallback(Keyboard_X);
		DefineKeyInputCallback(Keyboard_C);
		DefineKeyInputCallback(Keyboard_V);
		DefineKeyInputCallback(Keyboard_B);
		DefineKeyInputCallback(Keyboard_N);
		DefineKeyInputCallback(Keyboard_M);

		DefineKeyInputCallback(Keyboard_1);
		DefineKeyInputCallback(Keyboard_2);
		DefineKeyInputCallback(Keyboard_3);
		DefineKeyInputCallback(Keyboard_4);
		DefineKeyInputCallback(Keyboard_5);
		DefineKeyInputCallback(Keyboard_6);
		DefineKeyInputCallback(Keyboard_7);
		DefineKeyInputCallback(Keyboard_8);
		DefineKeyInputCallback(Keyboard_9);
		DefineKeyInputCallback(Keyboard_0);
		DefineKeyInputCallback(Keyboard_Plus);
		DefineKeyInputCallback(Keyboard_Minus);

		DefineKeyInputCallback(Keyboard_Tab);
		DefineKeyInputCallback(Keyboard_CapsLock);
		DefineKeyInputCallback(Keyboard_Shift);
		DefineKeyInputCallback(Keyboard_Ctrl);
		DefineKeyInputCallback(Keyboard_Alt);
		DefineKeyInputCallback(Keyboard_Space);
		DefineKeyInputCallback(Keyboard_Backspace);

		DefineKeyInputCallback(Mouse_Left);
		DefineKeyInputCallback(Mouse_Middle);
		DefineKeyInputCallback(Mouse_Right);
		DefineKeyInputCallback(Mouse_Scroll);

	};
}