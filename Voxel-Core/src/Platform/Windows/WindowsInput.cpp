#include "voxelpch.h"
#include "Core/Input.h"

namespace VoxelCore {

	bool Input::IsKeyPressed(int key)
	{
		return glfwGetKey((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), key) == GLFW_PRESS;
	}

	bool Input::IsMousePressed(int mouseButton)
	{
		return glfwGetMouseButton((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), mouseButton) == GLFW_PRESS;
	}

	int Input::GetInputMode(int mode)
	{
		return glfwGetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), mode);
	}

	void Input::SetInputMode(int mode, int value)
	{
		glfwSetInputMode((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), mode, value);
	}

	glm::vec2 Input::GetMousePosition()
	{
		glm::dvec2 cursorPos;
		glfwGetCursorPos((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), &cursorPos.x, &cursorPos.y);
		return glm::vec2((float)cursorPos.x, (float)cursorPos.y);
	}

	void Input::SetMousePosition(glm::dvec2& pos)
	{
		glfwSetCursorPos((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), pos.x, pos.y);
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
}