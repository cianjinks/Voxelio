#include "voxelpch.h"
#include "VoxelMesh.h"

namespace VoxelCore {

	VoxelMesh::VoxelMesh(int dimension)
		: m_MeshData(dimension)
	{
		UpdateMesh();
	}

	VoxelMesh::VoxelMesh(VoxelMeshData& data)
		: m_MeshData(data) 
	{
		UpdateMesh();
	}

	VoxelMesh::~VoxelMesh() {}

	void VoxelMesh::UpdateMesh()
	{
		size_t xDataSize = m_MeshData.m_Data.size();
		size_t yDataSize = m_MeshData.m_Data[0].size();
		size_t zDataSize = m_MeshData.m_Data[0][0].size();

		m_MeshIndices = 0;
		m_Vertices.clear();
		m_Vertices.reserve(xDataSize * yDataSize * zDataSize);

		const int lowerXBound = ((int)xDataSize - 1) / 2;
		const int lowerYBound = ((int)yDataSize - 1) / 2;
		const int lowerZBound = ((int)zDataSize - 1) / 2;

		for (int x = 1; x < xDataSize - 1; x++)
		{
			for (int y = 1; y < yDataSize - 1; y++)
			{
				for (int z = 1; z < zDataSize - 1; z++)
				{
					if (m_MeshData.m_Data[x][y][z].Active) {
						// Negative Z Face
						if (!m_MeshData.m_Data[x][y][(size_t)z - 1].Active)
						{
							AddNegZFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
						}
						// Positive Z Face
						if (!m_MeshData.m_Data[x][y][(size_t)z + 1].Active)
						{
							AddPosZFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
						}
						// Negative X Face
						if (!m_MeshData.m_Data[(size_t)x - 1][y][z].Active)
						{
							AddNegXFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
						}
						// Positive X Face
						if (!m_MeshData.m_Data[(size_t)x + 1][y][z].Active)
						{
							AddPosXFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
						}
						// Negative Y Face
						if (!m_MeshData.m_Data[x][(size_t)y - 1][z].Active)
						{
							AddNegYFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
						}
						// Positive Y Face
						if (!m_MeshData.m_Data[x][(size_t)y + 1][z].Active)
						{
							AddPosYFace(x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData.m_Data[x][y][z].Color);
						}
					}
				}
			}
		}
	}

	void VoxelMesh::RemoveBlock(int x, int y, int z)
	{
		m_MeshData.m_Data[x][y][z].Active = false;
		UpdateMesh();
	}

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