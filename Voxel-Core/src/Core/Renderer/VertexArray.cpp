#include "voxelpch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "OpenGL/GLVertexArray.h"

namespace VoxelCore {

	std::shared_ptr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Vertex Array"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLVertexArray>();
		}

		VX_CORE_ERROR("Unknown graphics API specified for Vertex Array");
		return nullptr;
	}
}