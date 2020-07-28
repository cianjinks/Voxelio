#pragma once
#include "Core/PerspectiveCameraController.h"
#include "Core/OrbitalCameraController.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Shader.h"

namespace VoxelCore {

	enum class GraphicsAPI
	{
		None = 0,
		OpenGL
	};

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec3 Normal;
	};

	struct RendererData
	{
		static const int MaxFaces = 128 * 128 * 6;
		static const int MaxVertices = MaxFaces * 4;
		static const int MaxIndices = MaxFaces * 6;

		static int IndicesCount;
		static int DrawCalls;
	};

	class Renderer
	{
	private:
		static GraphicsAPI s_API;

		static std::shared_ptr<VertexArray> vao;
		static std::shared_ptr<VertexBuffer> vbo;
		static std::shared_ptr<IndexBuffer> ibo;
		static std::shared_ptr<Shader> shader;

		static bool m_ActiveScene;
		static std::vector<float>* m_VertexData;
	public:
		static GraphicsAPI getAPI() { return s_API; };
		static int s_CubeVertexElementCount;
	private:

	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(PerspectiveCameraController& camera);
		static void BeginScene(OrbitalCameraController& camera);
		static void EndScene();
		static void FlushData();

		static void DrawQuad(const glm::vec3& pos);
	};

}