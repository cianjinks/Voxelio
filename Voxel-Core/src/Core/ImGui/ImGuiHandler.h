#pragma once
#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"

namespace VoxelCore {

	class ImGuiHandler
	{
	public:
		static void Init(GLFWwindow* window);
		static void NewFrame();
		static void Render();
	};
}