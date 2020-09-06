#include "voxelpch.h"

#include "WindowsWindow.h"
#include "Core/Application.h"

namespace VoxelCore {

	int WindowsWindow::s_NumWindows = 0;
	bool WindowsWindow::s_GLADInitialised = false;
	bool WindowsWindow::s_GLFWInitialised = false;

	WindowsWindow::WindowsWindow()
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init()
	{
		Application& application = Application::Get();

		if (!s_GLFWInitialised)
		{
			if (!glfwInit())
			{
				VX_CORE_CRITICAL("Failed to initialise GLFW");
			}
			s_GLFWInitialised = true;
			VX_CORE_INFO("Initialised GLFW");
		}

		m_Window = glfwCreateWindow((int)application.GetWindowWidth(), (int)application.GetWindowHeight(), application.GetWindowName(), NULL, NULL);
		if (!m_Window)
		{
			if (s_NumWindows == 0)
			{
				glfwTerminate();
			}
			VX_CORE_CRITICAL("Failed to create GLFW window: {}", application.GetWindowName());
		}
		s_NumWindows++;
		VX_CORE_INFO("Created GLFW window: {}", application.GetWindowName());

		glfwMakeContextCurrent(m_Window);

		if (!s_GLADInitialised)
		{
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			if (!status)
			{
				VX_CORE_CRITICAL("Failed to initialise GLAD");
			}
			s_GLADInitialised = true;
			VX_CORE_INFO("Initialised GLAD");
		}

		glfwSetWindowUserPointer(m_Window, &application);
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			VoxelCore::Application& application = *(VoxelCore::Application*)glfwGetWindowUserPointer(window);
			application.OnKeyPress(key, scancode, action, mods);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			VoxelCore::Application& application = *(VoxelCore::Application*)glfwGetWindowUserPointer(window);
			application.OnMouseMove((float)xpos, (float)ypos);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			VoxelCore::Application& application = *(VoxelCore::Application*)glfwGetWindowUserPointer(window);
			application.OnMouseScroll((float)xoffset, (float)yoffset);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			VoxelCore::Application& application = *(VoxelCore::Application*)glfwGetWindowUserPointer(window);
			application.OnMouseClick(button, action, mods);
		});

		glViewport(0, 0, (GLsizei)application.GetWindowWidth(), (GLsizei)application.GetWindowHeight());
		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			VoxelCore::Application& application = *(VoxelCore::Application*)glfwGetWindowUserPointer(window);
			application.OnResize(width, height);
		});

		VoxelCore::ImGuiHandler::Init(m_Window);
	}

	void WindowsWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		s_NumWindows--;
		if (s_NumWindows == 0)
		{
			glfwTerminate();
		}
	}

	bool WindowsWindow::WindowShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}
}