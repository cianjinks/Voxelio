#include "voxelpch.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace VoxelCore {

	GraphicsAPI Renderer::s_API = GraphicsAPI::OpenGL;
	int Renderer::s_CubeVertexElementCount = 9;

	std::shared_ptr<VertexArray> Renderer::vao = nullptr;
	std::shared_ptr<VertexBuffer> Renderer::vbo = nullptr;
	std::shared_ptr<IndexBuffer> Renderer::ibo = nullptr;
	std::shared_ptr<DataBuffer> Renderer::dbo = nullptr;
	std::shared_ptr<Shader> Renderer::shader = nullptr;

	std::vector<float>* Renderer::m_VertexData = nullptr;
	CompactVoxelOctree* Renderer::m_Octree = nullptr;
	bool Renderer::m_ActiveScene = false;
	int RendererData::IndicesCount = 0;
	int RendererData::DrawCalls = 0;

	void Renderer::Init()
	{
		vao = VertexArray::Create();
		vao->Bind();
		vbo = VertexBuffer::Create(RendererData::MaxFloats * sizeof(float));
		vbo->SetLayout({ 
			BufferElement("a_Pos", BufferDataType::Float3, false),
			BufferElement("a_Color", BufferDataType::Float3, false),
			BufferElement("a_Normal", BufferDataType::Float3, false)
			});
		vbo->Bind();

		// Generate Indices
		unsigned int* indices = new unsigned int[RendererData::MaxIndices];
		unsigned int j = 0;
		for (unsigned int i = 0; i < RendererData::MaxIndices; i += 6)
		{
			indices[i + 0] = j + 0;
			indices[i + 1] = j + 1;
			indices[i + 2] = j + 2;
							 
			indices[i + 3] = j + 2;
			indices[i + 4] = j + 3;
			indices[i + 5] = j + 0;
			
			j += 4;
		}
		ibo = IndexBuffer::Create(indices, RendererData::MaxIndices * sizeof(unsigned int));
		delete[] indices;
		ibo->Bind();
		vao->SetVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);
		shader = Shader::CreateBasicShader("Test Shader", "assets/shaders/testvert.glsl", "assets/shaders/testfrag.glsl");
		shader->Bind();
		
		m_VertexData = new std::vector<float>(RendererData::MaxVertices);

		// Test Octree:
		m_Octree = new CompactVoxelOctree(3);
		dbo = DataBuffer::Create(m_Octree->GetData(), m_Octree->GetNodeCount() * sizeof(uint32_t), DataBufferFormat::UINT1);
		//uint32_t array[230] = { 66046, 196352, 719365, 1111306, 1504267, 1798287, 2010960, 2383776, 2760672, 3081212, 3244159, 3317855, 3448927, 3604224, 4127237, 4521728, 4997311, 5075087, 5308160, 5821483, 6094592, 6583690, 6924885, 7205389, 7533324, 7879365, 8190469, 8581900, 8917486, 9045243, 9153109, 9436928, 9961216, 10485504, 10945277, 11012853, 11206400, 11685298, 11992832, 12474280, 12782835, 12914672, 13238016, 13729664, 14169036, 14418430, 14484221, 14565312, 14946798, 33023, 57599, 12543, 52479, 41727, 51455, 52479, 8447, 65535, 52479, 52479, 65535, 65535, 57599, 61695, 35071, 65535, 43775, 65535, 61695, 65535, 62207, 65535, 65535, 65535, 65535, 65535, 49407, 61695, 20735, 12543, 52479, 50431, 65535, 65535, 52479, 52479, 65535, 65535, 20735, 50431, 65535, 54783, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 62975, 28927, 65535, 12543, 65535, 33023, 61695, 56831, 14335, 12543, 60159, 65535, 47871, 65535, 61695, 61695, 65535, 65535, 29695, 12543, 65535, 511, 8447, 47103, 49407, 63743, 65535, 19711, 8959, 1535, 61695, 61695, 65535, 65535, 13311, 12799, 4351, 30719, 5631, 35071, 35071, 35071, 767, 52479, 52479, 65535, 65535, 52479, 52479, 8191, 49151, 35583, 65535, 44031, 65535, 2303, 36863, 2815, 45055, 65535, 65535, 65535, 65535, 53247, 61439, 65535, 65535, 3327, 2815, 767, 52479, 52479, 65535, 65535, 52479, 52479, 65535, 65535, 52479, 65535, 56831, 19967, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 13311, 65535, 24575, 24575, 3327, 1279, 4095, 3583, 4095, 511, 61183, 65535, 65535, 65535, 61183, 57343, 49151, 32767, 65535, 65535, 22527, 1023, 4095, 4095, 511, 13311, 13311, 767, 1023, 4607, 1279, 65535, 65535, 13311, 13311, 16383, 8191, 30719, 511 };
		//dbo = DataBuffer::Create(array, 230 * sizeof(uint32_t), DataBufferFormat::UINT1);
		dbo->Bind();

		VX_CORE_INFO("Initialised Renderer");
	}

	void Renderer::Shutdown()
	{
		//delete[] m_VertexData;
		delete m_VertexData;
		delete m_Octree;
	}

	void Renderer::BeginScene(OrbitalCameraController& camera)
	{
		if (!m_ActiveScene)
		{
			RendererData::IndicesCount = 0;
			shader->Bind();
			shader->SetMat4("u_MVP", camera.GetMVPMatrix());
			shader->SetFloat1("u_Time", (float)glfwGetTime());
			m_ActiveScene = true;
			RendererData::DrawCalls = 0;
		}
		else
		{
			VX_CORE_ERROR("Failed to begin scene. Active scene found. ( Maybe you forgot to call Renderer::EndScene() )");
		}
	}

	void Renderer::BeginScene(PerspectiveCameraController& camera) {};

	void Renderer::EndScene()
	{
		if (m_ActiveScene)
		{
			vbo->SetData(m_VertexData->data(), m_VertexData->size() * sizeof(float));
			// HARDCODED API USAGE
			if (RendererData::IndicesCount != 0)
			{
				glDrawElements(GL_TRIANGLES, RendererData::IndicesCount, GL_UNSIGNED_INT, 0);
				RendererData::DrawCalls++;
			}
			m_ActiveScene = false;
		}
		else
		{
			VX_CORE_ERROR("Failed to end scene. No active scene found. ( Make sure to call Renderer::BeginScene() first )");
		}
	}

	void Renderer::FlushData()
	{
		EndScene();
		RendererData::IndicesCount = 0;
		m_ActiveScene = true;
	}

	void Renderer::DrawQuad(const glm::vec3& pos)
	{
		if (RendererData::IndicesCount + 6 > RendererData::MaxIndices)
		{
			FlushData();
		}
		RendererData::IndicesCount += 6;
		m_VertexData->insert(m_VertexData->begin(), 
		{
			-0.5f + pos.x , -0.5f + pos.y,  0.5f + pos.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f + pos.x , -0.5f + pos.y,  0.5f + pos.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f + pos.x ,  0.5f + pos.y,  0.5f + pos.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.5f + pos.x ,  0.5f + pos.y,  0.5f + pos.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		});
	}
}