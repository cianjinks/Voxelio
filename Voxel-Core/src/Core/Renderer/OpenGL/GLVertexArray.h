#pragma once
#include "Core/Renderer/VertexArray.h"
#include <glad/glad.h>

namespace VoxelCore {

	class GLVertexArray : public VertexArray
	{
	private:
		GLuint m_VertexArrayID;
		std::shared_ptr<VertexBuffer> m_VBO;
		std::shared_ptr<IndexBuffer> m_IBO;
	public:
		GLVertexArray();
		virtual ~GLVertexArray();

		void Bind() override;
		void UnBind() override;

		void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo) override;

		std::shared_ptr<VertexBuffer>& GetVertexBuffer() override;
		std::shared_ptr<IndexBuffer>& GetIndexBuffer() override;

		static GLenum GetGLElementType(BufferDataType type);
	};
} 