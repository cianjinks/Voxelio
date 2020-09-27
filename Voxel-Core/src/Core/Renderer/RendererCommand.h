#pragma once
#include "Renderer.h"
#include <glm/glm.hpp>

namespace VoxelCore
{
	class RendererCommand
	{
	public:
		static void DrawElements(int indicesCount, int offset);
		static void ActiveTexture(int index);

		static void ClearColor(glm::vec4& color);
		static void Clear();

		static void EnableDepthTesting();
		static void EnableBlending();
	};
}