#pragma once
#include "VoxelMeshData.h"

namespace VoxelCore {

	class VoxelMesh 
	{
	private:
		VoxelMeshData m_MeshData;
		std::vector<float> m_Vertices;
		int m_MeshIndices = 0;

	private:
		void AddNegZFace(int x, int y, int z, glm::vec3& color);
		void AddPosZFace(int x, int y, int z, glm::vec3& color);
		void AddNegXFace(int x, int y, int z, glm::vec3& color);
		void AddPosXFace(int x, int y, int z, glm::vec3& color);
		void AddNegYFace(int x, int y, int z, glm::vec3& color);
		void AddPosYFace(int x, int y, int z, glm::vec3& color);

	public:
		VoxelMesh(int dimension);
		VoxelMesh(VoxelMeshData& data);
		~VoxelMesh();
		
		const std::vector<float>& GetVertices() const { return m_Vertices; };
		const int GetIndicesCount() const { return m_MeshIndices; };
	};
}