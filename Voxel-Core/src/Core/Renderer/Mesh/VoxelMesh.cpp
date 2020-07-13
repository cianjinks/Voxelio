#include "voxelpch.h"
#include "VoxelMesh.h"

namespace VoxelCore {

	VoxelMesh::VoxelMesh(int dimension)
		: m_MeshData(dimension)
	{
		size_t xDataSize = m_MeshData.m_Data.size();
		size_t yDataSize = m_MeshData.m_Data[0].size();
		size_t zDataSize = m_MeshData.m_Data[0][0].size();

		m_Vertices.reserve(xDataSize * yDataSize * zDataSize);

		const size_t lowerXBound = (xDataSize - 1) / 2;
		const size_t lowerYBound = (yDataSize - 1) / 2;
		const size_t lowerZBound = (zDataSize - 1) / 2;

		for (size_t x = 1; x < xDataSize - 1; x++)
		{
			for (size_t y = 1; y < yDataSize - 1; y++)
			{
				for (size_t z = 1; z < zDataSize - 1; z++)
				{
					// Negative Z Face
					if (!m_MeshData.m_Data[x][y][z - 1].Active) 
					{ AddNegZFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color); }
					// Positive Z Face
					if (!m_MeshData.m_Data[x][y][z + 1].Active) 
					{ AddPosZFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color); }
					// Negative X Face
					if (!m_MeshData.m_Data[x - 1][y][z].Active)
					{ AddNegXFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color); }
					// Positive X Face
					if (!m_MeshData.m_Data[x + 1][y][z].Active) 
					{ AddPosXFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color); }
					// Negative Y Face
					if (!m_MeshData.m_Data[x][y - 1][z].Active) 
					{ AddNegYFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color); }
					// Positive Y Face
					if (!m_MeshData.m_Data[x][y + 1][z].Active) 
					{ AddPosYFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color); }
				}
			}
		}
	}

	VoxelMesh::VoxelMesh(VoxelMeshData& data)
		: m_MeshData(data) {}

	VoxelMesh::~VoxelMesh() {}

	void VoxelMesh::AddNegZFace(int x, int y, int z, glm::vec3& color) 
	{
		m_MeshIndices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x, -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				 0.5f + x, -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				 0.5f + x,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				-0.5f + x,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f
			}
		);
	}

	void VoxelMesh::AddPosZFace(int x, int y, int z, glm::vec3& color) 
	{
		m_MeshIndices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				 0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				 0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				-0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f
			}
		);
	}

	void VoxelMesh::AddNegXFace(int x, int y, int z, glm::vec3& color) 
	{
		m_MeshIndices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x ,  0.5f + y, -0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f
			}
		);
	}

	void VoxelMesh::AddPosXFace(int x, int y, int z, glm::vec3& color) 
	{
		m_MeshIndices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x ,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f
			}
		);
	}

	void VoxelMesh::AddNegYFace(int x, int y, int z, glm::vec3& color) 
	{
		m_MeshIndices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				 0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				 0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
			}
		);
	}

	void VoxelMesh::AddPosYFace(int x, int y, int z, glm::vec3& color) 
	{
		m_MeshIndices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x , 0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
				 0.5f + x , 0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
				 0.5f + x , 0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
				-0.5f + x , 0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
			}
		);
	}
}