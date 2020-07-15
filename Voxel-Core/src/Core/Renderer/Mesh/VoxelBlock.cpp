#include "voxelpch.h"
#include "VoxelBlock.h"

namespace VoxelCore {

	void VoxelBlock::Reset()
	{
		m_Indices = 0;
		m_Vertices.clear();
	}

	// Creation Functions
	void VoxelBlock::AddNegZFace(int x, int y, int z, glm::vec3& color)
	{
		m_Indices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x, -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				 0.5f + x, -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				 0.5f + x,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				-0.5f + x,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f
			}
		);
	}

	void VoxelBlock::AddPosZFace(int x, int y, int z, glm::vec3& color)
	{
		m_Indices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				 0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				 0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				-0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f
			}
		);
	}

	void VoxelBlock::AddNegXFace(int x, int y, int z, glm::vec3& color)
	{
		m_Indices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x ,  0.5f + y, -0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f
			}
		);
	}

	void VoxelBlock::AddPosXFace(int x, int y, int z, glm::vec3& color)
	{
		m_Indices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x ,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f
			}
		);
	}

	void VoxelBlock::AddNegYFace(int x, int y, int z, glm::vec3& color)
	{
		m_Indices += 6;
		m_Vertices.insert(m_Vertices.end(),
			{
				-0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				 0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				 0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
			}
		);
	}

	void VoxelBlock::AddPosYFace(int x, int y, int z, glm::vec3& color)
	{
		m_Indices += 6;
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