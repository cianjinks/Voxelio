#pragma once
#include "Renderer.h"

namespace VoxelCore
{
	class RendererCommand
	{
	public:
		static void DrawElements(int indicesCount, int offset);
		static void ActiveTexture(int index);
	};
}