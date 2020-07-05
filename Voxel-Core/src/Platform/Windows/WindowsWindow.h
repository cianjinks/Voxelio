#pragma once
#include "Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VoxelCore {

	class WindowsWindow : public Window
	{
	private:
		static int s_NumWindows;
		static bool s_GLFWInitialised;
		static bool s_GLADInitialised;

		GLFWwindow* m_Window;
	public:
		WindowsWindow();
		virtual ~WindowsWindow();

		void Init() override;
		void Update() override;
		void Shutdown() override;
		bool WindowShouldClose() override;

		void* GetNativeWindow() override { return m_Window; };
	};
}