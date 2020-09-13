#pragma once
#include "VoxelCore.h"

class EditorApplication : public VoxelCore::Application
{
public:
	float m_WindowWidth;
	float m_WindowHeight;
	char* m_WindowName;

	int division = 0;

private:
	// Camera
	VoxelCore::OrbitalCameraController m_CameraController;

	// Octree
	VoxelCore::CompactVoxelOctree m_Octree;
	static int s_OctreeLevels;

	VoxelCore::FrameBufferData m_FBOData;
	std::shared_ptr<VoxelCore::FrameBuffer> m_FBO;
	glm::vec2 m_ViewportSize = { 0, 0 };

	// Palette Editor UI
	VoxelCore::VoxelColorPalette m_Palette;
	VoxelCore::VoxelColor m_DefaultColor = VoxelCore::VoxelColor("Default Color", 0.75f, 0.75f, 0.75f, 1.0f);
	VoxelCore::VoxelColor m_PaletteEditorColor = VoxelCore::VoxelColor("Test Color", 0.0f, 0.0f, 0.0f, 1.0f);
	VoxelCore::VoxelColor m_CurrentSelectedColor = m_DefaultColor;
	int m_CurrentSelectedColorIndex = 1;

	// Tools UI
	glm::ivec3 m_SelectedVoxel = glm::ivec3(0);

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
	char* GetWindowName() override { return m_WindowName; };

	void MouseSelection();

};

VoxelCore::Application* VoxelCore::CreateApplication()
{
	return new EditorApplication();
}