#include "EditorApplication.h"

bool EditorApplication::s_NewModel = false;
bool EditorApplication::s_LoadModel = false;
bool EditorApplication::s_CloseLoadModel = false;
bool EditorApplication::s_SaveModel = false;
bool EditorApplication::s_CloseSaveModel = false;

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window"), m_OctreeCameraController(1280.0f, 720.0f, 5.0f), m_OctreeOrthoCamera(1280.0f, 720.0f) {}

void EditorApplication::PreRender()
{
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);

	m_FBOData.Width = m_WindowWidth;
	m_FBOData.Height = m_WindowHeight;
	m_FBO = VoxelCore::FrameBuffer::Create(m_FBOData);

	m_FileBrowser.SetTypeFilters({".vio"});
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
	m_ToolHandler.ToolHover(m_Octree, GenerateMouseRay(), m_CurrentSelectedColorIndex);

}

void EditorApplication::ImGuiRender()
{
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
			ImGui::MenuItem("New Model", NULL, &s_NewModel);
			ImGui::MenuItem("Save Model", NULL, &s_SaveModel);
			ImGui::MenuItem("Load Model", NULL, &s_LoadModel);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// New Model
	if (s_NewModel)
	{
		m_Octree.ReloadOctree();
		s_NewModel = false;
	}

	// Saving / Loading Models
	if (s_SaveModel)
	{
		s_CloseSaveModel = s_SaveModel;
		m_FileBrowser.flags_ |= ImGuiFileBrowserFlags_EnterNewFilename;
		m_FileBrowser.Open();
		s_SaveModel = false;
	}

	if (s_LoadModel)
	{
		s_CloseLoadModel = s_LoadModel;
		m_FileBrowser.flags_ = 0;
		m_FileBrowser.Open();
		s_LoadModel = false;
	}

	m_FileBrowser.Display();
	if (m_FileBrowser.HasSelected())
	{
		if (s_CloseSaveModel)
		{
			SaveToFile(m_FileBrowser.GetSelected().string());
			s_CloseSaveModel = false;
		}
		if (s_CloseLoadModel)
		{
			LoadFromFile(m_FileBrowser.GetSelected().string());
			s_CloseLoadModel = false;
		}
		m_FileBrowser.ClearSelected();
	}


	// Info Panel

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
	int upperVoxelBound = std::pow(2, m_Octree.s_OctreeLevels) - 1;
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
		m_ToolHandler.SetActiveTool(VoxelCore::ToolType::COLOR);
	}
	ImGui::SameLine();
	if (ImGui::Button("Erase Tool", ImVec2(dim, dim)))
	{
		// This tool erases voxels
		m_ToolHandler.SetActiveTool(VoxelCore::ToolType::ERASE);
	}
	if (ImGui::Button("Build Tool", ImVec2(dim, dim)))
	{
		// This tool adds new voxels
		m_ToolHandler.SetActiveTool(VoxelCore::ToolType::BUILD);
	}

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	//m_ViewportFocused = ImGui::IsWindowFocused();
	//m_ViewportHovered = ImGui::IsWindowHovered();
	//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	// Mouse position relative to the drawing of our framebuffer
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	m_CursorPosImGui = { mousePos.x - windowPos.x - cursorPos.x, mousePos.y - windowPos.y - cursorPos.y };

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
		m_Octree.Deactivate(1, 0, 0);
		m_Octree.Deactivate(2, 0, 0);
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		// Test bind
		for (int i = 0; i < 8; i++)
		{
			m_Octree.Deactivate(i, i, i);
		}
		m_Octree.Deactivate(0, 0, 7);
		m_Octree.Deactivate(0, 7, 0);
		m_Octree.Deactivate(0, 7, 7);
		m_Octree.Deactivate(7, 0, 0);
		m_Octree.Deactivate(7, 0, 7);
		m_Octree.Deactivate(7, 7, 0);
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

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			// I could probably just save the mouse ray from the render loop for this
			m_ToolHandler.ToolLeftClick(m_Octree, GenerateMouseRay(), m_CurrentSelectedColorIndex);

		}
	//}
}

void EditorApplication::OnResize(int width, int height)
{
	//glViewport(0, 0, width, height);
	//m_FBO->Resize(width, height);
}

