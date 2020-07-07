#include "EditorApplication.h"

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window") {}

void EditorApplication::PreRender()
{
	float vertices[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	vao = VoxelCore::VertexArray::Create();
	vao->Bind();
	vbo = VoxelCore::VertexBuffer::Create(vertices, 8 * sizeof(float));
	vbo->SetLayout({VoxelCore::BufferElement("Position", VoxelCore::BufferDataType::Float2, false)});
	vbo->Bind();
	ibo = VoxelCore::IndexBuffer::Create(indices, 6 * sizeof(unsigned int));
	ibo->Bind();
	vao->SetVertexBuffer(vbo);
	vao->SetIndexBuffer(ibo);
	shader = VoxelCore::Shader::CreateBasicShader("Test Shader", "assets/shaders/testvert.glsl", "assets/shaders/testfrag.glsl");
	shader->Bind();
}

void EditorApplication::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 0.0f, 1.0f);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void EditorApplication::ImGuiRender()
{
	ImGui::SetNextWindowPos(ImVec2(25, 25));
	ImGui::Begin("Info: ");
	ImGui::Text(("Vendor: " + std::string((char*)glGetString(GL_VENDOR))).c_str());
	ImGui::Text(("Renderer: " + std::string((char*)glGetString(GL_RENDERER))).c_str());
	ImGui::Text(("Version: " + std::string((char*)glGetString(GL_VERSION))).c_str());
	ImGui::End();
}

void EditorApplication::OnKeyPress(int key, int scancode, int action, int mods)
{

}

void EditorApplication::OnMouseMove(float xpos, float ypos)
{

}

void EditorApplication::OnMouseScroll(float xoffset, float yoffset)
{

}

void EditorApplication::OnMouseClick(int button, int action, int mods)
{

}