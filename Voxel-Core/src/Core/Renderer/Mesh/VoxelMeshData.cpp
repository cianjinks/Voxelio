#include "voxelpch.h"
#include "VoxelMeshData.h"

namespace VoxelCore {

	VoxelMeshData::VoxelMeshData(int dimension)
	{
		// By default constructs a 10x10 mesh of color red
		const int meshSize = dimension + 2;
		m_Data.reserve(meshSize);

		for (int i = 0; i < meshSize; i++) 
		{
			float c = 0.0f;
			std::vector<std::vector<VoxelIndexData>> v1;
			v1.reserve(meshSize);
			for (int j = 0; j < meshSize; j++)
			{
				std::vector<VoxelIndexData> v2;
				v2.reserve(meshSize);
				for (int k = 0; k < meshSize; k++)
				{
					VoxelIndexData data;
					if (i == 0 || j == 0 || k == 0 || i == meshSize - 1 || j == meshSize - 1 || k == meshSize - 1) {
						data.Active = false;
					}
					data.Color = glm::vec3(c, c, c);
					v2.emplace_back(std::move(data));
				}
				c += 0.09f / 1.0f;
				v1.emplace_back(std::move(v2));
			}
			m_Data.emplace_back(std::move(v1));
		}
	}

	VoxelMeshData::VoxelMeshData(std::vector<std::vector<std::vector<VoxelIndexData>>>& data)
		: m_Data(data) {}

	VoxelMeshData::~VoxelMeshData() {}
}