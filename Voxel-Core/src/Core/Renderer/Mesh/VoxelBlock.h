#pragma once
#include "glm/glm.hpp"

namespace VoxelCore {
	
	class VoxelFace 
	{
	public:
		static const int None = 0;
		static const int NegZFace = 1 << 0;
		static const int PosZFace = 1 << 1;
		static const int NegXFace = 1 << 2;
		static const int PosXFace = 1 << 3;
		static const int NegYFace = 1 << 4;
		static const int PosYFace = 1 << 5;
	};

	struct VoxelVertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec3 Normal;
	};

	class VoxelBlockGenerator
	{
	private:
		static void SetNegZFace(std::vector<float>* data, int index, int xindex, int yindex, int zindex, glm::vec3& color);
		static void SetPosZFace(std::vector<float>* data, int index, int xindex, int yindex, int zindex, glm::vec3& color);
		static void SetNegXFace(std::vector<float>* data, int index, int xindex, int yindex, int zindex, glm::vec3& color);
		static void SetPosXFace(std::vector<float>* data, int index, int xindex, int yindex, int zindex, glm::vec3& color);
		static void SetNegYFace(std::vector<float>* data, int index, int xindex, int yindex, int zindex, glm::vec3& color);
		static void SetPosYFace(std::vector<float>* data, int index, int xindex, int yindex, int zindex, glm::vec3& color);

		static void EmptyFace(std::vector<float>* data, int index);

	public:
		static const int s_VoxelVertexElementCount = 9;
		static const int s_VoxelFaceElementCount = 4 * s_VoxelVertexElementCount;
		static const int s_VoxelCubeElementCount = 6 * s_VoxelFaceElementCount;

		static const int s_VoxelFaceIndiceCount = 6;
		static const int s_VoxelCubeIndicesCount = 6 * s_VoxelFaceIndiceCount;

	public:
		static void GenerateBlock(std::vector<float>* data, int faces, int x, int y, int z, glm::vec3& color, int index);
		static void GenerateEmptyBlock(std::vector<float>* data, int index);
	};
}