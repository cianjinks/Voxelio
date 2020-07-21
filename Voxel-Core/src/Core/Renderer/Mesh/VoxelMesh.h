#pragma once
#include "VoxelMeshData.h"
#include "VoxelBlock.h"

namespace VoxelCore {

	class VoxelMesh 
	{
	private:
		VoxelMeshData* m_MeshData;
		std::vector<float>* m_VoxelData;

		int m_MeshIndices = 0;
		int lowerXBound = 0, lowerYBound = 0, lowerZBound = 0;
		size_t xDataSize = 0, yDataSize = 0, zDataSize = 0;

	public:
		VoxelMesh(int dimension);
		~VoxelMesh();
		
		void UpdateMesh();
		void UpdateMesh(int xindex, int yindex, int zindex);
		void UpateSurroundingMesh(int xindex, int yindex, int zindex);
		void EnableBlock(int xindex, int yindex, int zindex);
		void DisableBlock(int xindex, int yindex, int zindex);
		void SetBlockColor(int xindex, int yindex, int zindex, const glm::vec3& color);


		glm::vec3 GetBlockPos(int xindex, int yindex, int zindex);
		glm::vec3 GetBlockMin(int xindex, int yindex, int zindex);
		glm::vec3 GetBlockMax(int xindex, int yindex, int zindex);

		std::vector<float>* GetData() { return m_VoxelData; };
		const int GetIndicesCount() const { return m_MeshIndices; };
	};
}