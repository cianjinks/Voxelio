#include "voxelpch.h"
#include "VoxelMesh.h"
#include "Core/Renderer/Renderer.h"

namespace VoxelCore {

	VoxelMesh::VoxelMesh(int dimension)
	{
		m_MeshData = new VoxelMeshData(dimension);
		UpdateMesh();
	}

	VoxelMesh::~VoxelMesh() { 
		m_VoxelData->clear();
		delete m_VoxelData; 
		delete m_MeshData;
	}

	void VoxelMesh::UpdateMesh()
	{
		xDataSize = m_MeshData->m_Data.size();
		yDataSize = m_MeshData->m_Data[0].size();
		zDataSize = m_MeshData->m_Data[0][0].size();

		lowerXBound = ((int)xDataSize) / 2;
		lowerYBound = ((int)yDataSize) / 2;
		lowerZBound = ((int)zDataSize) / 2;

		m_MeshIndices = 0;
		m_VoxelData = new std::vector<float>(xDataSize * yDataSize * zDataSize * VoxelBlockGenerator::s_VoxelCubeElementCount);
		//m_VoxelData->clear();
		//std::fill(m_VoxelData->begin(), m_VoxelData->end(), 0.0f);

		int blockCount = 0;
		for (int x = 0; x < xDataSize; x++)
		{
			for (int y = 0; y < yDataSize; y++)
			{
				for (int z = 0; z < zDataSize; z++)
				{
					//int index = ((x * yDataSize * zDataSize) + (y * zDataSize) + z) * VoxelBlockGenerator::s_VoxelCubeElementCount;
					//int index = ((z * yDataSize * xDataSize) + (y * xDataSize) + x) * VoxelBlockGenerator::s_VoxelCubeElementCount;
					int index = blockCount * VoxelBlockGenerator::s_VoxelCubeElementCount;
					if (m_MeshData->m_Data[x][y][z].Active) {
						int faces = VoxelFace::None;
						if (!m_MeshData->m_Data[x][y][(size_t)z - 1].Active) { faces = faces | VoxelFace::NegZFace; }
						if (!m_MeshData->m_Data[x][y][(size_t)z + 1].Active) { faces = faces | VoxelFace::PosZFace; }
						if (!m_MeshData->m_Data[(size_t)x - 1][y][z].Active) { faces = faces | VoxelFace::NegXFace; }
						if (!m_MeshData->m_Data[(size_t)x + 1][y][z].Active) { faces = faces | VoxelFace::PosXFace; }
						if (!m_MeshData->m_Data[x][(size_t)y - 1][z].Active) { faces = faces | VoxelFace::NegYFace; }
						if (!m_MeshData->m_Data[x][(size_t)y + 1][z].Active) { faces = faces | VoxelFace::PosYFace; }
						VoxelBlockGenerator::GenerateBlock(m_VoxelData, faces, x - lowerXBound, y - lowerYBound, z - lowerZBound, m_MeshData->m_Data[x][y][z].Color, index);
					}
					else
					{
						VoxelBlockGenerator::GenerateEmptyBlock(m_VoxelData, index);
					}
					m_MeshIndices += VoxelBlockGenerator::s_VoxelCubeIndicesCount;
					blockCount++;
				}
			}
		}
	}

	void VoxelMesh::UpdateMesh(int xindex, int yindex, int zindex)
	{
		/**auto& block = m_Blocks[xindex][yindex][zindex];
		m_MeshIndices -= block.GetIndices();
		block.Reset();
		if (m_MeshData.m_Data[xindex][yindex][zindex].Active) {
			// Negative Z Face
			if (!m_MeshData.m_Data[xindex][yindex][(size_t)zindex - 1].Active)
			{
				block.AddNegZFace(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound, m_MeshData.m_Data[xindex][yindex][zindex].Color);
			}
			// Positive Z Face
			if (!m_MeshData.m_Data[xindex][yindex][(size_t)zindex + 1].Active)
			{
				block.AddPosZFace(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound, m_MeshData.m_Data[xindex][yindex][zindex].Color);
			}
			// Negative X Face
			if (!m_MeshData.m_Data[(size_t)xindex - 1][yindex][zindex].Active)
			{
				block.AddNegXFace(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound, m_MeshData.m_Data[xindex][yindex][zindex].Color);
			}
			// Positive X Face
			if (!m_MeshData.m_Data[(size_t)xindex + 1][yindex][zindex].Active)
			{
				block.AddPosXFace(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound, m_MeshData.m_Data[xindex][yindex][zindex].Color);
			}
			// Negative Y Face
			if (!m_MeshData.m_Data[xindex][(size_t)yindex - 1][zindex].Active)
			{
				block.AddNegYFace(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound, m_MeshData.m_Data[xindex][yindex][zindex].Color);
			}
			// Positive Y Face
			if (!m_MeshData.m_Data[xindex][(size_t)yindex + 1][zindex].Active)
			{
				block.AddPosYFace(xindex - lowerXBound, yindex - lowerYBound, zindex - lowerZBound, m_MeshData.m_Data[xindex][yindex][zindex].Color);
			}
		}
		m_MeshIndices += block.GetIndices();
		**/
	}

	void VoxelMesh::UpateSurroundingMesh(int xindex, int yindex, int zindex)
	{
		// TODO: Could optimise to only update blocks that are opposite active faces
		UpdateMesh(xindex + 1, yindex, zindex);
		UpdateMesh(xindex - 1, yindex, zindex);
		UpdateMesh(xindex, yindex + 1, zindex);
		UpdateMesh(xindex, yindex - 1, zindex);
		UpdateMesh(xindex, yindex, zindex + 1);
		UpdateMesh(xindex, yindex, zindex - 1);
		UpdateMesh(xindex, yindex, zindex);
	}

	void VoxelMesh::SetBlockColor(int xindex, int yindex, int zindex, const glm::vec3& color)
	{
		m_MeshData->m_Data[xindex][yindex][zindex].Color = color;
		UpdateMesh(xindex, yindex, zindex);
	}

	void VoxelMesh::EnableBlock(int xindex, int yindex, int zindex)
	{
		m_MeshData->m_Data[xindex][yindex][zindex].Active = true;
		UpateSurroundingMesh(xindex, yindex, zindex);
	}

	void VoxelMesh::DisableBlock(int xindex, int yindex, int zindex)
	{
		m_MeshData->m_Data[xindex][yindex][zindex].Active = false;
		UpateSurroundingMesh(xindex, yindex, zindex);
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
}