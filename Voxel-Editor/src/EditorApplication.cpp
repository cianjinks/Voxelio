#include "EditorApplication.h"

EditorApplication::EditorApplication()
	: m_WindowWidth(1280.0f), m_WindowHeight(720.0f), m_WindowName("Test Window"), m_CameraController(1280.0f, 720.0f) {}

void EditorApplication::PreRender()
{
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f
	};

	std::vector<unsigned int> indices;
	int j = 0;
	for (int i = 0; i < 36; i++)
	{
		indices.emplace_back(j);
		indices.emplace_back(++j);
		indices.emplace_back(++j);
		indices.emplace_back(j);
		indices.emplace_back(++j);
		indices.emplace_back(j - 3);
		++j;
	}

	vao = VoxelCore::VertexArray::Create();
	vao->Bind();
	vbo = VoxelCore::VertexBuffer::Create(vertices, 12 * 6 * sizeof(float));
	vbo->SetLayout({VoxelCore::BufferElement("Position", VoxelCore::BufferDataType::Float3, false)});
	vbo->Bind();
	ibo = VoxelCore::IndexBuffer::Create(&indices[0], 36 * sizeof(unsigned int));
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

	// Camera
	m_CameraController.HandleInput();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_CameraController.GetViewMatrix();
	glm::mat4 projection = m_CameraController.GetProjectionMatrix();
	glm::mat4 mvp = projection * view * model;
	shader->SetMat4("u_MVP", mvp);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
	m_CameraController.OnMouseMove(xpos, ypos);
}

void EditorApplication::OnMouseScroll(float xoffset, float yoffset)
{

}

void EditorApplication::OnMouseClick(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		ImGuiIO& io = ImGui::GetIO();
		if (VoxelCore::Input::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			if (!io.WantCaptureMouse) {
				VoxelCore::Input::SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
	}
}