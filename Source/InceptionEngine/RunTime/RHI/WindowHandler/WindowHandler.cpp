#include "WindowHandler.h"

#include "External/glfw/include/GLFW/glfw3.h"

#include <functional>
#include <cstring>

namespace inceptionengine
{
	void WindowHandler::MousePosition::ComputeDeltaPos()
	{
		DeltaXPos = CurXPos - LastXPos;
		DeltaYPos = CurYPos - LastYPos;
	}
	void WindowHandler::MousePosition::SyncPosition()
	{
		LastXPos = CurXPos;
		LastYPos = CurYPos;
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

		glfwGetCursorPos(mWindow, &mMousePos.CurXPos, &mMousePos.CurYPos);

		glfwGetCursorPos(mWindow, &mMousePos.LastXPos, &mMousePos.LastYPos);

	}

	void WindowHandler::PollEvents()
	{
		glfwPollEvents();
		glfwGetCursorPos(mWindow, &mMousePos.CurXPos, &mMousePos.CurYPos);
		mMousePos.ComputeDeltaPos();
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
	}

	void WindowHandler::MouseInputCallback(GLFWwindow* window, int button, int action, int mods)
	{
	}
}



