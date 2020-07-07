#include "voxelpch.h"
#include "Shader.h"
#include "Renderer.h"
#include "OpenGL/GLShader.h"

namespace VoxelCore {

	std::shared_ptr<Shader> Shader::CreateBasicShader(const std::string& name, const std::string& vertexfilepath, const std::string& fragmentfilepath)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Shader"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLShader>(name, vertexfilepath, fragmentfilepath);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Shader");
		return nullptr;
	}


}