#include "EditorApplication.h"

bool EditorApplication::s_NewModel = false;
bool EditorApplication::s_LoadModel = false;
bool EditorApplication::s_CloseLoadModel = false;
bool EditorApplication::s_SaveModel = false;
bool EditorApplication::s_CloseSaveModel = false;
bool EditorApplication::s_PaletteLoad = false;
bool EditorApplication::s_PaletteSave = false;
std::string EditorApplication::s_ProjectFileExtension = ".vio";
std::string EditorApplication::s_PaletteFileExtension = ".viop";

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window"), m_OctreeCameraController(1280.0f, 720.0f, 5.0f), m_OctreeOrthoCamera(1280.0f, 720.0f) {}

void EditorApplication::PreRender()
{
	VoxelCore::RendererCommand::EnableBlending();
	VoxelCore::RendererCommand::EnableDepthTesting();
	glfwSwapInterval(0);

	m_FBOData.Width = m_WindowWidth;
	m_FBOData.Height = m_WindowHeight;
	m_FBO = VoxelCore::FrameBuffer::Create(m_FBOData);

	m_FileBrowser.SetTypeFilters({s_ProjectFileExtension.c_str()});
	m_PaletteFileBrowser.SetTypeFilters({s_PaletteFileExtension.c_str()});
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

	// Clear FBO Screen
	VoxelCore::RendererCommand::ClearColor(glm::vec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f));
	VoxelCore::RendererCommand::Clear();

	VoxelCore::Renderer::BeginScene(m_OctreeCameraController, m_OctreeOrthoCamera);
	VoxelCore::Renderer::DrawOctree(m_Octree, m_Palette, m_OctreeOrthoCamera.GetAspectRatio());
	VoxelCore::Renderer::EndScene();
	m_FBO->Unbind();

	// Clear Screen behind FBO
	VoxelCore::RendererCommand::ClearColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
	VoxelCore::RendererCommand::Clear();

	// Tools Rendering (Shows preview of current tools action)
	if (VoxelCore::Input::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		m_ToolHandler.ToolHover(m_Octree, GenerateMouseRay(), m_CurrentSelectedColorIndex);
	}
}

