#pragma once
#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>

namespace VoxelCore {
	class CompactNode
	{
	private:

		// LAYOUT:
		// Spare Index	Color Index  Voxel Index  Valid Mask  Leaf Mask
		// 16bits		16bits		 16bits	      8bits		  8bits

		uint64_t m_Data;
	public:
		CompactNode();
		CompactNode(uint64_t data);
		~CompactNode();

		void SetData(uint64_t data);
		uint64_t GetData();

		void SetValidMaskBit(uint64_t index);
		void ClearValidMaskBit(uint64_t index);
		uint64_t GetValidMaskBit(uint64_t index);

		void SetLeafMaskBit(uint64_t index);
		void ClearLeafMaskBit(uint64_t index);
		uint64_t GetLeafMaskBit(uint64_t index);

		bool HasBranch();
		uint64_t GetBranchIndex(uint64_t index);

		void SetIndex(uint64_t index);
		uint64_t GetIndex();

		void SetColorIndex(uint64_t index);
		uint64_t GetColorIndex();

		operator uint64_t() const { return m_Data; };
	};

	class CompactVoxelOctree
	{
	private:
		std::vector<CompactNode> m_Nodes;
		int m_Levels;

	public:
		CompactVoxelOctree(int levels);
		~CompactVoxelOctree();

		void Activate(int x, int y, int z);
		void Deactivate(int x, int y, int z);
		int Get(int x, int y, int z);

		void SetColorIndex(int x, int y, int z, uint64_t colorIndex);

		uint64_t* GetData();
		int GetNodeCount() const { return m_Nodes.size(); };

		constexpr static int MAX_OCTREE_NODES = 8 * 8 * 8;
	};
}