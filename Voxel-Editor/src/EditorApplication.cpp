#include "EditorApplication.h"

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window"), m_OctreeCameraController(1280.0f, 720.0f, 5.0f), m_OctreeOrthoCamera(1280.0f, 720.0f), m_Octree(s_OctreeLevels) {}

void EditorApplication::PreRender()
{
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);

	m_FBOData.Width = m_WindowWidth;
	m_FBOData.Height = m_WindowHeight;
	m_FBO = VoxelCore::FrameBuffer::Create(m_FBOData);

	// Add Default Color to slot 1
	m_Palette.AddColor(m_DefaultColor);
	m_Palette.AddColor(VoxelCore::VoxelColor("Blue Color", 0.0f, 0.0f, 1.0f, 1.0f));
}

void EditorApplication::Render()
{
	// Catch Resizes
	if (VoxelCore::FrameBufferData data = m_FBO->GetFrameBufferData();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // Zero size frambuffer is not valid 
		(data.Width != m_ViewportSize.x || data.Height != m_ViewportSize.y))
	{
		VX_EDITOR_INFO("Viewport Size: {0}, {1}", m_ViewportSize.x, m_ViewportSize.y);

		m_FBO->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_OctreeCameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_OctreeOrthoCamera.Resize(m_ViewportSize.x, m_ViewportSize.y);
	}

	// Mouse selection and Camera Controller input
	MouseSelection();
	m_OctreeCameraController.HandleInput();

	// Main Rendering
	m_FBO->Bind();

	glClearColor(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	VoxelCore::Renderer::BeginScene(m_OctreeCameraController, m_OctreeOrthoCamera);
	VoxelCore::Renderer::DrawOctree(m_Octree, m_Palette, m_OctreeOrthoCamera.GetAspectRatio());
	VoxelCore::Renderer::EndScene();
	m_FBO->Unbind();

	// Clear Screen
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Tools Rendering (Shows preview of current tools action)
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
	ImGui::InputText("Name", &m_PaletteEditorColor.name, 0);
	ImGui::ColorPicker4("", &m_PaletteEditorColor.color.x, 0);

	if (ImGui::Button("Add Color"))
	{
		if (!m_Palette.AddColor(m_PaletteEditorColor))
		{
			m_PaletteFull = true;
		}
	}

	ImGui::Separator();
	if (m_PaletteFull)
	{
		ImGui::Text(("[WARNING] Palette FULL (" + std::to_string(VoxelCore::VoxelColorPalette::MAX_PALETTE_SIZE) + " colors)").c_str());
	}
	ImGui::Text("Color Palette: ");

	const float squareDim = 50.0f;
	int squaresPerLine = (ImGui::GetWindowWidth() / squareDim) - 1;
	auto colors = m_Palette.GetColors();
	int count = 1;

	for (int pc = 0; pc < colors.size(); pc++)
	{

		if (count == squaresPerLine)
		{
			if (ImGui::ColorButton(colors[pc].name.c_str(), ImVec4(colors[pc].color.r, colors[pc].color.g, colors[pc].color.b, colors[pc].color.a), 0, ImVec2(50, 50)))
			{
				m_CurrentSelectedColor = colors[pc];
				m_CurrentSelectedColorIndex = pc;
				VX_EDITOR_INFO("Changed Selected Color to: {0} with index {1}", colors[pc].name, pc);
			}
			count = 0;
		}
		else
		{
			if(ImGui::ColorButton(colors[pc].name.c_str(), ImVec4(colors[pc].color.r, colors[pc].color.g, colors[pc].color.b, colors[pc].color.a), 0, ImVec2(50, 50)))
			{
				m_CurrentSelectedColor = colors[pc];
				m_CurrentSelectedColorIndex = pc;
				VX_EDITOR_INFO("Changed Selected Color to : {0} with index {1}", colors[pc].name, pc);
			}
			ImGui::SameLine();
		}
		count++;
	}
	
	ImGui::End();

	// Tools
	ImGui::Begin("Tools");

	float toolsWidth = ImGui::GetWindowWidth();

	ImGui::Text("Currently Selected Color: ");
	ImGui::ColorButton(m_CurrentSelectedColor.name.c_str(), 
		ImVec4(m_CurrentSelectedColor.color.r, m_CurrentSelectedColor.color.g, m_CurrentSelectedColor.color.b, m_CurrentSelectedColor.color.a), 0, ImVec2(toolsWidth / 2, toolsWidth / 2));
	ImGui::Text("Selected Voxel:");
	ImGui::InputInt3("", &m_SelectedVoxel.x);

	// Keep selected voxel in bounds
	int upperVoxelBound = std::pow(2, s_OctreeLevels) - 1;
	int lowerVoxelBound = 0;
	for (int i = 0; i < 3; i++)
	{
		if (m_SelectedVoxel[i] > upperVoxelBound) { m_SelectedVoxel[i] = upperVoxelBound; }
		if (m_SelectedVoxel[i] < lowerVoxelBound) { m_SelectedVoxel[i] = lowerVoxelBound; }
	}

	// Buttons for manipulating voxels
	if (ImGui::Button("Apply Color"))
	{
		m_Octree.SetColorIndex(m_SelectedVoxel.x, m_SelectedVoxel.y, m_SelectedVoxel.z, (uint64_t)m_CurrentSelectedColorIndex);
		VX_EDITOR_INFO("Selected Voxel: {0}, {1}, {2} Color Index: {3}", m_SelectedVoxel.x, m_SelectedVoxel.y, m_SelectedVoxel.z, (uint64_t)m_CurrentSelectedColorIndex);
	}
	if (ImGui::Button("Enable"))
	{
		m_Octree.Activate(m_SelectedVoxel.x, m_SelectedVoxel.y, m_SelectedVoxel.z);
	}
	if (ImGui::Button("Disable"))
	{
		m_Octree.Deactivate(m_SelectedVoxel.x, m_SelectedVoxel.y, m_SelectedVoxel.z);
	}

	float toolWindowWidth = ImGui::GetWindowWidth();
	float dim = (toolWindowWidth / 2) - (0.1 * toolWindowWidth);
	if (ImGui::Button("Color Tool", ImVec2(dim, dim)))
	{
		// This colors voxels
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase Tool", ImVec2(dim, dim)))
	{
		// This tool erases voxels
	}
	if (ImGui::Button("Build Tool", ImVec2(dim, dim)))
	{
		// This tool adds new voxels
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

	ImGui::ShowDemoWindow();
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
		m_Octree.SetColorIndex(3, 3, 3, 0x1);
	}
}

void EditorApplication::OnMouseMove(float xpos, float ypos)
{
	m_OctreeCameraController.OnMouseMove(xpos, ypos);
}

void EditorApplication::OnMouseScroll(float xoffset, float yoffset)
{
	m_OctreeCameraController.OnMouseScroll(xoffset, yoffset);
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
	
	glm::vec3 startcoord = glm::unProject(glm::vec3(x, m_WindowHeight - y, m_OctreeCameraController.GetNearPlane()),
		m_OctreeCameraController.GetViewMatrix(), m_OctreeCameraController.GetProjectionMatrix(), viewport);
	
	glm::vec3 endcoord = glm::unProject(glm::vec3(x, m_WindowHeight - y, m_OctreeCameraController.GetFarPlane()),
		m_OctreeCameraController.GetViewMatrix(), m_OctreeCameraController.GetProjectionMatrix(), viewport);
	
	glm::vec3 raydir = startcoord - endcoord;
	VoxelCore::Ray ray(startcoord, raydir);

	//glm::vec2 v_Pos;
	//if (x > m_WindowWidth / 2)
	//{
	//	v_Pos.x = (x / m_WindowWidth) / 2;
	//}
	//else
	//{
	//	v_Pos.x = -(x / m_WindowWidth) / 2;
	//}
	//
	//y = m_WindowHeight - y;
	//if (y > m_WindowHeight / 2)
	//{
	//	v_Pos.y = (y / m_WindowHeight) / 2;
	//}
	//else
	//{
	//	v_Pos.y = -(y / m_WindowHeight) / 2;
	//}
	//
	//VoxelCore::Ray ray(glm::vec3(0.0f, 0.0f, -m_CameraController.GetCameraRadius()), v_Pos);

	// Quick test for cube at index 0, 0, 0 with octree of resolution 2
	glm::vec3 bmin = glm::vec3(-1.0f);
	glm::vec3 bmax = glm::vec3(-0.5f);

	if (VoxelCore::Ray::RayAABBCollision(ray, bmin, bmax))
	{
		//m_Octree.SetColorIndex(0, 0, 0, 1);
	}
	else
	{
		//m_Octree.SetColorIndex(0, 0, 0, 0);
	}
}