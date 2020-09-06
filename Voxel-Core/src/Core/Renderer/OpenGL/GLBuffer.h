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

	// Used for uploading large amounts of data to a shader
	class GLDataBuffer : public DataBuffer
	{
	private:
		GLuint m_DataBufferID;
		GLuint m_BufferID;;
		int m_Size = 0;
		DataBufferFormat m_Format;

	public:
		GLDataBuffer(int size, DataBufferFormat format);
		GLDataBuffer(void* data, int size, DataBufferFormat format);
		virtual ~GLDataBuffer();

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, int size) override;

		void SetFormat(DataBufferFormat format) override { m_Format = format; };
		DataBufferFormat GetFormat() override { return m_Format; };

		int GetSize() const override { return m_Size; }

		static GLenum GetGLBufferDataFormat(DataBufferFormat format)
		{
			switch (format)
			{
				case DataBufferFormat::UINT1: return GL_R32UI;
				case DataBufferFormat::UINT2: return GL_RG32UI;
				case DataBufferFormat::UINT3: return GL_RGB32UI;
				case DataBufferFormat::UINT4: return GL_RGBA32UI;
					
				case DataBufferFormat::FLOAT1: return GL_R32F;
				case DataBufferFormat::FLOAT2: return GL_RG32F;
				case DataBufferFormat::FLOAT3: return GL_RGB32F;
				case DataBufferFormat::FLOAT4: return GL_RGBA32F;
			}

			VX_CORE_CRITICAL("Undefined Data Buffer Format Type");
			return 0;
		};
	};

	class GLFrameBuffer : public FrameBuffer
	{
	private:
		GLuint m_BufferID = 0, m_ColorAttachment = 0, m_DepthAttachment = 0;
		FrameBufferData m_FrameBufferData;

	public:
		GLFrameBuffer(const FrameBufferData& data);
		virtual ~GLFrameBuffer();

		void Bind() override;
		void Unbind() override;

		void Refresh() override;
		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetFrameBufferColorData() const override { return m_ColorAttachment; };
		uint32_t GetFrameBufferDepthData() const override { return m_DepthAttachment; };
		const FrameBufferData& GetFrameBufferData() const override { return m_FrameBufferData; };

	};
}