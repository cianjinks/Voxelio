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
				if ((hit.node != m_LastNodeHit || (hit.node == m_LastNodeHit && hit.childIndex != m_LastChildIndex)))
				{
					// Set color index of previous node back to its original color
					if (m_LastNodeHit != nullptr)
					{
						m_LastNodeHit->SetColorIndex(m_LastChildIndex, m_LastNodeColorIndex);
					}
					// Save the color of this new node
					m_LastNodeColorIndex = hit.node->GetColorIndex(hit.childIndex);
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
				RayTraceHit hit = octree.RayTrace(ray);
				// If the mouse has changed nodes
				if ((hit.node != m_LastNodeHit || (hit.node == m_LastNodeHit && hit.childIndex != m_LastChildIndex)))
				{
					// Set color index of previous node back to its original color
					if (m_LastNodeHit != nullptr)
					{
						m_LastNodeHit->SetColorIndex(m_LastChildIndex, m_LastNodeColorIndex);
					}
					// Save the color of this new node
					m_LastNodeColorIndex = hit.node->GetColorIndex(hit.childIndex);
				}
				// If the new node is valid
				if (hit.node != nullptr)
				{
					// Highlight it using the Selection Color
					hit.node->SetColorIndex(hit.childIndex, 1);
				}
				m_LastNodeHit = hit.node;
				m_LastChildIndex = hit.childIndex;
				m_LastNodeIndex = hit.nodeIndex;
				m_LastPreviousNodeIndex = hit.previousNodeIndex;
				break;
			}
			case ToolType::COLOR:
			{
				if (m_LastNodeHit != nullptr)
				{
					VX_CORE_INFO("LastNodeHit: {}, LastChild: {}, LastColor: {}", m_LastNodeHit->GetData(), m_LastChildIndex, m_LastNodeColorIndex);
				}
				RayTraceHit hit = octree.RayTrace(ray);
				// If the mouse has changed nodes
				if ((hit.node != m_LastNodeHit || (hit.node == m_LastNodeHit && hit.childIndex != m_LastChildIndex)))
				{
					// Set color index of previous node back to its original color
					if (m_LastNodeHit != nullptr && !m_LeftClickColored)
					{
						m_LastNodeHit->SetColorIndex(m_LastChildIndex, m_LastNodeColorIndex);
					}
					// Save the color of this new node
					m_LastNodeColorIndex = hit.node->GetColorIndex(hit.childIndex);
					// Reset bool to say we haven't colored this voxel yet
					m_LeftClickColored = false;
				}
				// If the new node is valid
				if (hit.node != nullptr)
				{
					// Highlight it using the currently selected color 
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
				if (m_LastNodeHit != nullptr)
				{
					// Make sure the index of the voxel we wish to place is valid (this is actually already done by CompactVoxelOctree::Activate
					//if ((m_LastPreviousNodeIndex.x < octree.m_Dimension && m_LastPreviousNodeIndex.x >= 0) 
					//	&& (m_LastPreviousNodeIndex.y < octree.m_Dimension && m_LastPreviousNodeIndex.y >= 0) 
					//	&& (m_LastPreviousNodeIndex.z < octree.m_Dimension && m_LastPreviousNodeIndex.z >= 0))
					{
						octree.Activate(m_LastPreviousNodeIndex.x, m_LastPreviousNodeIndex.y, m_LastPreviousNodeIndex.z);
						octree.SetColorIndex(m_LastPreviousNodeIndex.x, m_LastPreviousNodeIndex.y, m_LastPreviousNodeIndex.z, 0);
					}
				}
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

				m_LastNodeHit = hit.node;
				m_LastChildIndex = hit.childIndex;
				m_LastNodeColorIndex = hit.node->GetColorIndex(hit.childIndex);
				break;
			}
		}
	}
}