#pragma once

namespace VoxelCore {

	enum class BufferDataType
	{
		Float = 0, Float2, Float3, Float4, Mat3, Mat4, Integer, Integer2, Integer3, Integer4
	};

	static int GetDataTypeSize(BufferDataType type)
	{
		switch (type)
		{
			case BufferDataType::Float: return sizeof(float);
			case BufferDataType::Float2: return 2 * sizeof(float);
			case BufferDataType::Float3: return 3 * sizeof(float);
			case BufferDataType::Float4: return 4 * sizeof(float);

			case BufferDataType::Mat3: return 3 * 3 * sizeof(float);
			case BufferDataType::Mat4: return 4 * 4 * sizeof(float);

			case BufferDataType::Integer: return sizeof(int);
			case BufferDataType::Integer2: return 2 * sizeof(int);
			case BufferDataType::Integer3: return 3 * sizeof(int);
			case BufferDataType::Integer4: return 4 * sizeof(int);
		}

		VX_CORE_CRITICAL("Undefined Buffer DataType");
		return 0;

	}

	struct BufferElement
	{
		std::string Name;
		BufferDataType DataType;
		int Size;
		bool Normalised;
		int Offset = 0;

		BufferElement(const std::string& name, BufferDataType dataType, bool normalised)
			: Name(name), DataType(dataType), Size(GetDataTypeSize(dataType)), Normalised(normalised) {}

		int GetElementCount() const
		{
			switch (DataType)
			{
				case BufferDataType::Float: return 1;
				case BufferDataType::Float2: return 2;
				case BufferDataType::Float3: return 3;
				case BufferDataType::Float4: return 4;

				case BufferDataType::Mat3: return 3 * 3;
				case BufferDataType::Mat4: return 4 * 4;

				case BufferDataType::Integer: return 1;
				case BufferDataType::Integer2: return 2;
				case BufferDataType::Integer3: return 3;
				case BufferDataType::Integer4: return 4;
			}

			VX_CORE_CRITICAL("Undefined Buffer DataType");
			return 0;
		}
	};

	class BufferLayout
	{
	private:
		std::vector<BufferElement> m_Elements;
		int m_Stride = 0;
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements) 
		{
			CalculateStrideAndOffsets();
		}

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		const std::vector<BufferElement>& GetElements() const
		{
			return m_Elements;
		}

		void CalculateStrideAndOffsets()
		{
			m_Stride = 0;
			int offset = 0;
			for (auto& element : m_Elements)
			{
				m_Stride += element.Size;
				element.Offset = offset;
				offset += element.Size;
			}
		}

		int GetStride() const 
		{
			return m_Stride;
		}
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, int size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout & layout) = 0;

		static std::shared_ptr<VertexBuffer> Create(int size);
		static std::shared_ptr<VertexBuffer> Create(void* vertices, int size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, int size) = 0;
		virtual int GetSize() const = 0;

		static std::shared_ptr<IndexBuffer> Create(int size);
		static std::shared_ptr<IndexBuffer> Create(void* indices, int size);
	};
}
