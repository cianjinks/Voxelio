#pragma once
#include "VoxelMeshData.h"
#include "VoxelBlock.h"

namespace VoxelCore {

	class VoxelMesh 
	{
	private:
		VoxelMeshData m_MeshData;
		std::vector<std::vector<std::vector<VoxelBlock>>> m_Blocks;

		int m_MeshIndices = 0;
		int lowerXBound = 0, lowerYBound = 0, lowerZBound = 0;
		size_t xDataSize = 0, yDataSize = 0, zDataSize = 0;

	public:
		VoxelMesh(int dimension);
		VoxelMesh(VoxelMeshData& data);
		~VoxelMesh();
		
		void UpdateMesh();
		void UpdateMesh(int xindex, int yindex, int zindex);
		void RemoveBlock(int xindex, int yindex, int zindex);

		glm::vec3 GetBlockPos(int xindex, int yindex, int zindex);
		glm::vec3 GetBlockMin(int xindex, int yindex, int zindex);
		glm::vec3 GetBlockMax(int xindex, int yindex, int zindex);

		void SetBlockColor(int xindex, int yindex, int zindex, const glm::vec3& color);

		const std::vector<std::vector<std::vector<VoxelBlock>>>& GetBlocks() const { return m_Blocks; };
		const int GetIndicesCount() const { return m_MeshIndices; };
	};
}