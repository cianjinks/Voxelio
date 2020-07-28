#include "voxelpch.h"
#include "CompactVoxelOctree.h"

// NODE
CompactNode::CompactNode()
	: m_Data(0) {}

CompactNode::~CompactNode() {}

void CompactNode::SetData(uint32_t data) { m_Data = data; }

uint32_t CompactNode::GetData() { return m_Data; }

void CompactNode::SetValidMaskBit(int index)
{
	uint32_t bit = 1 << (15 - index);
	m_Data |= bit;
}

void CompactNode::ClearValidMaskBit(int index)
{
	uint32_t bit = 1 << (15 - index);
	m_Data &= ~bit;
}

int CompactNode::GetValidMaskBit(int index)
{
	uint32_t clearmask = 0xFFFFFFFE;
	uint32_t bit = m_Data >> (15 - (index + 1));
	return bit & ~clearmask;
}

void CompactNode::SetLeafMaskBit(int index)
{
	uint32_t bit = 1 << (7 - index);
	m_Data |= bit;
}
void CompactNode::ClearLeafMaskBit(int index)
{
	uint32_t bit = 1 << (7 - index);
	m_Data &= ~bit;
}

int CompactNode::GetLeafMaskBit(int index)
{
	uint32_t clearmask = 0xFFFFFFFE;
	uint32_t bit = m_Data >> (7 - index);
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
	m_Data |= index;
}

uint32_t CompactNode::GetIndex()
{
	uint32_t clearmask = 0x0000FFFF;
	return (m_Data & ~clearmask) >> 16;
}

// OCTREE
CompactVoxelOctree::CompactVoxelOctree(int levels)
	: m_NodeCount(1), m_Levels(levels)
{
	const int count = std::pow(8, m_Levels);
	m_Nodes = new CompactNode[count + 1];

	/**uint32_t array[] = { 129526, 197373, 312318, 1791, 329726, 1023 };
	for (int i = 0; i < 6; i++)
	{
		m_Nodes[i].SetData(array[i]);
	}**/
	m_Nodes[0].SetData(0x0001FFFFFFFF);
}

CompactVoxelOctree::~CompactVoxelOctree()
{
	delete[] m_Nodes;
}

#if 0
void CompactVoxelOctree::Activate(int x, int y, int z)
{
	CompactNode node = m_Nodes[0];
	uint32_t size = (uint32_t)std::pow(2, 3);

	// We build a stack of nodes along the path that we edited via this function so that we can try simplifying it at the end
	// This also results in it filling in any news nodes required along that path
	int stack_ptr = 0;
	uint32_t* simplify_stack = new uint32_t[3];

	while (size != 0)
	{
		//if (node->GetNodeType() == NodeType::BRANCH)
		if (node.GetLeafMaskBit(x + 2 * y + 4 * z) == 0)
		{
			simplify_stack[stack_ptr++] = node;
			size /= 2;
			int localx = x / size;
			int localy = y / size;
			int localz = z / size;
			node = node->GetBranchChild(localx, localy, localz);
			x -= size * localx;
			y -= size * localy;
			z -= size * localz;
		}
		else {
			if (size == 1)
			{
				node->SetLeafData(data);
				break;
			}
			else {
				node->CollapseLeaf();
			}
		}
	}

	//simplify_stack[stack_ptr++] = node;

	// Traverses the path we made a change within to see if any simplification can be done
	while (stack_ptr != 0)
	{
		node = simplify_stack[--stack_ptr];
		if (node->GetNodeType() == NodeType::BRANCH)
		{
			node->SimplifyBranch();
		}
	}

	delete[] simplify_stack;
}
#endif

#if 1
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
			node = m_Nodes[node.GetIndex() + node.GetBranchIndex((x / size) + 2 * (y / size) + 4 * (z / size))];
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
	return (uint32_t*)m_Nodes;
}