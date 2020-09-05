#include "voxelpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "OpenGL/GLBuffer.h"

namespace VoxelCore {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(int size)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Vertex Buffer"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLVertexBuffer>(size);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Vertex Buffer");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, int size)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Vertex Buffer"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLVertexBuffer>(vertices, size);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Vertex Buffer");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(int size)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Index Buffer"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLIndexBuffer>(size);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Index Buffer");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* indices, int size)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Index Buffer"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLIndexBuffer>(indices, size);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Index Buffer");
		return nullptr;
	}

	std::shared_ptr<DataBuffer> DataBuffer::Create(int size, DataBufferFormat format)
	{
		switch (Renderer::getAPI())
		{
			case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Data Buffer"); return nullptr;
			case GraphicsAPI::OpenGL: return std::make_shared<GLDataBuffer>(size, format);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Data Buffer");
		return nullptr;
	}

	std::shared_ptr<DataBuffer> DataBuffer::Create(void* data, int size, DataBufferFormat format)
	{
		switch (Renderer::getAPI())
		{
		case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Data Buffer"); return nullptr;
		case GraphicsAPI::OpenGL: return std::make_shared<GLDataBuffer>(data, size, format);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Data Buffer");
		return nullptr;
	}

	std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferData& data)
	{
		switch (Renderer::getAPI())
		{
		case GraphicsAPI::None: VX_CORE_CRITICAL("Unsupported graphics API specified for Frame Buffer"); return nullptr;
		case GraphicsAPI::OpenGL: return std::make_shared<GLFrameBuffer>(data);
		}

		VX_CORE_ERROR("Unknown graphics API specified for Frame Buffer");
		return nullptr;
	}
}