#pragma once
#include "Core/Renderer/Octree/CompactVoxelOctree.h"
#include "Util/Ray.h"

namespace VoxelCore {

	// I've opted to use this enum instead of a hierarchical class structure for now.
	// However its probably best to create such a structure with static methods.
	enum class ToolType
	{
		ERASE, BUILD, COLOR
	};

	class ToolHandler
	{
	private:
		ToolType m_ActiveToolType = ToolType::ERASE;

		CompactNode* m_LastNodeHit = nullptr;
		int m_LastNodePointer = -1;
		int m_LastChildIndex = -1;
		bool m_LeftClickColored = false;
		int m_LastNodeColorIndex = 0;

	public:
		void ToolHover(CompactVoxelOctree& octree, Ray ray, int currentSelectedColorIndex);
		void ToolLeftClick(CompactVoxelOctree& octree, Ray ray, int currentSelectedColorIndex);

		void SetActiveTool(ToolType tool) { m_ActiveToolType = tool; };
		ToolType GetActiveTool() { return m_ActiveToolType; };
	};

}