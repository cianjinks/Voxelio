#include "voxelpch.h"
#include "VoxelMesh.h"

namespace VoxelCore {

	VoxelMesh::VoxelMesh()
	{
		size_t xDataSize = m_MeshData.m_Data.size();
		size_t yDataSize = m_MeshData.m_Data[0].size();
		size_t zDataSize = m_MeshData.m_Data[0][0].size();

		m_Vertices.reserve(xDataSize * yDataSize * zDataSize);

		const size_t lowerXBound = xDataSize / 2;
		const size_t lowerYBound = yDataSize / 2;
		const size_t lowerZBound = zDataSize / 2;

		for (size_t x = 0; x < xDataSize; x++)
		{
			for (size_t y = 0; y < yDataSize; y++)
			{
				for (size_t z = 0; z < zDataSize; z++)
				{
					CreateCube(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
				}
			}
		}
	}

	VoxelMesh::VoxelMesh(VoxelMeshData& data)
		: m_MeshData(data) {}

	VoxelMesh::~VoxelMesh() {}

	void VoxelMesh::CreateCube(int x, int y, int z, glm::vec3& color)
	{
		float vertices[] = {
			// Position:								 Color:						Normal:
			-1.0f + x, -1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x, -1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x,  1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x,  1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x, -1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x, -1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x,  1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x,  1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x,  1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x,  1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x, -1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x, -1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x,  1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x,  1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x, -1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x, -1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x, -1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x, -1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x, -1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x, -1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x,  1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x,  1.0f + y, -1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			 1.0f + x,  1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f,
			-1.0f + x,  1.0f + y,  1.0f + z, color.x, color.y, color.z, 0.0f, 0.0f, 0.0f
		};

		m_MeshIndices += 6 * 6;
		m_Vertices.insert(m_Vertices.end(), std::begin(vertices), std::end(vertices));
	}
}