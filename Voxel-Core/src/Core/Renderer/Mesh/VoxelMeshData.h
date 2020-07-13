#pragma once
#include "Core/Renderer/Buffer.h"
#include "glm/glm.hpp"

namespace VoxelCore {

	enum class MeshAxis
	{
		X, Y, Z
	};

	// Currently only contains color as position and normals are calculated based on vector index
	// TODO: Avoid data wastage (colors still stored even if inactive)
	struct VoxelIndexData
	{
		bool Active = true;
		glm::vec3 Color = glm::vec3(0.6f, 0.6f, 0.6f);
	};

	class VoxelMeshData
	{
	public:
		//X:        Y:          Z:
		std::vector<std::vector<std::vector<VoxelIndexData>>> m_Data;

		VoxelMeshData(int dimension);
		VoxelMeshData(std::vector<std::vector<std::vector<VoxelIndexData>>>& data);
		~VoxelMeshData();

	};
}