#pragma once

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

	public:
		void ToolHover();
		void ToolLeftClick();

		void SetActiveTool(ToolType tool) { m_ActiveToolType = tool; };
		ToolType GetActiveTool() { return m_ActiveToolType; };
	};

}