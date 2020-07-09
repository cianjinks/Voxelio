#pragma once
#include "Core/PerspectiveCameraController.h"

namespace VoxelCore {

	enum class GraphicsAPI
	{
		None = 0,
		OpenGL
	};

	class Renderer
	{
	private:
		static GraphicsAPI s_API;
	public:
		static GraphicsAPI getAPI() { return s_API; };

	private:

	public:
		static void BeginScene();
		static void EndScene();
	};

}