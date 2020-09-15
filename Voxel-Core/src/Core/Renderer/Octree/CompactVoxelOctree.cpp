#include "voxelpch.h"
#include "CompactVoxelOctree.h"

namespace VoxelCore {
	// NODE
	CompactNode::CompactNode()
		: m_Data(0) {}

	CompactNode::CompactNode(uint32_t data)
		: m_Data(data) {}

	CompactNode::~CompactNode() {}

	void CompactNode::SetData(uint32_t data) { m_Data = data; }

	uint32_t CompactNode::GetData() { return m_Data; }

	void CompactNode::SetValidMaskBit(int index)
	{
		uint32_t bit = 1 << (8 + index);
		m_Data |= bit;
	}

	void CompactNode::ClearValidMaskBit(int index)
	{
		uint32_t bit = 1 << (8 + index);
		m_Data &= ~bit;
	}

	int CompactNode::GetValidMaskBit(int index)
	{
		uint32_t clearmask = 0xFFFFFFFE;
		uint32_t bit = m_Data >> (8 + index);
		return bit & ~clearmask;
	}

	void CompactNode::SetLeafMaskBit(int index)
	{
		uint32_t bit = 1 << index;
		m_Data |= bit;
	}
	void CompactNode::ClearLeafMaskBit(int index)
	{
		uint32_t bit = 1 << index;
		m_Data &= ~bit;
	}

	int CompactNode::GetLeafMaskBit(int index)
	{
		uint32_t clearmask = 0xFFFFFFFE;
		uint32_t bit = m_Data >> index;
		return bit & ~clearmask;
	}

	bool CompactNode::HasBranch()
	{
		uint32_t branchbits = m_Data & ~(0xFFFFFF00);
		uint32_t enabledbits = m_Data & ~(0xFFFF00FF);
		branchbits ^= 0x000000FF;
		if (branchbits > 0)
		{
			if ((branchbits & enabledbits) > 0)
			{
				return false;
			}
			return true;
		}
		return false;
	}

	// Assumes the index is definitely a valid branch
	int CompactNode::GetBranchIndex(int index)
	{
		int count = -1;
		for (int i = 0; i <= index; i++)
		{
			if (GetLeafMaskBit(i) == 0)
			{
				count++;
			}
		}
		return count;
	}

	void CompactNode::SetIndex(uint32_t index)
	{
		uint32_t aligned = index << 16;
		uint32_t clearmask = 0xFFFF0000;
		m_Data &= ~clearmask;
		m_Data |= aligned;
	}

	uint32_t CompactNode::GetIndex()
	{
		uint32_t clearmask = 0x0000FFFF;
		return (m_Data & ~clearmask) >> 16;
	}

	void CompactNode::SetColorIndex(uint32_t voxelIndex, uint32_t colorIndex)
	{
		if (voxelIndex <= 7 || voxelIndex >= 0)
		{
			switch (voxelIndex)
			{
				case 0:
				{
					uint32_t aligned = colorIndex << 20;
					m_ColorIndex1 &= 0x000FFFFF;
					m_ColorIndex1 |= aligned;
					break;
				}
				case 1:
				{
					uint32_t aligned = colorIndex << 8;
					m_ColorIndex1 &= 0xFFF000FF;
					m_ColorIndex1 |= aligned;
					break;
				}
				case 2:
				{
					// First 8 bits of colorIndex go into m_ColorIndex1
					uint32_t firstEight = colorIndex >> 4;
					m_ColorIndex1 &= 0xFFFFFF00;
					m_ColorIndex1 |= firstEight;

					// Last 4 bits of colorIndex go into m_ColorIndex2
					uint32_t lastFour = (colorIndex & 0x0000000F) << 28;
					m_ColorIndex2 &= 0x0FFFFFFF;
					m_ColorIndex2 |= lastFour;
					break;
				}
				case 3:
				{
					uint32_t aligned = colorIndex << 16;
					m_ColorIndex2 &= 0xF000FFFF;
					m_ColorIndex2 |= aligned;
					break;
				}
				case 4:
				{
					uint32_t aligned = colorIndex << 4;
					m_ColorIndex2 &= 0xFFFF000F;
					m_ColorIndex2 |= aligned;
					break;
				}
				case 5:
				{
					// First 4 bits of colorIndex go into m_ColorIndex2
					uint32_t firstFour = colorIndex >> 8;
					m_ColorIndex2 &= 0xFFFFFFF0;
					m_ColorIndex2 |= firstFour;

					// Last 8 bits of colorIndex go into m_ColorIndex3
					uint32_t lastEight = (colorIndex & 0x000000FF) << 24;
					m_ColorIndex3 &= 0x00FFFFFF;
					m_ColorIndex3 |= lastEight;
					break;
				}
				case 6:
				{
					uint32_t aligned = colorIndex << 12;
					m_ColorIndex3 &= 0xFF000FFF;
					m_ColorIndex3 |= aligned;
					break;
				}
				case 7:
				{
					uint32_t aligned = colorIndex << 0;
					m_ColorIndex3 &= 0xFFFFF000;
					m_ColorIndex3 |= aligned;
					break;
				}
			}
		}
	}

