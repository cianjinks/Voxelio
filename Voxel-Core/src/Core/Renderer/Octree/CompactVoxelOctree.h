#pragma once
#include <cstdint>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include "Util/Ray.h"

namespace VoxelCore {
	class CompactNode
	{
	private:

		// LAYOUT:
		// Color Index Data		Index	Valid Mask  Leaf Mask
		// 96 bits				16bits	8bits		8bits

		// Every Color Index is 12 bits meaning we can store up to 4096

		uint32_t m_Data = 0;
		uint32_t m_ColorIndex1 = 0;
		uint32_t m_ColorIndex2 = 0;
		uint32_t m_ColorIndex3 = 0;

	public:
		CompactNode();
		CompactNode(uint32_t data);
		~CompactNode();

		void SetData(uint32_t data);
		uint32_t GetData();

		void Activate(int index);
		void Deactivate(int index);

		void SetValidMaskBit(int index);
		void ClearValidMaskBit(int index);
		int GetValidMaskBit(int index);

		void SetLeafMaskBit(int index);
		void ClearLeafMaskBit(int index);
		int GetLeafMaskBit(int index);

		bool HasBranch();
		int GetBranchIndex(int index);

		void SetIndex(uint32_t index);
		uint32_t GetIndex();

		void SetColorIndex(uint32_t voxelIndex, uint32_t colorIndex);
		uint32_t GetColorIndex(uint32_t voxelIndex);

		operator uint32_t() const { return m_Data; };
	};

	struct RayTraceHit
	{
		CompactNode* node = nullptr;
		int childIndex = -1;
		glm::ivec3 nodeIndex = glm::ivec3(0);
		glm::ivec3 previousNodeIndex = glm::ivec3(0);

		RayTraceHit() {}
		RayTraceHit(CompactNode* node, int childIndex, glm::ivec3 nodeIndex, glm::ivec3 previousNodeIndex)
			: node(node), childIndex(childIndex), nodeIndex(nodeIndex), previousNodeIndex(previousNodeIndex) {}
	};

	class CompactVoxelOctree
	{
	private:
		std::vector<CompactNode> m_Nodes;
	public:
		int m_Dimension;
	private:

	public:
		CompactVoxelOctree(int levels);
		~CompactVoxelOctree();

		void GenerateOctree();
		void GenerateLevels();
		void Reload(int levels);

		void Activate(int x, int y, int z);
		void Deactivate(int x, int y, int z);
		int Get(int x, int y, int z);

		RayTraceHit RayTrace(Ray ray);

		void SetColorIndex(int x, int y, int z, uint32_t colorIndex);

		uint32_t* GetData();
		int GetNodeCount() const { return (int)m_Nodes.size(); };

		int m_OctreeLevels;
		std::map<int, std::string> m_OctreeLevelsMap;
		constexpr static int MAX_OCTREE_LEVELS = 6;
		constexpr static int MAX_OCTREE_NODES = 262144; // 8^6

		int get2DIndex(glm::vec3 index);
	};

	struct OctreeStackElement
	{
		CompactNode* node = nullptr;
		glm::vec3 position = glm::vec3(0);
		float size = 0;
		glm::vec3 index = glm::vec3(0);
		float h = 0;

		OctreeStackElement() {}
		OctreeStackElement(CompactNode* node, glm::vec3 pos, float size, glm::vec3 index, float h)
			: node(node), position(pos), size(size), index(index), h(h) {}
	};
}