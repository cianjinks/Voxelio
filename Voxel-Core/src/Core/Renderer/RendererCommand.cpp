#include "voxelpch.h"
#include "RendererCommand.h"

#include <glad/glad.h>

namespace VoxelCore
{

	void RendererCommand::DrawElements(int indicesCount, int offset)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Draw Elements call"); return;
			case GraphicsAPI::OpenGL: glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)offset); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for Draw Elements call");
	}

	void RendererCommand::ActiveTexture(int index)
	{
		switch (Renderer::getAPI())
		{
		case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Active Texture call"); return;
		case GraphicsAPI::OpenGL: glActiveTexture(GL_TEXTURE0 + index); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for Active Texture call");
	}

}
