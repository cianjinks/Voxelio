#include "voxelpch.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace VoxelCore {

	GraphicsAPI Renderer::s_API = GraphicsAPI::OpenGL;
	int Renderer::s_CubeVertexElementCount = 9;

	std::shared_ptr<VertexArray> Renderer::vao = nullptr;
	std::shared_ptr<VertexBuffer> Renderer::vbo = nullptr;
	std::shared_ptr<IndexBuffer> Renderer::ibo = nullptr;
	std::shared_ptr<DataBuffer> Renderer::octdbo = nullptr;
	std::shared_ptr<DataBuffer> Renderer::colordbo = nullptr;
	std::shared_ptr<Shader> Renderer::shader = nullptr;

	std::vector<float>* Renderer::m_VertexData = nullptr;
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
		shader = Shader::CreateBasicShader("Test Shader", "assets/shaders/testvert.glsl", "assets/shaders/copyfrag.glsl");
		shader->Bind();
		
		m_VertexData = new std::vector<float>(RendererData::MaxVertices);

		glActiveTexture(GL_TEXTURE0);
		octdbo = DataBuffer::Create(RendererData::MaxNodeCount * sizeof(uint64_t), DataBufferFormat::UINT2);
		octdbo->Bind();
		glActiveTexture(GL_TEXTURE1);
		colordbo = DataBuffer::Create(RendererData::MaxColorPaletteSize * 4 * sizeof(float), DataBufferFormat::FLOAT4);
		colordbo->Bind();

		VX_CORE_INFO("Initialised Renderer");
	}

	void Renderer::Shutdown()
	{
		//delete[] m_VertexData;
		delete m_VertexData;
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

	void Renderer::DrawOctree(CompactVoxelOctree& octree, VoxelColorPalette& palette)
	{
		glActiveTexture(GL_TEXTURE0);
		octdbo->SetData(octree.GetData(), octree.GetNodeCount() * sizeof(uint64_t));
		octdbo->Bind();

		glActiveTexture(GL_TEXTURE1);
		auto& colorData = palette.GetColorData();
		colordbo->SetData(colorData.data(), colorData.size() * sizeof(float));
		colordbo->Bind();
	}
}