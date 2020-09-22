#include "voxelpch.h"
#include "Tool.h"

namespace VoxelCore {

	void ToolHandler::ToolHover(CompactVoxelOctree& octree, Ray ray, int currentSelectedColorIndex)
	{
		switch (m_ActiveToolType)
		{
			case ToolType::ERASE:
			{
				RayTraceHit hit = octree.RayTrace(ray);
				// If the mouse has changed nodes
				if ((hit.node != m_LastNodeHit || hit.childIndex != m_LastChildIndex) && (m_LastNodeHit != nullptr))
				{
					// Set color index of previous node back to grey
					m_LastNodeHit->SetColorIndex(m_LastChildIndex, 0);
				}
				// If the new node is valid
				if (hit.node != nullptr)
				{ 
					// Highlight it using the Selection Color
					hit.node->SetColorIndex(hit.childIndex, 1);
				}
				m_LastNodeHit = hit.node;
				m_LastChildIndex = hit.childIndex;
				break;
			}
			case ToolType::BUILD:
			{

				break;
			}
			case ToolType::COLOR:
			{
				RayTraceHit hit = octree.RayTrace(ray);
				// If mouse has changed nodes
				if ((hit.node != m_LastNodeHit || hit.childIndex != m_LastChildIndex) && (m_LastNodeHit != nullptr))
				{
					// If the user has not clicked to color the previous node
					if (!m_LeftClickColored)
					{
						// Set it back to the grey color
						m_LastNodeHit->SetColorIndex(m_LastChildIndex, 0);
					}
					// For the new node, set this to false
					m_LeftClickColored = false;
				}

				// If the new node is valid
				if (hit.node != nullptr)
				{
					// Hightlight it using the selection color
					hit.node->SetColorIndex(hit.childIndex, currentSelectedColorIndex);
				}
				m_LastNodeHit = hit.node;
				m_LastChildIndex = hit.childIndex;
				break;
			}
		}
	}

	void ToolHandler::ToolLeftClick(CompactVoxelOctree& octree, Ray ray, int currentSelectedColorIndex)
	{
		switch (m_ActiveToolType)
		{
			case ToolType::ERASE:
			{
				RayTraceHit hit = octree.RayTrace(ray);
				// If the current node is valid
				if (hit.node != nullptr && hit.childIndex >= 0)
				{
					// We deactivate it (This never resets the color)
					hit.node->ClearValidMaskBit(hit.childIndex);
					hit.node->ClearLeafMaskBit(hit.childIndex);
				}
				break;
			}
			case ToolType::BUILD:
			{

				break;
			}
			case ToolType::COLOR:
			{
				RayTraceHit hit = octree.RayTrace(ray);
				// If the current node is valid
				if (hit.node != nullptr && hit.childIndex >= 0)
				{
					// We color it the currently selected palette color
					hit.node->SetColorIndex(hit.childIndex, currentSelectedColorIndex);
				}
				// Make sure the hover function knows it has been colored
				m_LeftClickColored = true;
				break;
			}
		}
	}
}