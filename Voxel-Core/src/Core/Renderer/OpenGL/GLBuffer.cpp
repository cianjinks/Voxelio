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
}