#pragma once
#include "VoxelCore.h"

class EditorApplication : public VoxelCore::Application
{
public:
	float m_WindowWidth;
	float m_WindowHeight;
	char* m_WindowName;
private:
	std::shared_ptr<VoxelCore::VertexArray> vao;
	std::shared_ptr<VoxelCore::VertexBuffer> vbo;
	std::shared_ptr<VoxelCore::IndexBuffer> ibo;
	std::shared_ptr<VoxelCore::Shader> shader;

	VoxelCore::OrbitalCameraController m_CameraController;
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

	float GetWindowWidth() override { return m_WindowWidth; };
	float GetWindowHeight() override { return m_WindowHeight; };
	char* GetWindowName() override { return m_WindowName; };

};

VoxelCore::Application* VoxelCore::CreateApplication()
{
	return new EditorApplication();
}