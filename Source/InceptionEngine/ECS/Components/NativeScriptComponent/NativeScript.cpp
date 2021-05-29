
#include "IE_PCH.h"

#include "NativeScript.h"

#include "ECS/World.h"
#include "RunTime/KeyInputCallbackRegistry.h"
#include "RunTime/RHI/WindowHandler/WindowHandler.h"

namespace inceptionengine
{
	NativeScript::NativeScript(EntityID entityID, std::reference_wrapper<World> world)
		:mEntityID(entityID), mWorld(world)
	{
		mKeyInputCallbackRegistry = std::make_unique<KeyInputCallbackRegistry>();
	}

	NativeScript::~NativeScript()
	{
	}
#ifdef __MSVCRT__
#define RegisterKeyInputCallback(X, Y) mKeyInputCallbackRegistry->OnPeripheralInput_##X = ##Y
#else
#define RegisterKeyInputCallback(X, Y) mKeyInputCallbackRegistry->OnPeripheralInput_##X = Y
#endif
	void NativeScript::BindKeyInputCallback(KeyInputTypes keyType, std::function<void(bool)> callback)
	{
		switch (keyType)
		{
			case KeyInputTypes::Keyboard_Q: RegisterKeyInputCallback(Keyboard_Q, callback); break;
			case KeyInputTypes::Keyboard_W: RegisterKeyInputCallback(Keyboard_W, callback); break;
			case KeyInputTypes::Keyboard_E: RegisterKeyInputCallback(Keyboard_E, callback); break;
			case KeyInputTypes::Keyboard_R: RegisterKeyInputCallback(Keyboard_R, callback); break;
			case KeyInputTypes::Keyboard_T: RegisterKeyInputCallback(Keyboard_T, callback); break;
			case KeyInputTypes::Keyboard_Y: RegisterKeyInputCallback(Keyboard_Y, callback); break;
			case KeyInputTypes::Keyboard_U: RegisterKeyInputCallback(Keyboard_U, callback); break;
			case KeyInputTypes::Keyboard_I: RegisterKeyInputCallback(Keyboard_I, callback); break;
			case KeyInputTypes::Keyboard_O: RegisterKeyInputCallback(Keyboard_O, callback); break;
			case KeyInputTypes::Keyboard_P: RegisterKeyInputCallback(Keyboard_P, callback); break;
			case KeyInputTypes::Keyboard_A: RegisterKeyInputCallback(Keyboard_A, callback); break;
			case KeyInputTypes::Keyboard_S: RegisterKeyInputCallback(Keyboard_S, callback); break;
			case KeyInputTypes::Keyboard_D: RegisterKeyInputCallback(Keyboard_D, callback); break;
			case KeyInputTypes::Keyboard_F: RegisterKeyInputCallback(Keyboard_F, callback); break;
			case KeyInputTypes::Keyboard_G: RegisterKeyInputCallback(Keyboard_G, callback); break;
			case KeyInputTypes::Keyboard_H: RegisterKeyInputCallback(Keyboard_H, callback); break;
			case KeyInputTypes::Keyboard_J: RegisterKeyInputCallback(Keyboard_J, callback); break;
			case KeyInputTypes::Keyboard_K: RegisterKeyInputCallback(Keyboard_K, callback); break;
			case KeyInputTypes::Keyboard_L: RegisterKeyInputCallback(Keyboard_L, callback); break;
			case KeyInputTypes::Keyboard_Z: RegisterKeyInputCallback(Keyboard_Z, callback); break;
			case KeyInputTypes::Keyboard_X: RegisterKeyInputCallback(Keyboard_X, callback); break;
			case KeyInputTypes::Keyboard_C: RegisterKeyInputCallback(Keyboard_C, callback); break;
			case KeyInputTypes::Keyboard_V: RegisterKeyInputCallback(Keyboard_V, callback); break;
			case KeyInputTypes::Keyboard_B: RegisterKeyInputCallback(Keyboard_B, callback); break;
			case KeyInputTypes::Keyboard_N: RegisterKeyInputCallback(Keyboard_N, callback); break;
			case KeyInputTypes::Keyboard_M: RegisterKeyInputCallback(Keyboard_M, callback); break;

			case KeyInputTypes::Keyboard_1: RegisterKeyInputCallback(Keyboard_1, callback); break;
			case KeyInputTypes::Keyboard_2: RegisterKeyInputCallback(Keyboard_2, callback); break;
			case KeyInputTypes::Keyboard_3: RegisterKeyInputCallback(Keyboard_3, callback); break;
			case KeyInputTypes::Keyboard_4: RegisterKeyInputCallback(Keyboard_4, callback); break;
			case KeyInputTypes::Keyboard_5: RegisterKeyInputCallback(Keyboard_5, callback); break;
			case KeyInputTypes::Keyboard_6: RegisterKeyInputCallback(Keyboard_6, callback); break;
			case KeyInputTypes::Keyboard_7: RegisterKeyInputCallback(Keyboard_7, callback); break;
			case KeyInputTypes::Keyboard_8: RegisterKeyInputCallback(Keyboard_8, callback); break;
			case KeyInputTypes::Keyboard_9: RegisterKeyInputCallback(Keyboard_9, callback); break;
			case KeyInputTypes::Keyboard_0: RegisterKeyInputCallback(Keyboard_0, callback); break;
			case KeyInputTypes::Keyboard_Plus: RegisterKeyInputCallback(Keyboard_Plus, callback); break;
			case KeyInputTypes::Keyboard_Minus: RegisterKeyInputCallback(Keyboard_Minus, callback); break;

			case KeyInputTypes::Keyboard_Tab: RegisterKeyInputCallback(Keyboard_Tab, callback); break;
			case KeyInputTypes::Keyboard_CapsLock: RegisterKeyInputCallback(Keyboard_CapsLock, callback); break;
			case KeyInputTypes::Keyboard_Shift: RegisterKeyInputCallback(Keyboard_Shift, callback); break;
			case KeyInputTypes::Keyboard_Ctrl: RegisterKeyInputCallback(Keyboard_Ctrl, callback); break;
			case KeyInputTypes::Keyboard_Alt: RegisterKeyInputCallback(Keyboard_Alt, callback); break;
			case KeyInputTypes::Keyboard_Space: RegisterKeyInputCallback(Keyboard_Space, callback); break;
			case KeyInputTypes::Keyboard_Backspace: RegisterKeyInputCallback(Keyboard_Backspace, callback); break;

			case KeyInputTypes::Mouse_Left: RegisterKeyInputCallback(Mouse_Left, callback); break;
			case KeyInputTypes::Mouse_Middle: RegisterKeyInputCallback(Mouse_Middle, callback); break;
			case KeyInputTypes::Mouse_Right: RegisterKeyInputCallback(Mouse_Right, callback); break;
			case KeyInputTypes::Mouse_Scroll: RegisterKeyInputCallback(Mouse_Scroll, callback); break;

			default:
			{
				throw std::runtime_error("unknown key value\n");
			}
		}
	}

	Entity const& NativeScript::GetEntity()
	{
		return mWorld.get().GetEntity(mEntityID);
	}






	


}

