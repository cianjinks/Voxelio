#include "voxelpch.h"
#include "Tool.h"

namespace VoxelCore {

	// This is kinda digusting :/

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
				m_LastNodeIndex = hit.nodeIndex;
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
			case ToolType::LARGE_ERASE: 
			{
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
					// Highlight it using the highlight color
					hit.node->SetColorIndex(hit.childIndex, 1);
				}
				m_LastNodeHit = hit.node;
				m_LastChildIndex = hit.childIndex;
				break;
			}
		}
	}

	void ToolHandler::ToolLeftClick(CompactVoxelOctree& octree, Ray ray, int currentSelectedColorIndex, glm::ivec3& currentVoxel)
	{
		switch (m_ActiveToolType)
		{
			case ToolType::ERASE:
			{
				RayTraceHit hit = octree.RayTrace(ray);
				currentVoxel = hit.nodeIndex;
				// If the current node is valid
				if (hit.node != nullptr && hit.childIndex >= 0)
				{
					// We deactivate it (This never resets the color)
					if (octree.m_VoxelStatusList[m_LastNodeIndex.x][m_LastNodeIndex.y][m_LastNodeIndex.z])
					{
						hit.node->ClearValidMaskBit(hit.childIndex);
						hit.node->ClearLeafMaskBit(hit.childIndex);
						octree.m_VoxelStatusList[m_LastNodeIndex.x][m_LastNodeIndex.y][m_LastNodeIndex.z] = false;
					}
				}
				break;
			}
			case ToolType::BUILD:
			{
				currentVoxel = m_LastNodeIndex;
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
				currentVoxel = hit.nodeIndex;
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
			case ToolType::LARGE_ERASE:
			{
				RayTraceHit hit = octree.RayTrace(ray);
				if (m_IsFirstNode)
				{
					currentVoxel = hit.nodeIndex;
					// If the current node is valid
					if (hit.node != nullptr && hit.childIndex >= 0)
					{
						// We highlight it using the highlight color
						m_FirstNodeColorIndex = m_LastNodeColorIndex;
						hit.node->SetColorIndex(hit.childIndex, 1);
					}
					// Make sure the hover function knows it has been colored
					m_LeftClickColored = true;

					m_LastNodeHit = hit.node;
					m_LastChildIndex = hit.childIndex;
					m_LastNodeColorIndex = hit.node->GetColorIndex(hit.childIndex);

					m_FirstNode = hit.node;
					m_FirstNodeChildIndex = hit.childIndex;
					m_FirstNodeIndex = hit.nodeIndex;
					m_IsFirstNode = false;
				}
				else
				{
					glm::ivec3 secondNodeIndex = hit.nodeIndex;
					// Erase all blocks between first and second node
					glm::ivec3 nodeDifference = abs(secondNodeIndex - m_FirstNodeIndex);
					for (int x = 0; x <= nodeDifference.x; x++)
					{
						for (int y = 0; y <= nodeDifference.y; y++)
						{
							for (int z = 0; z <= nodeDifference.z; z++)
							{
								// Calculate block to erase
								glm::ivec3 eraseBlock;
								if (m_FirstNodeIndex.x <= secondNodeIndex.x) { eraseBlock.x = m_FirstNodeIndex.x + x; }
								else { eraseBlock.x = secondNodeIndex.x + x; }
								if (m_FirstNodeIndex.y <= secondNodeIndex.y) { eraseBlock.y = m_FirstNodeIndex.y + y; }
								else { eraseBlock.y = secondNodeIndex.y + y; }
								if (m_FirstNodeIndex.z <= secondNodeIndex.z) { eraseBlock.z = m_FirstNodeIndex.z + z; }
								else { eraseBlock.z = secondNodeIndex.z + z; }
								octree.Deactivate(eraseBlock.x, eraseBlock.y, eraseBlock.z);
							}
						}
					}

					// Reset color of first node
					m_FirstNode->SetColorIndex(m_FirstNodeChildIndex, m_FirstNodeColorIndex);

					// Reset first node flag
					m_IsFirstNode = true;
				}
				break;
			}
		}
	}
}