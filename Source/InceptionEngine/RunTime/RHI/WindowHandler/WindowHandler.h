#pragma once

/*
A wrapper class of glfw functionality
*/

#include "PeripheralInput.h"


struct GLFWwindow;

namespace inceptionengine
{

	struct MousePosition
	{
	public:
		/*
		We have to use double rather than float here because
		glfw api expects double* rather than float*
		*/
		double lastXPos = 0.0;
		double lastYPos = 0.0;
		double curXPos = 0.0;
		double curYPos = 0.0;


		/*
		consider add a speed field and pass in a delta time parameter when
		compute delta pos, and update speed.
		*/

		MouseDeltaPos GetDeltaPos();

		void SyncPosition();
	};



	class WindowHandler
	{

	public:
		WindowHandler() = default;

		~WindowHandler() = default;

		void Initialize();

		GLFWwindow*& GetWindow() { return mWindow; }

		void SetMouseVisibility(bool visible);

		void PollEvents();

		bool WindowClosed();

		void DestroyWindow();

		unsigned int GetWindowHeight() const { return mWindowHeight; }

		unsigned int GetWindowWidth() const { return mWindowWidth; }

		PeripheralInput GetAndClearPeripheralInput() { PeripheralInput copy = mPeripheralInput; mPeripheralInput.Clear(); return copy; }

	private:
		static void KeyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void MouseInputCallback(GLFWwindow* window, int button, int action, int mods);

		static void MouseSrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		KeyInput SwitchGLFWKeyboardInput(int key, int action);

		KeyInput SwitchGLFWMouseInput(int button, int action);

	private:
		GLFWwindow* mWindow = nullptr;

		unsigned int mWindowHeight = 1600;

		unsigned int mWindowWidth = 900;

		MousePosition mMousePos;

		PeripheralInput mPeripheralInput;
	};
}
