#include "voxelpch.h"
#include "GLVertexArray.h"
#include "../Buffer.h"

namespace VoxelCore {

	GLVertexArray::GLVertexArray()
	{
		glGenVertexArrays(1, &m_VertexArrayID);
	}

	GLVertexArray::~GLVertexArray()
	{
		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void GLVertexArray::Bind()
	{
		glBindVertexArray(m_VertexArrayID);
	}

	void GLVertexArray::UnBind()
	{
		glBindVertexArray(0);
	}

	void GLVertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo)
	{
		glBindVertexArray(m_VertexArrayID);
		vbo->Bind();
		m_VBO = vbo;

		// Currently only supports a single index buffer and therefore the vertex attrib pointer index is reset every time
		int index = 0;
		const auto& layout = vbo->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetElementCount(), GetGLElementType(element.DataType), element.Normalised ? GL_TRUE : GL_FALSE, layout.GetStride(), (void*)(element.Offset));
			index++;
		}

	}

	void GLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo)
	{
		glBindVertexArray(m_VertexArrayID);
		ibo->Bind();
		m_IBO = ibo;
	}

	std::shared_ptr<VertexBuffer>& GLVertexArray::GetVertexBuffer()
	{
		return m_VBO;
	}

	std::shared_ptr<IndexBuffer>& GLVertexArray::GetIndexBuffer()
	{
		return m_IBO;
	}

	GLenum GLVertexArray::GetGLElementType(BufferDataType type)
	{
		switch (type)
		{
		case BufferDataType::Float: return GL_FLOAT;
		case BufferDataType::Float2: return GL_FLOAT;
		case BufferDataType::Float3: return GL_FLOAT;
		case BufferDataType::Float4: return GL_FLOAT;

		case BufferDataType::Mat3: return GL_FLOAT;
		case BufferDataType::Mat4: return GL_FLOAT;

		case BufferDataType::Integer: return GL_INT;
		case BufferDataType::Integer2: return GL_INT;
		case BufferDataType::Integer3: return GL_INT;
		case BufferDataType::Integer4: return GL_INT;
		}

		VX_CORE_CRITICAL("Undefined Buffer DataType");
		return 0;
	}
}