#include "EditorApplication.h"

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window"), m_CameraController(1280.0f, 720.0f, 2.0f), m_Octree(3) {}

void EditorApplication::PreRender()
{
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);

	m_FBOData.Width = m_WindowWidth;
	m_FBOData.Height = m_WindowHeight;
	m_FBO = VoxelCore::FrameBuffer::Create(m_FBOData);
}

void EditorApplication::Render()
{
	// Catch Resizes
	if (VoxelCore::FrameBufferData data = m_FBO->GetFrameBufferData();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(data.Width != m_ViewportSize.x || data.Height != m_ViewportSize.y))
	{
		VX_CORE_INFO("Viewport Size: {0}, {1}", m_ViewportSize.x, m_ViewportSize.y);
		m_FBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
	}

	MouseSelection();
	m_CameraController.HandleInput();

	m_FBO->Bind();

	glClearColor(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	VoxelCore::Renderer::BeginScene(m_CameraController);
	VoxelCore::Renderer::DrawQuad(glm::vec3(0, 0, 0));
	VoxelCore::Renderer::DrawOctree(m_Octree);
	VoxelCore::Renderer::EndScene();
	m_FBO->Unbind();

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EditorApplication::ImGuiRender()
{
	/**ImGui::SetNextWindowPos(ImVec2(25, 25));
	ImGui::Begin("Info: ");
	ImGui::Text(("Frame Time: " + std::to_string(VoxelCore::Timestep::GetDeltaTimeMS()) + "ms | " + std::to_string((int)(1 / VoxelCore::Timestep::GetDeltaTime())) + "fps").c_str());
	ImGui::Text(("Vendor: " + std::string((char*)glGetString(GL_VENDOR))).c_str());
	ImGui::Text(("Renderer: " + std::string((char*)glGetString(GL_RENDERER))).c_str());
	ImGui::Text(("Version: " + std::string((char*)glGetString(GL_VERSION))).c_str());
	ImGui::Text("Renderer Data: ");
	ImGui::Text(("Draw Calls: " + std::to_string(VoxelCore::RendererData::DrawCalls)).c_str());

	uint32_t textureID = m_FBO->GetFrameBufferColorData();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_WindowWidth, m_WindowHeight }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();**/

	// Note: Switch this to true to enable dockspace
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			// Here we actually build out this option
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Begin("Info: ");
	ImGui::Text(("Frame Time: " + std::to_string(VoxelCore::Timestep::GetDeltaTimeMS()) + "ms | " + std::to_string((int)(1 / VoxelCore::Timestep::GetDeltaTime())) + "fps").c_str());
	ImGui::Text(("Vendor: " + std::string((char*)glGetString(GL_VENDOR))).c_str());
	ImGui::Text(("Renderer: " + std::string((char*)glGetString(GL_RENDERER))).c_str());
	ImGui::Text(("Version: " + std::string((char*)glGetString(GL_VERSION))).c_str());
	ImGui::Text("Renderer Data: ");
	ImGui::Text(("Draw Calls: " + std::to_string(VoxelCore::RendererData::DrawCalls)).c_str());
	ImGui::End();

	// Palette Editor
	ImGui::Begin("Palette Editor");
	//ImGui::InputText("Name", );
	ImGui::ColorPicker4("", &m_PaletteEditorColor.color.x, 0);

	if (ImGui::Button("Add Color"))
	{
		
		m_Palette.GetColors().emplace_back(m_PaletteEditorColor);
	}

	ImGui::Separator();
	ImGui::Text("Color Palette: ");

	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	for (auto& pc : m_Palette.GetColors())
	{
		//drawlist->AddRectFilled(ImVec2(0, 0), ImVec2(40, 40), ImU32(ImColor(color.color.x, color.color.y, color.color.z, color.color.a)), 0);
		ImGui::Text((pc.name + ": " + std::to_string(pc.color.r) + " " + std::to_string(pc.color.g) + " " + std::to_string(pc.color.b) + " " + std::to_string(pc.color.a)).c_str());
	}
	
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	//m_ViewportFocused = ImGui::IsWindowFocused();
	//m_ViewportHovered = ImGui::IsWindowHovered();
	//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	uint64_t textureID = m_FBO->GetFrameBufferColorData();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();

}

void EditorApplication::OnKeyPress(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		m_Octree.Deactivate(0, 0, 0);
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		m_Octree.Deactivate(0, 0, 1); 
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		m_Octree.Deactivate(0, 1, 0);
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		m_Octree.Deactivate(0, 1, 1);
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		m_Octree.Deactivate(3, 3, 2);
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		m_Octree.Deactivate(3, 3, 3);
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		m_Octree.SetColorIndex(7, 7, 7, 0x1);
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
	//if (!io.WantCaptureMouse) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	//}
}

void EditorApplication::OnResize(int width, int height)
{
	//glViewport(0, 0, width, height);
	//m_FBO->Resize(width, height);
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
}