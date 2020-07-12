#include "voxelpch.h"
#include "Texture.h"
#include "Renderer.h"
#include "OpenGL/GLTexture.h"

namespace VoxelCore {

	std::shared_ptr<Texture> CreateTexture(const std::string& name, const std::string& filepath)
	{
		switch (Renderer::getAPI())
		{
		case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Texture"); return nullptr;
		case GraphicsAPI::OpenGL: return std::make_shared<GLTexture>(name, filepath);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Texture");
		return nullptr;
	}
}