	uint32_t CompactNode::GetColorIndex(uint32_t voxelIndex)
	{
		uint32_t clearmask = 0x00000FFF;
		if (voxelIndex <= 7 || voxelIndex >= 0)
		{
			switch (voxelIndex)
			{
				case 0:
				{
					return m_ColorIndex1 >> 24;
				}
				case 1:
				{
					return (m_ColorIndex1 >> 8) & clearmask;
				}
				case 2:
				{
					// Last 8 bits of m_ColorIndex1 are first 8 bits of color index
					uint32_t firstEight = (m_ColorIndex1 & 0x000000FF) << 4;
					// First 4 bits of m_ColorIndex2 are last 4 bits of color index
					uint32_t lastFour = (m_ColorIndex2 & 0xF0000000) >> 28;

					return firstEight | lastFour;
				}
				case 3:
				{
					return (m_ColorIndex2 >> 16) & clearmask;
				}
				case 4:
				{
					return (m_ColorIndex2 >> 4) & clearmask;
				}
				case 5:
				{
					// Last 4 bits of m_ColorIndex2 are first 4 bits of color index
					uint32_t firstFour = (m_ColorIndex2 & 0x0000000F) << 8;
					// First 8 bits of m_ColorIndex3 are last 8 bits of color index
					uint32_t lastEight = (m_ColorIndex3 & 0xFF000000) >> 24;

					return firstFour | lastEight;
				}
				case 6:
				{
					return (m_ColorIndex3 >> 12) & clearmask;
				}
				case 7:
				{
					return m_ColorIndex3 & clearmask;
				}
			}
		}
	}

	// OCTREE
	CompactVoxelOctree::CompactVoxelOctree(int levels)
		: m_Levels(levels)
	{
		const int count = std::pow(8, m_Levels);

		// TEST
		/**uint32_t array[230] = { 66046, 196352, 719365, 1111306, 1504267, 1798287, 2010960, 2383776, 2760672, 3081212, 3244159, 3317855, 3448927, 3604224, 4127237, 4521728, 4997311, 5075087, 5308160, 5821483, 6094592, 6583690, 6924885, 7205389, 7533324, 7879365, 8190469, 8581900, 8917486, 9045243, 9153109, 9436928, 9961216, 10485504, 10945277, 11012853, 11206400, 11685298, 11992832, 12474280, 12782835, 12914672, 13238016, 13729664, 14169036, 14418430, 14484221, 14565312, 14946798, 33023, 57599, 12543, 52479, 41727, 51455, 52479, 8447, 65535, 52479, 52479, 65535, 65535, 57599, 61695, 35071, 65535, 43775, 65535, 61695, 65535, 62207, 65535, 65535, 65535, 65535, 65535, 49407, 61695, 20735, 12543, 52479, 50431, 65535, 65535, 52479, 52479, 65535, 65535, 20735, 50431, 65535, 54783, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 62975, 28927, 65535, 12543, 65535, 33023, 61695, 56831, 14335, 12543, 60159, 65535, 47871, 65535, 61695, 61695, 65535, 65535, 29695, 12543, 65535, 511, 8447, 47103, 49407, 63743, 65535, 19711, 8959, 1535, 61695, 61695, 65535, 65535, 13311, 12799, 4351, 30719, 5631, 35071, 35071, 35071, 767, 52479, 52479, 65535, 65535, 52479, 52479, 8191, 49151, 35583, 65535, 44031, 65535, 2303, 36863, 2815, 45055, 65535, 65535, 65535, 65535, 53247, 61439, 65535, 65535, 3327, 2815, 767, 52479, 52479, 65535, 65535, 52479, 52479, 65535, 65535, 52479, 65535, 56831, 19967, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 13311, 65535, 24575, 24575, 3327, 1279, 4095, 3583, 4095, 511, 61183, 65535, 65535, 65535, 61183, 57343, 49151, 32767, 65535, 65535, 22527, 1023, 4095, 4095, 511, 13311, 13311, 767, 1023, 4607, 1279, 65535, 65535, 13311, 13311, 16383, 8191, 30719, 511 };
		for (int i = 0; i < 230; i++)
		{
			m_Nodes.emplace_back(array[i]);
		}**/
		m_Nodes.reserve(MAX_OCTREE_NODES);
		m_Nodes.emplace_back(0x0001FFFF);
		//m_Nodes.emplace_back(0x00000000);
	}

