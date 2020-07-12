#pragma once
#include "Core/Renderer/Buffer.h"
#include <glad/glad.h>

namespace VoxelCore {

	class GLVertexBuffer : public VertexBuffer
	{
	private:
		GLuint m_BufferID;
		BufferLayout m_BufferLayout;
	public:
		GLVertexBuffer(int size);
		GLVertexBuffer(void* data, int size);
		virtual ~GLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, int size) override;

		void SetLayout(const BufferLayout& layout) override;
		const BufferLayout& GetLayout() const override;
	};

	class GLIndexBuffer : public IndexBuffer
	{
	private:
		GLuint m_BufferID;
		int m_Size = 0;
	public:
		GLIndexBuffer(int size);
		GLIndexBuffer(void* data, int size);
		virtual ~GLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, int size) override;

		int GetSize() const override { return m_Size; }
	};
}