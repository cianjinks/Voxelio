#pragma once
#include "VoxelCore.h"

class EditorApplication : public VoxelCore::Application
{
public:
	float m_WindowWidth;
	float m_WindowHeight;
	std::string m_WindowName;

	int division = 0;

private:
	// Camera
	VoxelCore::OrbitalCameraController m_OctreeCameraController;
	VoxelCore::OrthographicCamera m_OctreeOrthoCamera;
	bool m_MouseInViewport = false;

	// Octree
	VoxelCore::CompactVoxelOctree m_Octree;

	VoxelCore::FrameBufferData m_FBOData;
	std::shared_ptr<VoxelCore::FrameBuffer> m_FBO;
	glm::vec2 m_ViewportSize = { 0, 0 };

	// Palette Editor UI
	VoxelCore::VoxelColorPalette m_Palette;
	glm::vec4 m_PaletteEditorColor = m_Palette.m_DefaultColor;
	glm::vec4 m_CurrentSelectedColor = m_Palette.m_DefaultColor;
	int m_CurrentSelectedColorIndex = 0;
	bool m_PaletteFull = false;

	// Tools UI
	VoxelCore::ToolHandler m_ToolHandler;
	glm::ivec3 m_SelectedVoxel = glm::ivec3(0);
	glm::vec2 m_CursorPosImGui = glm::vec2(0.0f);

	// Viewport Info
	ImVec2 m_ViewportWindowDim = ImVec2(0, 0);

	// Model Saving / Loading
	ImGui::FileBrowser m_FileBrowser;
	static std::string s_ProjectFileExtension;
	static bool s_NewModel;
	static bool s_LoadModel;
	static bool s_CloseLoadModel;
	static bool s_SaveModel;
	static bool s_CloseSaveModel;

	std::string m_CurrentFile = "untitled.vio";
	bool m_IsSaved = true;

	void SaveToFile(std::string& filePath);
	void LoadFromFile(std::string& filePath);

	// Palette Loading / Saving
	ImGui::FileBrowser m_PaletteFileBrowser;
	static std::string s_PaletteFileExtension;
	static bool s_PaletteLoad;
	static bool s_PaletteSave;

	void SavePaletteToFile(std::string& filePath);
	void LoadPaletteFromFile(std::string& filePath);

	void GenerateToolButtons();

public:
	EditorApplication();
	virtual ~EditorApplication() = default;

	void PreRender() override;
	void Render() override;
	void ImGuiRender() override;

	void OnKeyPress(int key, int scancode, int action, int mods) override;
	void OnMouseMove(float xpos, float ypos) override;
	void OnMouseScroll(float xoffset, float yoffset) override;
	void OnMouseClick(int button, int action, int mods) override;
	void OnResize(int width, int height) override;

	float GetWindowWidth() override { return m_WindowWidth; };
	float GetWindowHeight() override { return m_WindowHeight; };
	std::string GetWindowName() override { return m_WindowName; };
	bool GetSaveState() override { return m_IsSaved; };

	VoxelCore::Ray GenerateMouseRay();

};

VoxelCore::Application* VoxelCore::CreateApplication()
{
	return new EditorApplication();
}