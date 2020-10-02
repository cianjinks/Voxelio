#include "voxelpch.h"
#include "CompactVoxelOctree.h"

namespace VoxelCore {
	// NODE
	CompactNode::CompactNode()
		: m_Data(0), m_ColorIndex1(0), m_ColorIndex2(0), m_ColorIndex3(0) {}

	CompactNode::CompactNode(uint32_t data)
		: m_Data(data), m_ColorIndex1(0), m_ColorIndex2(0), m_ColorIndex3(0) {}

	CompactNode::~CompactNode() {}

	void CompactNode::SetData(uint32_t data) { m_Data = data; }

	uint32_t CompactNode::GetData() { return m_Data; }

	void CompactNode::Activate(int index)
	{
		SetLeafMaskBit(index);
		SetValidMaskBit(index);
	}

	void CompactNode::Deactivate(int index)
	{
		ClearLeafMaskBit(index);
		ClearValidMaskBit(index);
	}

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
					return m_ColorIndex1 >> 20;
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
		return -1;
	}

	// OCTREE
	CompactVoxelOctree::CompactVoxelOctree(int levels)
		: m_OctreeLevels(levels)
	{
		GenerateOctree();
		GenerateLevels();
	}

	CompactVoxelOctree::~CompactVoxelOctree()
	{

	}

	void CompactVoxelOctree::GenerateOctree()
	{
		m_Nodes.reserve(MAX_OCTREE_NODES);
		m_Nodes.emplace_back(0x0001FFFF);
		m_Dimension = (int)std::pow(2, m_OctreeLevels);
		for (int x = 0; x < m_Dimension; x++)
		{
			for (int y = 0; y < m_Dimension; y++)
			{
				for (int z = 0; z < m_Dimension; z++)
				{
					Activate(x, y, z);
				}
			}
		}
	}

	// Generates a map of octree resolutions to strings of their dimensions
	void CompactVoxelOctree::GenerateLevels()
	{
		for (int i = 1; i <= MAX_OCTREE_LEVELS; i++)
		{
			int dimension = (int)std::pow(2, i);
			std::string dimString = std::to_string(dimension);
			std::string name = dimString + "x" + dimString + "x" + dimString;
			m_OctreeLevelsMap.insert(std::make_pair(i, name));
		}
	}

	void CompactVoxelOctree::Reload(int levels)
	{
		m_OctreeLevels = levels;
		m_Nodes.clear();
		GenerateOctree();
	}

	void CompactVoxelOctree::Activate(int x, int y, int z)
	{
		int sizelength = (int)std::pow(2, m_OctreeLevels);
		// Makes sure index is in bounds (otherwise undefined behavior can occur)
		if ((x < sizelength && x >= 0) && (y < sizelength && y >= 0) && (z < sizelength && z >= 0))
		{
			//VX_CORE_INFO("[Activate Subnode] X: {0} Y {1} Z {2}", x, y, z);

			CompactNode* node = &m_Nodes[0]; // Retrieve the root node
			int size = (int)std::pow(2, m_OctreeLevels - 1);

			while (size != 0)
			{
				int index = (x / size) + 2 * (y / size) + 4 * (z / size);
				// Check if subnode at index is a branch or a leaf
				if (node->GetValidMaskBit(index) == 1 && node->GetLeafMaskBit(index) == 0) // This would mean it is a branch
				{
					node = &m_Nodes[(size_t)node->GetIndex() + (size_t)index];
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
						// Actiavte the subnode if it is the final one
						node->Activate(index);
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
							m_Nodes[(size_t)node->GetIndex() + (size_t)index] = 0x0000FFFF;
						}
						else
						{
							node->SetIndex((uint32_t)m_Nodes.size());
							m_Nodes.insert(m_Nodes.end(), { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF });
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
		int sizelength = (int)std::pow(2, m_OctreeLevels);
		// Makes sure index is in bounds (otherwise undefined behavior can occur)
		if ((x < sizelength && x >= 0) && (y < sizelength && y >= 0) && (z < sizelength && z >= 0))
		{
			VX_CORE_INFO("[Deactivate Subnode] X: {0} Y {1} Z {2}", x, y, z);

			CompactNode* node = &m_Nodes[0]; // Retrieve the root node
			int size = (int)std::pow(2, m_OctreeLevels - 1);

			while (size != 0)
			{
				int index = (x / size) + 2 * (y / size) + 4 * (z / size);
				// Check if subnode at index is a branch or a leaf
				if (node->GetValidMaskBit(index) == 1 && node->GetLeafMaskBit(index) == 0) // This would mean it is a branch
				{
					node = &m_Nodes[(size_t)node->GetIndex() + (size_t)index];//(size_t)node->GetBranchIndex(index)];
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
						node->Deactivate(index);
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
							m_Nodes[(size_t)node->GetIndex() + (size_t)index] = 0x0000FFFF;
						}
						else
						{
							node->SetIndex((uint32_t)m_Nodes.size());
							m_Nodes.insert(m_Nodes.end(), { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF });
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
		int sizelength = (int)std::pow(2, m_OctreeLevels);
		// Makes sure index is in bounds (otherwise undefined behavior can occur)
		if ((x < sizelength && x >= 0) && (y < sizelength && y >= 0) && (z < sizelength && z >= 0))
		{
			//VX_CORE_INFO("[Set Color Index of Subnode] X: {0} Y {1} Z {2}", x, y, z);

			CompactNode* node = &m_Nodes[0]; // Retrieve the root node
			int size = (int)std::pow(2, m_OctreeLevels - 1);

			while (size != 0)
			{
				int index = (x / size) + 2 * (y / size) + 4 * (z / size);
				// Check if subnode at index is a branch or a leaf
				if (node->GetValidMaskBit(index) == 1 && node->GetLeafMaskBit(index) == 0) // This would mean it is a branch
				{
					node = &m_Nodes[(size_t)node->GetIndex() + (size_t)index];
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
							m_Nodes[(size_t)node->GetIndex() + (size_t)index] = 0x0000FFFF;
						}
						else
						{
							node->SetIndex((uint32_t)m_Nodes.size());
							m_Nodes.insert(m_Nodes.end(), { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF });
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

	// Loosely based on https://www.shadertoy.com/view/3d2XRd
	RayTraceHit CompactVoxelOctree::RayTrace(Ray ray)
	{
		glm::vec3 pos = glm::vec3(0.0f);
		float size = 1.0f;
		bool canPush = true;

		// Initialize stack
		std::stack<OctreeStackElement> OctreeStack;

		RayHit hit = Ray::RayAABBCollision(ray, pos, size);
		float h = hit.t.y;

		if (!hit.Collision)
		{
			return RayTraceHit();
		}

		// Inital push (Obtains the index, position and scale of the first octant voxel upon entering the cube)
		glm::vec3 index = mix(-sign(ray.Direction), sign(ray.Direction), lessThanEqual(hit.tmid, glm::vec3(hit.t.x)));
		size *= 0.5;
		pos += size * index;

		// Retrieve this initial octant voxel
		int childIndex = get2DIndex(index);
		uint32_t parentptr = 0;
		CompactNode* node = &m_Nodes[parentptr]; // Root

		// while(true) for now but we may want some way to make sure it always breaks
		while (true)
		{
			// Run another collision test for new octant
			hit = Ray::RayAABBCollision(ray, pos, size);

			// Check if the voxel exists
			if (node->GetValidMaskBit(childIndex) == 1)
			{
				// If it is a leaf and exists it means we have hit a voxel
				if (node->GetLeafMaskBit(childIndex) == 1)
				{
					float divisor = (1.0f / m_Dimension) * 2;
					pos += 1.0f;

					glm::ivec3 nodeIndex = glm::ivec3((int)(pos.x / divisor), (int)(pos.y / divisor), (int)(pos.z / divisor));
					glm::ivec3 previousNodeIndex = nodeIndex + glm::ivec3(hit.normal.x, hit.normal.y, hit.normal.z);

					return RayTraceHit(node, childIndex, nodeIndex, previousNodeIndex);
				}
				else if (canPush)
				{
					// PUSH
					if (hit.t.y < h)
					{
						OctreeStack.push(OctreeStackElement(node, pos, size, index, h));
					}
					h = hit.t.y;

					index = mix(-sign(ray.Direction), sign(ray.Direction), lessThanEqual(hit.tmid, glm::vec3(hit.t.x)));
					size *= 0.5;
					pos += size * index;

					// Retrieve next node from the array
					parentptr = node->GetIndex() + childIndex;
					node = &m_Nodes[parentptr];
					childIndex = get2DIndex(index);

					continue;
				}
			}

			// ADVANCE
			glm::vec3 oldIndex = index;
			index = mix(index, sign(ray.Direction), equal(hit.tmax, glm::vec3(hit.t.y)));
			pos += mix(glm::vec3(0), sign(ray.Direction), notEqual(oldIndex, index)) * (2.0f * size);

			childIndex = get2DIndex(index);

			if (index == oldIndex)
			{
				// POP
				if (OctreeStack.size() <= 0)
				{
					// Didn't hit anything along this ray
					return RayTraceHit();
				}

				OctreeStackElement element = OctreeStack.top();
				OctreeStack.pop();
				node = element.node;
				pos = element.position;
				size = element.size;
				index = element.index;
				h = element.h;

				canPush = false;
			}
			else { canPush = true; }

		}
		return RayTraceHit();
	}

#if 0
	int CompactVoxelOctree::Get(int x, int y, int z)
	{
		CompactNode node = m_Nodes[0];
		uint32_t size = (uint32_t)std::pow(2, m_OctreeLevels - 1);

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

	int CompactVoxelOctree::get2DIndex(glm::vec3 index)
	{
		for (int i = 0; i < 3; i++)
		{
			if (index[i] == -1.0f)
			{
				index[i] = 0.0f;
			}
		}
		return int(index.x + (2 * index.y) + (4 * index.z));
	}
}