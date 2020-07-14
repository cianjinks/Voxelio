#include "voxelpch.h"
#include "VoxelMesh.h"
#include "Core/Renderer/Renderer.h"

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
		xDataSize = m_MeshData.m_Data.size();
		yDataSize = m_MeshData.m_Data[0].size();
		zDataSize = m_MeshData.m_Data[0][0].size();

		m_MeshIndices = 0;
		m_Vertices.clear();
		m_Vertices.reserve(xDataSize * yDataSize * zDataSize);

		lowerXBound = ((int)xDataSize - 1) / 2;
		lowerYBound = ((int)yDataSize - 1) / 2;
		lowerZBound = ((int)zDataSize - 1) / 2;

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

	void VoxelMesh::UpdateMesh(int xindex, int yindex, int zindex)
	{

	}

	glm::vec3 VoxelMesh::GetBlockPos(int xindex, int yindex, int zindex)
	{
		return glm::vec3(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound);
	}

	glm::vec3 VoxelMesh::GetBlockMin(int xindex, int yindex, int zindex)
	{
		glm::vec3 pos = GetBlockPos(xindex, yindex, zindex);
		return glm::vec3(pos.x - 0.5, pos.y - 0.5, pos.z - 0.5);
	}

	glm::vec3 VoxelMesh::GetBlockMax(int xindex, int yindex, int zindex)
	{
		glm::vec3 pos = GetBlockPos(xindex, yindex, zindex);
		return glm::vec3(pos.x + 0.5, pos.y + 0.5, pos.z + 0.5);
	}

	void VoxelMesh::SetBlockColor(int xindex, int yindex, int zindex, const glm::vec3 color)
	{
		m_MeshData.m_Data[xindex][yindex][zindex].Color = color;
		UpdateMesh(xindex, yindex, zindex);
	}

	void VoxelMesh::RemoveBlock(int xindex, int yindex, int zindex)
	{
		m_MeshData.m_Data[xindex][yindex][zindex].Active = false;
		UpdateMesh(xindex, yindex, zindex);
	}

	// Creation Functions
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