	CompactVoxelOctree::~CompactVoxelOctree()
	{

	}

	void CompactVoxelOctree::Activate(int x, int y, int z)
	{
		int sizelength = std::pow(2, m_Levels);
		// Makes sure index is in bounds (otherwise undefined behavior can occur)
		if ((x < sizelength && x >= 0) && (y < sizelength && y >= 0) && (z < sizelength && z >= 0))
		{
			VX_CORE_INFO("[Activate Subnode] X: {0} Y {1} Z {2}", x, y, z);

			CompactNode* node = &m_Nodes[0]; // Retrieve the root node
			int size = std::pow(2, m_Levels - 1);

			while (size != 0)
			{
				int index = (x / size) + 2 * (y / size) + 4 * (z / size);
				// Check if subnode at index is a branch or a leaf
				if (node->GetValidMaskBit(index) == 1 && node->GetLeafMaskBit(index) == 0) // This would mean it is a branch
				{
					node = &m_Nodes[(size_t)node->GetIndex() + (size_t)node->GetBranchIndex(index)];
					x -= size * (x / size);
					y -= size * (y / size);
					z -= size * (z / size);
					size /= 2;
				}
				else
				{
					// If it's not a branch we need to check if this is the final subnode and thus supposed leaf
					if (size == 1)
					{
						// Deactivate the subnode if it is the final one
						node->SetLeafMaskBit(index);
						node->SetValidMaskBit(index);
						break;
					}
					else
					{
						// Turn the leaf into a branch if it is not the final one

						// Check if a branch index already exists:
						bool branch = node->HasBranch();

						node->ClearLeafMaskBit(index);
						node->SetValidMaskBit(index);
						if (branch)
						{
							// In this case I think it will ruin all other nodes index (solved via a freelist or padding as done below)
							m_Nodes[(size_t)node->GetIndex() + (size_t)node->GetBranchIndex(index)] = 0x0000FFFF;
						}
						else
						{
							node->SetIndex(m_Nodes.size());
							m_Nodes.emplace_back(0x0000FFFF);
							// Add 7 spaces of padding for future branches:
							m_Nodes.insert(m_Nodes.end(), { 0, 0, 0, 0, 0, 0, 0 });
						}
					}
				}
			}
		}
		else
		{
			VX_CORE_CRITICAL("Octree Index {0}, {1}, {2} is out of bounds", x, y, z);
		}
	}

	void CompactVoxelOctree::Deactivate(int x, int y, int z)
	{
		int sizelength = std::pow(2, m_Levels);
		// Makes sure index is in bounds (otherwise undefined behavior can occur)
		if ((x < sizelength && x >= 0) && (y < sizelength && y >= 0) && (z < sizelength && z >= 0))
		{
			VX_CORE_INFO("[Deactivate Subnode] X: {0} Y {1} Z {2}", x, y, z);

			CompactNode* node = &m_Nodes[0]; // Retrieve the root node
			int size = std::pow(2, m_Levels - 1);

			while (size != 0)
			{
				int index = (x / size) + 2 * (y / size) + 4 * (z / size);
				// Check if subnode at index is a branch or a leaf
				if (node->GetValidMaskBit(index) == 1 && node->GetLeafMaskBit(index) == 0) // This would mean it is a branch
				{
					node = &m_Nodes[(size_t)node->GetIndex() + (size_t)node->GetBranchIndex(index)];
					x -= size * (x / size);
					y -= size * (y / size);
					z -= size * (z / size);
					size /= 2;
				}
				else
				{
					// If it's not a branch we need to check if this is the final subnode and thus supposed leaf
					if (size == 1)
					{
						// Deactivate the subnode if it is the final one
						node->ClearLeafMaskBit(index);
						node->ClearValidMaskBit(index);
						break;
					}
					else
					{
						// Turn the leaf into a branch if it is not the final one

						// Check if a branch index already exists:
						bool branch = node->HasBranch();

						node->ClearLeafMaskBit(index);
						node->SetValidMaskBit(index);
						if (branch)
						{
							// In this case I think it will ruin all other nodes index (solved via a freelist or padding as done below)
							m_Nodes[(size_t)node->GetIndex() + (size_t)node->GetBranchIndex(index)] = 0x0000FFFF;
						}
						else
						{
							node->SetIndex(m_Nodes.size());
							m_Nodes.emplace_back(0x0000FFFF);
							// Add 7 spaces of padding for future branches:
							m_Nodes.insert(m_Nodes.end(), { 0, 0, 0, 0, 0, 0, 0 });
						}
					}
				}
			}
		}
		else
		{
			VX_CORE_CRITICAL("Octree Index {0}, {1}, {2} is out of bounds", x, y, z);
		}
	}

