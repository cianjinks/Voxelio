#pragma once
#include "Core/PerspectiveCameraController.h"
#include "Core/OrbitalCameraController.h"
#include "Core/OrthographicCamera.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/Octree/CompactVoxelOctree.h"
#include "Core/Renderer/RendererCommand.h"
#include "Util/Palette.h"

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
		static const int MaxCubes = 125000;
		static const int MaxFaces = MaxCubes * 6;
		static const int MaxVertices = MaxFaces * 4;
		static const int MaxIndices = MaxFaces * 6;
		static const int MaxFloats = MaxVertices * 9;

		static const int MaxColorPaletteSize = 4096;

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
		static std::shared_ptr<DataBuffer> octdbo;
		static std::shared_ptr<DataBuffer> colordbo;
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
		// Specifically for Octree:
		static void BeginScene(OrbitalCameraController& orbitalCamera, OrthographicCamera& orthoCamera);
		static void EndScene();
		static void FlushData();

		static void DrawQuad(const glm::vec3& pos, const float scale);
		static void DrawRect(const glm::vec3& pos, const float width, const float height);
		static void DrawOctree(CompactVoxelOctree& octree, VoxelColorPalette& palette, float aspectRatio);
	};

}