void EditorApplication::ImGuiRender()
{
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

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
			ImGui::MenuItem("New Project", NULL, &s_NewModel);
			ImGui::MenuItem("Save Project", NULL, &s_SaveModel);
			ImGui::MenuItem("Load Project", NULL, &s_LoadModel);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// New Model
	if (s_NewModel)
	{
		m_Octree.Reload();
		m_Palette.Reload();
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
	ImGui::ColorPicker4("", &m_PaletteEditorColor.r, 0);

	if (ImGui::Button("Add Color"))
	{
		if (!m_Palette.AddColor(m_PaletteEditorColor))
		{
			m_PaletteFull = true;
		}
	}

	// Load and Save Palette Buttons
	if (ImGui::Button("Load Palette"))
	{
		m_PaletteFileBrowser.flags_ = 0;
		m_PaletteFileBrowser.Open();
		s_PaletteLoad = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Palette"))
	{
		m_PaletteFileBrowser.flags_ |= ImGuiFileBrowserFlags_EnterNewFilename;
		m_PaletteFileBrowser.Open();
		s_PaletteSave = true;
	}

	m_PaletteFileBrowser.Display();
	if (m_PaletteFileBrowser.HasSelected())
	{
		if (s_PaletteLoad)
		{
			LoadPaletteFromFile(m_PaletteFileBrowser.GetSelected().string());
			s_PaletteLoad = false;
		}
		if (s_PaletteSave)
		{
			SavePaletteToFile(m_PaletteFileBrowser.GetSelected().string());
			s_PaletteSave = false;
		}
		m_PaletteFileBrowser.ClearSelected();
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
			if (ImGui::ColorButton(std::to_string(pc).c_str(), ImVec4(colors[pc].r, colors[pc].g, colors[pc].b, colors[pc].a), 0, ImVec2(50, 50)))
			{
				m_CurrentSelectedColor = colors[pc];
				m_CurrentSelectedColorIndex = pc;
				VX_EDITOR_INFO("Changed Selected Color to Index: {}", pc);
			}
			count = 0;
		}
		else
		{
			if (ImGui::ColorButton(std::to_string(pc).c_str(), ImVec4(colors[pc].r, colors[pc].g, colors[pc].b, colors[pc].a), 0, ImVec2(50, 50)))
			{
				m_CurrentSelectedColor = colors[pc];
				m_CurrentSelectedColorIndex = pc;
				VX_EDITOR_INFO("Changed Selected Color to Index: {}", pc);
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
	ImGui::ColorButton("",
		ImVec4(m_CurrentSelectedColor.r, m_CurrentSelectedColor.g, m_CurrentSelectedColor.b, m_CurrentSelectedColor.a), 0, ImVec2(toolsWidth / 2, toolsWidth / 2));
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
	ImGui::Separator();

	GenerateToolButtons();

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	//m_ViewportFocused = ImGui::IsWindowFocused();
	//m_ViewportHovered = ImGui::IsWindowHovered();
	m_MouseInViewport = ImGui::IsWindowHovered();

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	// Mouse position relative to the drawing of our framebuffer
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	m_CursorPosImGui = { mousePos.x - windowPos.x - cursorPos.x, mousePos.y - windowPos.y - cursorPos.y };
	m_ViewportWindowDim = { ImGui::GetWindowWidth() - cursorPos.x, ImGui::GetWindowHeight() - cursorPos.y };

	uint64_t textureID = m_FBO->GetFrameBufferColorData();
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();

	//ImGui::ShowDemoWindow();
}

void EditorApplication::OnKeyPress(int key, int scancode, int action, int mods)
{
	if (m_MouseInViewport)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{

		}
	}
}

void EditorApplication::OnMouseMove(float xpos, float ypos)
{
	m_OctreeCameraController.OnMouseMove(xpos, ypos);
}

void EditorApplication::OnMouseScroll(float xoffset, float yoffset)
{
	if (m_MouseInViewport)
	{
		m_OctreeCameraController.OnMouseScroll(xoffset, yoffset);
	}
}

void EditorApplication::OnMouseClick(int button, int action, int mods)
{
	// Check if mouse is over viewport
	if (m_MouseInViewport) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			// I could probably just save the mouse ray from the render loop for this
			m_ToolHandler.ToolLeftClick(m_Octree, GenerateMouseRay(), m_CurrentSelectedColorIndex, m_SelectedVoxel);
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
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
	if (filePath.substr(filePath.length() - s_ProjectFileExtension.length(), filePath.length()) != s_ProjectFileExtension)
	{
		filePath.append(s_ProjectFileExtension);
	}
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
	size_t paletteSize = sizeof(float) * 4 * m_Palette.GetColors().size();
	file.write(reinterpret_cast<char*>(&paletteSize), 8);
	fileptr += 8;
	file.seekp(fileptr);

	// x Bytes: Color Palette Data
	file.write(reinterpret_cast<char*>(m_Palette.GetColors().data()), paletteSize);
	fileptr += paletteSize;
	file.seekp(fileptr);

	file.close();
}

void EditorApplication::LoadFromFile(std::string& filePath)
{
	uint64_t fileptr = 0;
	// Open File 
	VX_CORE_INFO("[SAVE] File Path: {}", filePath);
	std::ifstream file;
	file.open(filePath, std::ios_base::binary);

	// 8 Bytes: Size of Octree Data in Bytes
	size_t fileSize = 0;
	file.read(reinterpret_cast<char*>(&fileSize), 8);
	fileptr += 8;
	file.seekg(fileptr);

	// x Bytes: Octree Data
	m_Octree.Replace(fileSize / (sizeof(uint32_t) * 4));
	file.read(reinterpret_cast<char*>(m_Octree.GetData()), fileSize);
	fileptr += fileSize;
	file.seekg(fileptr);

	// 8 Bytes: Size of Color Palette Data in Bytes
	size_t paletteSize = 0;
	file.read(reinterpret_cast<char*>(&paletteSize), 8);
	fileptr += 8;
	file.seekg(fileptr);

	// x Bytes: Color Palette Data
	m_Palette.Replace(paletteSize / (sizeof(float) * 4));
	file.read(reinterpret_cast<char*>(m_Palette.GetColors().data()), paletteSize);
	fileptr += paletteSize;
	file.seekg(fileptr);

	file.close();
}

void EditorApplication::SavePaletteToFile(std::string& filePath)
{
	if (filePath.substr(filePath.length() - s_PaletteFileExtension.length(), filePath.length()) != s_PaletteFileExtension)
	{
		filePath.append(s_PaletteFileExtension);
	}
	VX_CORE_INFO("[PALETTE SAVE] File Path: {}", filePath);
	std::ofstream file;
	file.open(filePath, std::ios_base::binary);

	size_t paletteSize = sizeof(float) * 4 * m_Palette.GetColors().size();
	file.write(reinterpret_cast<char*>(&paletteSize), 8);
	file.seekp(8);
	file.write(reinterpret_cast<char*>(m_Palette.GetColors().data()), paletteSize);

	file.close();
}

void EditorApplication::LoadPaletteFromFile(std::string& filePath)
{
	VX_CORE_INFO("[PALETTE LOAD] File Path: {}", filePath);
	std::ifstream file;
	file.open(filePath, std::ios_base::binary);

	size_t paletteSize = 0;
	file.read(reinterpret_cast<char*>(&paletteSize), 8);
	file.seekg(8);
	m_Palette.Replace(paletteSize / (sizeof(float) * 4));
	file.read(reinterpret_cast<char*>(m_Palette.GetColors().data()), paletteSize);

	file.close();
}

void EditorApplication::GenerateToolButtons()
{
	float toolWindowWidth = ImGui::GetWindowWidth();
	float dim = (toolWindowWidth / 3) - (0.05 * toolWindowWidth);

	// Color Tool Button
	if (m_ToolHandler.GetActiveTool() == VoxelCore::ToolType::COLOR) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f));
		if (ImGui::Button("Color", ImVec2(dim, dim)))
		{
			// This colors voxels
			m_ToolHandler.SetActiveTool(VoxelCore::ToolType::COLOR);
		}
		ImGui::PopStyleColor(1);
	}
	else
	{
		if (ImGui::Button("Color", ImVec2(dim, dim)))
		{
			// This colors voxels
			m_ToolHandler.SetActiveTool(VoxelCore::ToolType::COLOR);
		}
	}
	ImGui::SameLine();

	// Erase Tool Button
	if (m_ToolHandler.GetActiveTool() == VoxelCore::ToolType::ERASE) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f));
		if (ImGui::Button("Erase", ImVec2(dim, dim)))
		{
			// This tool erases voxels
			m_ToolHandler.SetActiveTool(VoxelCore::ToolType::ERASE);
		}
		ImGui::PopStyleColor(1);
	}
	else
	{
		if (ImGui::Button("Erase", ImVec2(dim, dim)))
		{
			// This tool erases voxels
			m_ToolHandler.SetActiveTool(VoxelCore::ToolType::ERASE);
		}
	}
	ImGui::SameLine();

	// Build Tool Button
	if (m_ToolHandler.GetActiveTool() == VoxelCore::ToolType::BUILD) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(64.0f / 255.0f, 224.0f / 255.0f, 208.0f / 255.0f, 1.0f));
		if (ImGui::Button("Build", ImVec2(dim, dim)))
		{
			// This tool adds new voxels
			m_ToolHandler.SetActiveTool(VoxelCore::ToolType::BUILD);
		}
		ImGui::PopStyleColor(1);
	}
	else
	{
		if (ImGui::Button("Build", ImVec2(dim, dim)))
		{
			// This tool adds new voxels
			m_ToolHandler.SetActiveTool(VoxelCore::ToolType::BUILD);
		}
	}
}