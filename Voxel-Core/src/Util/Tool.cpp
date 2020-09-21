#include "voxelpch.h"
#include "Tool.h"

namespace VoxelCore {

	void ToolHandler::ToolHover(CompactVoxelOctree& octree, Ray ray)
	{
		switch (m_ActiveToolType)
		{
			case ToolType::ERASE:
			{
				octree.RayTrace(ray);
			}
			case ToolType::BUILD:
			{

			}
			case ToolType::COLOR:
			{

			}
		}
	}

	void ToolHandler::ToolLeftClick()
	{
		switch (m_ActiveToolType)
		{
			case ToolType::ERASE:
			{

			}
			case ToolType::BUILD:
			{

			}
			case ToolType::COLOR:
			{

			}
		}
	}
}