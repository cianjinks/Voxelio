#pragma once

#include "Core/Window.h"
#include "Core/Timestep.h"
#include "Core/ImGui/ImGuiHandler.h"

namespace VoxelCore {

	class Application
	{
	private:
		static Application* s_Instance;
		std::unique_ptr<Window> m_Window;
	public:
		Application();
		void Init();
		void Run();
		Window& GetWindow() { return *m_Window; };

		virtual ~Application() = default;
		
		virtual void PreRender() = 0;
		virtual void Render() = 0;
		virtual void ImGuiRender() = 0;

		virtual float GetWindowWidth() = 0;
		virtual float GetWindowHeight() = 0;
		virtual char* GetWindowName() = 0;

		virtual void OnKeyPress(int key, int scancode, int action, int mods) = 0;
		virtual void OnMouseMove(float xpos, float ypos) = 0;
		virtual void OnMouseScroll(float xoffset, float yoffset) = 0;
		virtual void OnMouseClick(int button, int action, int mods) = 0;
		virtual void OnResize(int width, int height) = 0;

		static Application& Get() { return *s_Instance; };
	};

	Application* CreateApplication();
}