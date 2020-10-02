#include "voxelpch.h"
#include "Application.h"
#include "Renderer/Renderer.h"

namespace VoxelCore {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Init();
	}

	void Application::Init()
	{
		s_Instance = this;
	}
	void Application::Run()
	{
		m_Window = Window::Create();

		Renderer::Init();
		this->PreRender();
		while (!m_Window->WindowShouldClose())
		{
			VoxelCore::Timestep::Update();
			this->Render();
			VoxelCore::ImGuiHandler::NewFrame();
			this->ImGuiRender();
			VoxelCore::ImGuiHandler::Render();
			m_Window->Update();
		}
		Renderer::Shutdown();
		m_Window->Shutdown();
	}
}