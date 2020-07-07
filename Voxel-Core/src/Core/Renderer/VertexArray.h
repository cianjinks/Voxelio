#pragma once
#include "Buffer.h"

namespace VoxelCore {

	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo) = 0;

		virtual std::shared_ptr<VertexBuffer>& GetVertexBuffer() = 0;
		virtual std::shared_ptr<IndexBuffer>& GetIndexBuffer() = 0;

		static std::shared_ptr<VertexArray> Create();
	};

}