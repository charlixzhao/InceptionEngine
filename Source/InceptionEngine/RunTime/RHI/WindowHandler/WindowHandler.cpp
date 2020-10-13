#include "WindowHandler.h"

#include "ThirdPartyLibrary/glfw/include/GLFW/glfw3.h"


namespace inceptionengine
{
	MouseDeltaPos MousePosition::GetDeltaPos()
	{
		return { curXPos - lastXPos, curYPos - lastYPos };
	}
	void MousePosition::SyncPosition()
	{
		lastXPos = curXPos;
		lastYPos = curYPos;
	}


	void WindowHandler::Initialize()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		mWindowWidth = 1600;
		mWindowHeight = 900;
		mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Inception Game", nullptr, nullptr);


		SetMouseVisibility(true);

		unsigned char pixels[16 * 16 * 4];
		std::memset(pixels, 0xff, sizeof(pixels));
		GLFWimage image;
		image.width = 16;
		image.height = 16;
		image.pixels = pixels;
		GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
		glfwSetCursor(mWindow, cursor);

		glfwSetWindowUserPointer(mWindow, this);

		glfwSetKeyCallback(mWindow, KeyboardInputCallback);

		glfwSetMouseButtonCallback(mWindow, MouseInputCallback);

		glfwSetScrollCallback(mWindow, MouseSrollCallback);

	}

	void WindowHandler::SetMouseVisibility(bool visible)
	{
		if (visible == true)
		{
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		glfwGetCursorPos(mWindow, &mMousePos.curXPos, &mMousePos.curYPos);

		glfwGetCursorPos(mWindow, &mMousePos.lastXPos, &mMousePos.lastYPos);

	}

	void WindowHandler::PollEvents()
	{
		glfwPollEvents();
		glfwGetCursorPos(mWindow, &mMousePos.curXPos, &mMousePos.curYPos);
		mPeripheralInput.mouseDeltaPos = mMousePos.GetDeltaPos();
		mMousePos.SyncPosition();
	}

	bool WindowHandler::WindowClosed()
	{
		return glfwWindowShouldClose(mWindow);
	}

	void WindowHandler::DestroyWindow()
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	void WindowHandler::KeyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WindowHandler* pWindonHandler = static_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		KeyInput keyboardInput = pWindonHandler->SwitchGLFWKeyboardInput(key, action);
		if (keyboardInput.inputKey != KeyInputTypes::None)
		{
			pWindonHandler->mPeripheralInput.keyInputs.push(keyboardInput);
		}
	}

	void WindowHandler::MouseInputCallback(GLFWwindow* window, int button, int action, int mods)
	{
		WindowHandler* pWindonHandler = static_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		KeyInput mouseInput = pWindonHandler->SwitchGLFWMouseInput(button, action);
		if (mouseInput.inputKey != KeyInputTypes::None)
		{
			pWindonHandler->mPeripheralInput.keyInputs.push(mouseInput);
		}
	}

	void WindowHandler::MouseSrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		WindowHandler* pWindonHandler = static_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
		pWindonHandler->mPeripheralInput.keyInputs.push(KeyInput(KeyInputTypes::Mouse_Scroll, yoffset > 0));
	}

	KeyInput WindowHandler::SwitchGLFWKeyboardInput(int key, int action)
	{
		if (action == GLFW_REPEAT)
		{
			return KeyInput();
		}
		
		bool press = action == GLFW_PRESS;
		
		switch (key)
		{
			case GLFW_KEY_Q: return KeyInput(KeyInputTypes::Keyboard_Q, press);
			case GLFW_KEY_W: return KeyInput(KeyInputTypes::Keyboard_W, press);
			case GLFW_KEY_E: return KeyInput(KeyInputTypes::Keyboard_E, press);
			case GLFW_KEY_R: return KeyInput(KeyInputTypes::Keyboard_R, press);
			case GLFW_KEY_T: return KeyInput(KeyInputTypes::Keyboard_T, press);
			case GLFW_KEY_Y: return KeyInput(KeyInputTypes::Keyboard_Y, press);
			case GLFW_KEY_U: return KeyInput(KeyInputTypes::Keyboard_U, press);
			case GLFW_KEY_I: return KeyInput(KeyInputTypes::Keyboard_I, press);
			case GLFW_KEY_O: return KeyInput(KeyInputTypes::Keyboard_O, press);
			case GLFW_KEY_P: return KeyInput(KeyInputTypes::Keyboard_P, press);
			case GLFW_KEY_A: return KeyInput(KeyInputTypes::Keyboard_A, press);
			case GLFW_KEY_S: return KeyInput(KeyInputTypes::Keyboard_S, press);
			case GLFW_KEY_D: return KeyInput(KeyInputTypes::Keyboard_D, press);
			case GLFW_KEY_F: return KeyInput(KeyInputTypes::Keyboard_F, press);
			case GLFW_KEY_G: return KeyInput(KeyInputTypes::Keyboard_G, press);
			case GLFW_KEY_H: return KeyInput(KeyInputTypes::Keyboard_H, press);
			case GLFW_KEY_J: return KeyInput(KeyInputTypes::Keyboard_J, press);
			case GLFW_KEY_K: return KeyInput(KeyInputTypes::Keyboard_K, press);
			case GLFW_KEY_L: return KeyInput(KeyInputTypes::Keyboard_L, press);
			case GLFW_KEY_Z: return KeyInput(KeyInputTypes::Keyboard_Z, press);
			case GLFW_KEY_X: return KeyInput(KeyInputTypes::Keyboard_X, press);
			case GLFW_KEY_C: return KeyInput(KeyInputTypes::Keyboard_C, press);
			case GLFW_KEY_V: return KeyInput(KeyInputTypes::Keyboard_V, press);
			case GLFW_KEY_B: return KeyInput(KeyInputTypes::Keyboard_B, press);
			case GLFW_KEY_N: return KeyInput(KeyInputTypes::Keyboard_N, press);
			case GLFW_KEY_M: return KeyInput(KeyInputTypes::Keyboard_M, press);

			case GLFW_KEY_1: return KeyInput(KeyInputTypes::Keyboard_1, press);
			case GLFW_KEY_2: return KeyInput(KeyInputTypes::Keyboard_2, press);
			case GLFW_KEY_3: return KeyInput(KeyInputTypes::Keyboard_3, press);
			case GLFW_KEY_4: return KeyInput(KeyInputTypes::Keyboard_4, press);
			case GLFW_KEY_5: return KeyInput(KeyInputTypes::Keyboard_5, press);
			case GLFW_KEY_6: return KeyInput(KeyInputTypes::Keyboard_6, press);
			case GLFW_KEY_7: return KeyInput(KeyInputTypes::Keyboard_7, press);
			case GLFW_KEY_8: return KeyInput(KeyInputTypes::Keyboard_8, press);
			case GLFW_KEY_9: return KeyInput(KeyInputTypes::Keyboard_9, press);
			case GLFW_KEY_0: return KeyInput(KeyInputTypes::Keyboard_9, press);

			case GLFW_KEY_MINUS: return KeyInput(KeyInputTypes::Keyboard_Minus, press);

			case GLFW_KEY_TAB: return KeyInput(KeyInputTypes::Keyboard_Tab, press);
			case GLFW_KEY_CAPS_LOCK: return KeyInput(KeyInputTypes::Keyboard_CapsLock, press);
			case GLFW_KEY_LEFT_SHIFT: return KeyInput(KeyInputTypes::Keyboard_Shift, press);
			case GLFW_KEY_RIGHT_SHIFT: return KeyInput(KeyInputTypes::Keyboard_Shift, press);
			case GLFW_KEY_LEFT_CONTROL: return KeyInput(KeyInputTypes::Keyboard_Ctrl, press);
			case GLFW_KEY_RIGHT_CONTROL: return KeyInput(KeyInputTypes::Keyboard_Ctrl, press);
			case GLFW_KEY_LEFT_ALT: return KeyInput(KeyInputTypes::Keyboard_Alt, press);
			case GLFW_KEY_RIGHT_ALT: return KeyInput(KeyInputTypes::Keyboard_Alt, press);
			case GLFW_KEY_SPACE: return KeyInput(KeyInputTypes::Keyboard_Space, press);
			case GLFW_KEY_BACKSPACE: return KeyInput(KeyInputTypes::Keyboard_Backspace, press);

			default: return KeyInput();
		}
	}

	KeyInput WindowHandler::SwitchGLFWMouseInput(int button, int action)
	{
		if (action == GLFW_REPEAT)
		{
			return KeyInput();
		}
		bool press = action == GLFW_PRESS;
		switch (button)
		{
			case GLFW_MOUSE_BUTTON_LEFT: return { KeyInputTypes::Mouse_Left, press };
			case GLFW_MOUSE_BUTTON_MIDDLE: return { KeyInputTypes::Mouse_Middle, press };
			case GLFW_MOUSE_BUTTON_RIGHT: return { KeyInputTypes::Mouse_Right, press };
			default: return KeyInput();
		}
	}
}



