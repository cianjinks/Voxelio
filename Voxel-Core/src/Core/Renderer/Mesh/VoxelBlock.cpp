#include "voxelpch.h"
#include "VoxelBlock.h"

namespace VoxelCore {

	void VoxelBlockGenerator::GenerateBlock(std::vector<float>* data, int faces, int x, int y, int z, glm::vec3& color, int index)
	{
		// TODO: Add blank data for null faces
		if (faces != VoxelFace::None) {
			if (faces & VoxelFace::NegZFace) {
				SetNegZFace(data, index + (0 * s_VoxelFaceElementCount), x, y, z, color);
			}
			else
			{
				EmptyFace(data, index + (0 * s_VoxelFaceElementCount));
			}

			if (faces & VoxelFace::PosZFace) {
				SetPosZFace(data, index + (1 * s_VoxelFaceElementCount), x, y, z, color);
			}
			else
			{
				EmptyFace(data, index + (1 * s_VoxelFaceElementCount));
			}

			if (faces & VoxelFace::NegXFace) {
				SetNegXFace(data, index + (2 * s_VoxelFaceElementCount), x, y, z, color);
			}
			else
			{
				EmptyFace(data, index + (2 * s_VoxelFaceElementCount));
			}

			if (faces & VoxelFace::PosXFace) {
				SetPosXFace(data, index + (3 * s_VoxelFaceElementCount), x, y, z, color);
			}
			else
			{
				EmptyFace(data, index + (3 * s_VoxelFaceElementCount));
			}

			if (faces & VoxelFace::NegYFace) {
				SetNegYFace(data, index + (4 * s_VoxelFaceElementCount), x, y, z, color);
			}
			else
			{
				EmptyFace(data, index + (4 * s_VoxelFaceElementCount));
			}

			if (faces & VoxelFace::PosYFace) {
				SetPosYFace(data, index + (5 * s_VoxelFaceElementCount), x, y, z, color);
			}
			else
			{
				EmptyFace(data, index + (5 * s_VoxelFaceElementCount));
			}
		}
		else
		{
			GenerateEmptyBlock(data, index);
		}
	}

	void VoxelBlockGenerator::GenerateEmptyBlock(std::vector<float>* data, int index)
	{
		std::fill(data->begin() + index, data->begin() + index + s_VoxelCubeElementCount, 0.0f);
	}

	// Creation Functions
	void VoxelBlockGenerator::SetNegZFace(std::vector<float>* data, int index, int x, int y, int z, glm::vec3& color)
	{
		data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		data->insert(data->begin() + index,
			{
				-0.5f + x, -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				 0.5f + x, -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				 0.5f + x,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f,
				-0.5f + x,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, -1.0f
			}
		);
	}

	void VoxelBlockGenerator::SetPosZFace(std::vector<float>* data, int index, int x, int y, int z, glm::vec3& color)
	{
		data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		data->insert(data->begin() + index,
			{
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				 0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				 0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f,
				-0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 0.0f, 1.0f
			}
		);
	}

	void VoxelBlockGenerator::SetNegXFace(std::vector<float>* data, int index, int x, int y, int z, glm::vec3& color)
	{
		data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		data->insert(data->begin() + index,
			{
				-0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x ,  0.5f + y, -0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f,
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, -1.0f, 0.0f, 0.0f
			}
		);
	}

	void VoxelBlockGenerator::SetPosXFace(std::vector<float>* data, int index, int x, int y, int z, glm::vec3& color)
	{

		data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		data->insert(data->begin() + index,
			{
				0.5f + x ,  0.5f + y,  0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x ,  0.5f + y, -0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f,
				0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 1.0f, 0.0f, 0.0f
			}
		);
	}

	void VoxelBlockGenerator::SetNegYFace(std::vector<float>* data, int index, int x, int y, int z, glm::vec3& color)
	{
		data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		data->insert(data->begin() + index,
			{
				-0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				 0.5f + x , -0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				 0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f,
				-0.5f + x , -0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, -1.0f, 0.0f
			}
		);
	}

	void VoxelBlockGenerator::SetPosYFace(std::vector<float>* data, int index, int x, int y, int z, glm::vec3& color)
	{
		data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		data->insert(data->begin() + index,
			{
				-0.5f + x , 0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
				 0.5f + x , 0.5f + y, -0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
				 0.5f + x , 0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
				-0.5f + x , 0.5f + y,  0.5f + z, color.x, color.y, color.z, 0.0f, 1.0f, 0.0f,
			}
		);
	}

	void VoxelBlockGenerator::EmptyFace(std::vector<float>* data, int index)
	{
		//data->erase(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount);
		//std::fill(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount, 0.0f);
		std::fill(data->begin() + index, data->begin() + index + s_VoxelFaceElementCount, 0.0f);
	}
}