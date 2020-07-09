#pragma once
#include "glm/glm.hpp"
#include "Application.h"
#include <GLFW/glfw3.h>

namespace VoxelCore {

	class Input
	{
	public:
		static bool IsKeyPressed(int key);
		static bool IsMousePressed(int mouseButton);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

		static int GetInputMode(int mode);
		static void SetInputMode(int mode, int value);
	};
}