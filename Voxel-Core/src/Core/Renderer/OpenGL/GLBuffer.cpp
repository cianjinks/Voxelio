#include "voxelpch.h"
#include "GLBuffer.h"

namespace VoxelCore {

	// OpenGL Vertex Buffer

	GLVertexBuffer::GLVertexBuffer(int size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	GLVertexBuffer::GLVertexBuffer(void* data, int size)
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void GLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void GLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void GLVertexBuffer::SetData(const void* data, int size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void GLVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_BufferLayout = layout;
	}

	const BufferLayout& GLVertexBuffer::GetLayout() const
	{
		return m_BufferLayout;
	}

	// OpenGL Index Buffer

	GLIndexBuffer::GLIndexBuffer(int size)
		: m_Size(size)
	{
		// A VertexArray must exist for this to work
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	GLIndexBuffer::GLIndexBuffer(void* data, int size)
		: m_Size(size)
	{
		// A VertexArray must exist for this to work
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	GLIndexBuffer::~GLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void GLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void GLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void GLIndexBuffer::SetData(const void* data, int size)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
	}

	// OpenGL Texture Buffer (Data Buffer - Used to upload large amounts of data to shader)
	GLDataBuffer::GLDataBuffer(int size, DataBufferFormat format)
		: m_Size(size), m_Format(format)
	{
		glGenBuffers(1, &m_DataBufferID);
		glBindBuffer(GL_TEXTURE_BUFFER, m_DataBufferID);
		glBufferData(GL_TEXTURE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		glGenTextures(1, &m_BufferID);
		glBindTexture(GL_TEXTURE_BUFFER, m_BufferID);
		glTexBuffer(GL_TEXTURE_BUFFER, GetGLBufferDataFormat(format), m_DataBufferID);
	}

	GLDataBuffer::GLDataBuffer(void* data, int size, DataBufferFormat format)
		: m_Size(size), m_Format(format)
	{
		glGenBuffers(1, &m_DataBufferID);
		glBindBuffer(GL_TEXTURE_BUFFER, m_DataBufferID);
		glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);

		glGenTextures(1, &m_BufferID);
		glBindTexture(GL_TEXTURE_BUFFER, m_BufferID);
		glTexBuffer(GL_TEXTURE_BUFFER, GetGLBufferDataFormat(format), m_DataBufferID);
	}

	GLDataBuffer::~GLDataBuffer()
	{
		glDeleteBuffers(1, &m_DataBufferID);
		glDeleteTextures(1, &m_BufferID);
	}

	void GLDataBuffer::Bind() const
	{
		glBindBuffer(GL_TEXTURE_BUFFER, m_DataBufferID);
		glBindTexture(GL_TEXTURE_BUFFER, m_BufferID);
		glTexBuffer(GL_TEXTURE_BUFFER, GetGLBufferDataFormat(m_Format), m_DataBufferID);
	}
	void GLDataBuffer::Unbind() const
	{
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
		// Test this:
		glTexBuffer(GL_TEXTURE_BUFFER, GetGLBufferDataFormat(m_Format), 0);
	}

	void GLDataBuffer::SetData(const void* data, int size)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, m_DataBufferID);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size, data);
		glBindTexture(GL_TEXTURE_BUFFER, m_BufferID);
		glTexBuffer(GL_TEXTURE_BUFFER, GetGLBufferDataFormat(m_Format), m_DataBufferID);
	}

	GLFrameBuffer::GLFrameBuffer(const FrameBufferData& data)
		: m_FrameBufferData(data)
	{
		Refresh();
	}

	GLFrameBuffer::~GLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_BufferID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void GLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
		glViewport(0, 0, m_FrameBufferData.Width, m_FrameBufferData.Height);
	}

	void GLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFrameBuffer::Refresh()
	{
		if (m_BufferID)
		{
			glDeleteFramebuffers(1, &m_BufferID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		glCreateFramebuffers(1, &m_BufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_FrameBufferData.Width, m_FrameBufferData.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_FrameBufferData.Width, m_FrameBufferData.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			VX_CORE_CRITICAL("Framebuffer is incomplete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_FrameBufferData.Width = width;
		m_FrameBufferData.Height = height;

		Refresh();
	}
}