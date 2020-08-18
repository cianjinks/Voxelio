#pragma once
#include <cstdint>
#include <cmath>

namespace VoxelCore {
	class CompactNode
	{
	private:

		// LAYOUT:
		// Index	Valid Mask  Leaf Mask
		// 16bits	8bits		8bits

		uint32_t m_Data;
	public:
		CompactNode();
		CompactNode(uint32_t data);
		~CompactNode();

		void SetData(uint32_t data);
		uint32_t GetData();

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

		operator uint32_t() const { return m_Data; };
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

		uint32_t* GetData();
		int GetNodeCount() const { return m_Nodes.size(); };

		constexpr static int MAX_OCTREE_NODES = 8 * 8 * 8;
	};
}