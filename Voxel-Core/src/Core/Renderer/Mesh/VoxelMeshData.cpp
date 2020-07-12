#include "voxelpch.h"
#include "VoxelMeshData.h"

namespace VoxelCore {

	VoxelMeshData::VoxelMeshData()
	{
		// By default constructs a 10x10 mesh of color red
		const int meshSize = 10;
		m_Data.reserve(meshSize);

		VoxelIndexData data;
		data.Active = true;
		data.Color = glm::vec3(1.0f, 0.0f, 0.0f);

		for (int i = 0; i < meshSize; i++) 
		{
			std::vector<std::vector<VoxelIndexData>> v1;
			v1.reserve(meshSize);
			for (int j = 0; j < meshSize; j++)
			{
				std::vector<VoxelIndexData> v2;
				v2.reserve(meshSize);
				for (int k = 0; k < meshSize; k++)
				{
					v2.emplace_back(std::move(data));
				}
				v1.emplace_back(std::move(v2));
			}
			m_Data.emplace_back(std::move(v1));
		}
	}

	VoxelMeshData::VoxelMeshData(std::vector<std::vector<std::vector<VoxelIndexData>>>& data)
		: m_Data(data) {}

	VoxelMeshData::~VoxelMeshData() {}
}