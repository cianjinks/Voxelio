#include "EditorApplication.h"

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window"), m_CameraController(1280.0f, 720.0f, 3.0f) {}

void EditorApplication::PreRender()
{
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);
	m_CameraController.SetFarPlane(1000.0f);
}

void EditorApplication::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);

	//m_Mesh.Refresh();

	MouseSelection();
	m_CameraController.HandleInput();
	unsigned int bufferData;
	glGenTextures(1, &bufferData);
	glBindTexture(GL_TEXURE_BUFFER)
	VoxelCore::Renderer::BeginScene(m_CameraController);
	VoxelCore::Renderer::DrawQuad(glm::vec3(0, 0, 0));
	VoxelCore::Renderer::EndScene();
}

void EditorApplication::ImGuiRender()
{
	ImGui::SetNextWindowPos(ImVec2(25, 25));
	ImGui::Begin("Info: ");
	ImGui::Text(("Frame Time: " + std::to_string(VoxelCore::Timestep::GetDeltaTimeMS()) + "ms | " + std::to_string((int)(1 / VoxelCore::Timestep::GetDeltaTime())) + "fps").c_str());
	ImGui::Text(("Vendor: " + std::string((char*)glGetString(GL_VENDOR))).c_str());
	ImGui::Text(("Renderer: " + std::string((char*)glGetString(GL_RENDERER))).c_str());
	ImGui::Text(("Version: " + std::string((char*)glGetString(GL_VERSION))).c_str());
	ImGui::Text("Renderer Data: ");
	ImGui::Text(("Draw Calls: " + std::to_string(VoxelCore::RendererData::DrawCalls)).c_str());
	ImGui::End();
}

void EditorApplication::OnKeyPress(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		VX_CORE_INFO("Removed block: 1, 1, 1");
		//m_Mesh.RemoveBlock(1, 1, 1);
	}
}

void EditorApplication::OnMouseMove(float xpos, float ypos)
{
	m_CameraController.OnMouseMove(xpos, ypos);
}

void EditorApplication::OnMouseScroll(float xoffset, float yoffset)
{
	m_CameraController.OnMouseScroll(xoffset, yoffset);
}

void EditorApplication::OnMouseClick(int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void EditorApplication::MouseSelection()
{
	float x = VoxelCore::Input::GetMouseX();
	float y = VoxelCore::Input::GetMouseY();
	glm::vec4 viewport = glm::vec4(0, 0, m_WindowWidth, m_WindowHeight);

	glm::vec3 startcoord = glm::unProject(glm::vec3(x, m_WindowHeight - y, m_CameraController.GetNearPlane()),
		m_CameraController.GetViewMatrix(), m_CameraController.GetProjectionMatrix(), viewport);

	glm::vec3 endcoord = glm::unProject(glm::vec3(x, m_WindowHeight - y, m_CameraController.GetFarPlane()),
		m_CameraController.GetViewMatrix(), m_CameraController.GetProjectionMatrix(), viewport);

	glm::vec3 raydir = startcoord - endcoord;
	VoxelCore::Ray ray(startcoord, raydir);

	//glm::vec3 bmin = m_Mesh.GetBlockMin(1, 1, 1);
	//glm::vec3 bmax = m_Mesh.GetBlockMax(1, 1, 1);
	//
	//if (VoxelCore::Ray::RayAABBCollision(ray, bmin, bmax))
	//{
	//	m_Mesh.SetBlockColor(1, 1, 1, glm::vec3(1.0f, 1.0f, 0.0f));
	//}
	//else 
	//{
	//	m_Mesh.SetBlockColor(1, 1, 1, glm::vec3(0.09f, 0.09f, 0.09f));
	//}
}