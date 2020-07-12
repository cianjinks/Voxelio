#pragma once
#include "VoxelMeshData.h"

namespace VoxelCore {

	class VoxelMesh 
	{
	private:
		VoxelMeshData m_MeshData;
		std::vector<float> m_Vertices;
		int m_MeshIndices = 0;

	public:
		VoxelMesh();
		VoxelMesh(VoxelMeshData& data);
		~VoxelMesh();

		void CreateCube(int x, int y, int z, glm::vec3& color);

		const std::vector<float>& GetVertices() const { return m_Vertices; };
		const int GetIndicesCount() const { return m_MeshIndices; };
	};
}