#pragma once
#include "glm/glm.hpp"

namespace VoxelCore {

	class VoxelBlock
	{
	private:
		std::vector<float> m_Vertices;
		int m_Indices = 0;
	public:
		void Reset();

		void AddNegZFace(int xindex, int yindex, int zindex, glm::vec3& color);
		void AddPosZFace(int xindex, int yindex, int zindex, glm::vec3& color);
		void AddNegXFace(int xindex, int yindex, int zindex, glm::vec3& color);
		void AddPosXFace(int xindex, int yindex, int zindex, glm::vec3& color);
		void AddNegYFace(int xindex, int yindex, int zindex, glm::vec3& color);
		void AddPosYFace(int xindex, int yindex, int zindex, glm::vec3& color);

		const std::vector<float>& GetVertices() const { return m_Vertices; };
		const int GetIndices() const { return m_Indices; };
	};
}