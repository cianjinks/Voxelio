#include "voxelpch.h"
#include "RendererCommand.h"

#include <glad/glad.h>

namespace VoxelCore
{

	void RendererCommand::DrawElements(int indicesCount, const void* offset)
	{
		switch (Renderer::getAPI())
		{
		case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Draw Elements call"); return;
		case GraphicsAPI::OpenGL: glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, offset); return;
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

	void RendererCommand::ClearColor(glm::vec4& color)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Clear Color call"); return;
			case GraphicsAPI::OpenGL: glClearColor(color.r, color.g, color.b, color.a); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for Clear Color call");
	}

	void RendererCommand::Clear()
	{
		// Probably could have a check for if depth testing is enabled in which case no need to clear that bit
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Clear call"); return;
			case GraphicsAPI::OpenGL: glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for Clear call");
	}

	void RendererCommand::EnableDepthTesting()
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for enabling blending call"); return;
			case GraphicsAPI::OpenGL: glEnable(GL_BLEND); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for enabling blending call");
	}

	void RendererCommand::EnableBlending()
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for enabling depth testing call"); return;
			case GraphicsAPI::OpenGL: glEnable(GL_DEPTH_TEST); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for enabling depth testing call");
	}

	void RendererCommand::EnableVsync()
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for enabling vsync call"); return;
			case GraphicsAPI::OpenGL: glfwSwapInterval(1); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for enabling vsync call");
	}

	void RendererCommand::DisableVsync()
	{
		switch (Renderer::getAPI())
		{
		case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for disabling vsync call"); return;
		case GraphicsAPI::OpenGL: glfwSwapInterval(0); return;
		}

		VX_CORE_ERROR("Unknown graphics API specified for disabling vsync call");
	}



}