VoxelCore::Ray EditorApplication::GenerateMouseRay()
{
	// mouse x and y need to be relative to the window the framebuffer is being rendered to
	float x = m_CursorPosImGui.x;
	float y = m_CursorPosImGui.y;
	float aspectRatio = (m_ViewportSize.x / m_ViewportSize.y);

	// For some reason multiplying by the orthographic MVP matrix like in the shader dosen't work here (it produces a result which is slightly offset) so I instead manually convert the mouse 
	// coordinates to the correct orthographic range (which is based on the aspect ratio). I'm not sure why it dosen't work as it should be doing the same math as within the shader.
	// ndc is incorrect naming now but I'm going to leave it
	glm::vec3 ndc = glm::vec3((2.0f * aspectRatio * x) / m_ViewportSize.x - aspectRatio, 1.0f - (2.0f * y) / m_ViewportSize.y, 0.0f);

	// Just like in the vertex shader I multiply the ndc by the orthographic MVP matrix
	//glm::vec3 transformed = m_OctreeOrthoCamera.GetMVPMatrix() * glm::vec4(ndc, 1.0f);
	//VX_EDITOR_INFO("Transformed X: {0}, Y: {1}, Z: {2}", transformed.x, transformed.y, transformed.z);

	// Compute ray as in fragment shader
	glm::vec3 cameraDir = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 rayDir = cameraDir + ndc;
	glm::vec3 rayOrigin = glm::vec3(0.0f, 0.0f, m_OctreeCameraController.GetCameraRadius());

	rayOrigin = glm::vec3(glm::vec4(rayOrigin, 1.0f) * m_OctreeCameraController.GetViewMatrix());
	rayDir = glm::vec3(glm::vec4(rayDir, 1.0f) * m_OctreeCameraController.GetViewMatrix());

	return VoxelCore::Ray(rayOrigin, rayDir);
}

void EditorApplication::SaveToFile(std::string& filePath)
{
	// VIO File Format:
	// 8 Bytes: Size of Octree Data in Bytes
	// x Bytes: Octree Data
	// 8 Bytes: Size of Color Palette Data in Bytes
	// x Bytes: Color Palette Data

	uint64_t fileptr = 0;
	// Open File
	filePath.append(".vio");
	VX_CORE_INFO("[SAVE] File Path: {}", filePath);
	std::ofstream file;
	file.open(filePath, std::ios_base::binary);

	// 8 Bytes: Size of Octree Data in Bytes
	size_t fileSize = sizeof(uint32_t) * 4 * m_Octree.GetNodeCount();
	file.write(reinterpret_cast<char*>(&fileSize), 8);
	fileptr += 8;
	file.seekp(fileptr);

	// x Bytes: Octree Data
	file.write(reinterpret_cast<char*>(m_Octree.GetData()), fileSize);
	fileptr += fileSize;
	file.seekp(fileptr);

	// 8 Bytes: Size of Color Palette Data in Bytes
	size_t paletteSize = sizeof(float) * m_Palette.GetColorData().size();
	file.write(reinterpret_cast<char*>(&paletteSize), 8);
	fileptr += 8;
	file.seekp(fileptr);

	// x Bytes: Color Palette Data
	file.write(reinterpret_cast<char*>(m_Palette.GetColorData().data()), paletteSize);
	fileptr += paletteSize;
	file.seekp(fileptr);

	file.close();


	//filePath.append(".vio");
	//VX_CORE_INFO("[SAVE] File Path: {}", filePath);
	//std::ofstream file;
	//file.open(filePath, std::ios_base::binary);
	//size_t fileSize = sizeof(uint32_t) * 4 * m_Octree.GetNodeCount();
	//file.write(reinterpret_cast<char*>(m_Octree.GetData()), fileSize);
	//file.close();
}

void EditorApplication::LoadFromFile(std::string& filePath)
{
	uint64_t fileptr = 0;
	// Open File 
	std::ifstream file;
	file.open(filePath, std::ios_base::binary);

	// 8 Bytes: Size of Octree Data in Bytes
	size_t fileSize = 0;
	file.read(reinterpret_cast<char*>(&fileSize), 8);
	fileptr += 8;
	file.seekg(fileptr);

	// x Bytes: Octree Data
	m_Octree.ReplaceOctree(fileSize / (sizeof(uint32_t) * 4));
	file.read(reinterpret_cast<char*>(m_Octree.GetData()), fileSize);
	fileptr += fileSize;
	file.seekg(fileptr);

	// 8 Bytes: Size of Color Palette Data in Bytes
	size_t paletteSize = 0;
	file.read(reinterpret_cast<char*>(&paletteSize), 8);
	fileptr += 8;
	file.seekg(fileptr);

	// x Bytes: Color Palette Data
	m_Palette.ReplacePalette(paletteSize / sizeof(float));
	file.read(reinterpret_cast<char*>(m_Palette.GetColorData().data()), paletteSize);
	fileptr += paletteSize;
	file.seekg(fileptr);

	file.close();

	//VX_CORE_INFO("[LOAD] File Path: {}", filePath);
	//std::ifstream file;
	//file.open(filePath, std::ios_base::binary);
	//
	//file.seekg(0, std::ios::end);
	//size_t fileSize = file.tellg();
	//file.seekg(0, std::ios::beg);
	//
	//m_Octree.ReplaceOctree(fileSize / (sizeof(uint32_t) * 4));
	//file.read(reinterpret_cast<char*>(m_Octree.GetData()), fileSize);
	//file.close();
}