	void CompactVoxelOctree::SetColorIndex(int x, int y, int z, uint32_t colorIndex)
	{
		int sizelength = std::pow(2, m_Levels);
		// Makes sure index is in bounds (otherwise undefined behavior can occur)
		if ((x < sizelength && x >= 0) && (y < sizelength && y >= 0) && (z < sizelength && z >= 0))
		{
			VX_CORE_INFO("[Set Color Index of Subnode] X: {0} Y {1} Z {2}", x, y, z);

			CompactNode* node = &m_Nodes[0]; // Retrieve the root node
			int size = std::pow(2, m_Levels - 1);

			while (size != 0)
			{
				int index = (x / size) + 2 * (y / size) + 4 * (z / size);
				// Check if subnode at index is a branch or a leaf
				if (node->GetValidMaskBit(index) == 1 && node->GetLeafMaskBit(index) == 0) // This would mean it is a branch
				{
					node = &m_Nodes[(size_t)node->GetIndex() + (size_t)node->GetBranchIndex(index)];
					x -= size * (x / size);
					y -= size * (y / size);
					z -= size * (z / size);
					size /= 2;
				}
				else
				{
					// If it's not a branch we need to check if this is the final subnode and thus supposed leaf
					if (size == 1)
					{
						// Set the color index of this subnode if it is the final one
						node->SetColorIndex(index, colorIndex);
						break;
					}
					else
					{
						// Turn the leaf into a branch if it is not the final one

						// Check if a branch index already exists:
						bool branch = node->HasBranch();

						node->ClearLeafMaskBit(index);
						node->SetValidMaskBit(index);
						if (branch)
						{
							// In this case I think it will ruin all other nodes index (solved via a freelist or padding as done below)
							m_Nodes[(size_t)node->GetIndex() + (size_t)node->GetBranchIndex(index)] = 0x0000FFFF;
						}
						else
						{
							node->SetIndex(m_Nodes.size());
							m_Nodes.emplace_back(0x0000FFFF);
							// Add 7 spaces of padding for future branches:
							m_Nodes.insert(m_Nodes.end(), { 0, 0, 0, 0, 0, 0, 0 });
						}
					}
				}
			}
		}
		else
		{
			VX_CORE_CRITICAL("Octree Index {0}, {1}, {2} is out of bounds", x, y, z);
		}
	}

#if 0
	int CompactVoxelOctree::Get(int x, int y, int z)
	{
		CompactNode node = m_Nodes[0];
		uint32_t size = (uint32_t)std::pow(2, m_Levels - 1);

		while (size != 0)
		{
			if (node.GetLeafMaskBit((x / size) + 2 * (y / size) + 4 * (z / size)) == 0)
			{
				size /= 2;
				// This probably dosen't work
				node = m_Nodes[(size_t)node.GetIndex() + (size_t)node.GetBranchIndex((x / size) + 2 * (y / size) + 4 * (z / size))];
				x -= size * (x / size);
				y -= size * (y / size);
				z -= size * (z / size);
			}
			else
			{
				return node.GetLeafMaskBit((x / size) + 2 * (y / size) + 4 * (z / size));
			}
		}
		// Error (size should never be 0)
		//return 0;
	}
#endif

	uint32_t* CompactVoxelOctree::GetData()
	{
		// I'm not 100% if this returns a correct array. Perhaps I need to store a uint32_t array instead of a node array 
		return (uint32_t*)m_Nodes.data();
	}
}