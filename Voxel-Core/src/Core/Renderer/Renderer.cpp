#include "voxelpch.h"
#include "Renderer.h"

#include <glad/glad.h>
#include "RendererCommand.h"

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
			BufferElement("a_Pos", BufferDataType::Float3, false)
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
		shader = Shader::CreateBasicShader("Test Shader", "assets/shaders/testvert.glsl", "assets/shaders/finalfrag.glsl");
		shader->Bind();
		
		m_VertexData = new std::vector<float>(RendererData::MaxVertices);

		RendererCommand::ActiveTexture(0);
		octdbo = DataBuffer::Create(CompactVoxelOctree::MAX_OCTREE_NODES * 4 * sizeof(uint32_t), DataBufferFormat::UINT4);
		octdbo->Bind();
		RendererCommand::ActiveTexture(1);
		colordbo = DataBuffer::Create(RendererData::MaxColorPaletteSize * 4 * sizeof(float), DataBufferFormat::FLOAT4);
		colordbo->Bind();

		VX_CORE_INFO("Initialised Renderer");
	}

	void Renderer::Shutdown()
	{
		//delete[] m_VertexData;
		delete m_VertexData;
	}

	void Renderer::BeginScene(OrbitalCameraController& orbitalCamera, OrthographicCamera& orthoCamera)
	{
		if (!m_ActiveScene)
		{
			RendererData::IndicesCount = 0;
			shader->Bind();
			// Camera Uniforms
			shader->SetMat4("u_OrthoMVP", orthoCamera.GetMVPMatrix());
			shader->SetMat4("u_PerspectiveViewMatrix", orbitalCamera.GetViewMatrix());
			shader->SetFloat1("u_CameraRadius", orbitalCamera.GetCameraRadius());
			// Time Uniform
			shader->SetFloat1("u_Time", (float)glfwGetTime());
			// Set texture units of data buffers
			shader->SetInt1("u_VoxelData", 0); 
			shader->SetInt1("u_ColorData", 1);
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
			if (RendererData::IndicesCount != 0)
			{
				RendererCommand::DrawElements(RendererData::IndicesCount, 0);
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

	void Renderer::DrawQuad(const glm::vec3& pos, const float scale)
	{
		if (RendererData::IndicesCount + 6 > RendererData::MaxIndices)
		{
			FlushData();
		}
		RendererData::IndicesCount += 6;
		m_VertexData->insert(m_VertexData->begin(), 
		{
			(scale * -1.0f) + pos.x , (scale * -1.0f) + pos.y,  0.0f + pos.z,
			(scale *  1.0f) + pos.x , (scale * -1.0f) + pos.y,  0.0f + pos.z,
			(scale *  1.0f) + pos.x , (scale *  1.0f) + pos.y,  0.0f + pos.z,
			(scale * -1.0f) + pos.x , (scale *  1.0f) + pos.y,  0.0f + pos.z
		});
	}

	void Renderer::DrawRect(const glm::vec3& pos, const float width, const float height)
	{
		if (RendererData::IndicesCount + 6 > RendererData::MaxIndices)
		{
			FlushData();
		}
		RendererData::IndicesCount += 6;
		m_VertexData->insert(m_VertexData->begin(),
			{
				pos.x, pos.y, pos.z, // Bottom Left
				pos.x + width, pos.y, pos.z, // Bottom Right
				pos.x + width, pos.y + height, pos.z, // Top Right
				pos.x, pos.y + height, pos.z // Top Left
			});
	}

	void Renderer::DrawOctree(CompactVoxelOctree& octree, VoxelColorPalette& palette, float aspectRatio)
	{
		DrawRect(glm::vec3(-aspectRatio, -1.0f, 0.0f), 2 * aspectRatio, 2.0f);

		RendererCommand::ActiveTexture(0);
		octdbo->SetData(octree.GetData(), octree.GetNodeCount() * 4 * sizeof(uint32_t));
		octdbo->Bind();

		RendererCommand::ActiveTexture(1);
		auto& colorData = palette.GetColors();
		colordbo->SetData(colorData.data(), colorData.size() * 4 * sizeof(float));
		colordbo->Bind();
	}
}