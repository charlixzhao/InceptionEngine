#pragma once

/*
A wrapper class of glfw functionality
*/


struct GLFWwindow;

namespace inceptionengine
{

	class WindowHandler
	{
	public:
		struct MousePosition
		{
			/*
			We have to use double rather than float here because
			glfw api expects double* rather than float*
			*/
			double LastXPos = 0.0;
			double LastYPos = 0.0;
			double CurXPos = 0.0;
			double CurYPos = 0.0;
			double DeltaXPos = 0.0;
			double DeltaYPos = 0.0;

			/*
			consider add a speed field and pass in a delta time parameter when
			compute delta pos, and update speed.
			*/

			void ComputeDeltaPos();

			void SyncPosition();
		};

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
		
		static void KeyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static void MouseInputCallback(GLFWwindow* window, int button, int action, int mods);

	private:
		GLFWwindow* mWindow = nullptr;

		unsigned int mWindowHeight = 1600;

		unsigned int mWindowWidth = 900;

		MousePosition mMousePos;
	};
}
