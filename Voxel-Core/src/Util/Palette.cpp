#include "voxelpch.h"
#include "Palette.h"

namespace VoxelCore {

	VoxelColorPalette::VoxelColorPalette()
	{
		m_ColorData.reserve(MAX_PALETTE_SIZE);

		// Add Default Color and Selection Color
		AddColor(m_DefaultColor);
		AddColor(m_SelectionColor);
	}

	void VoxelColorPalette::Replace(size_t size)
	{
		m_ColorData.clear();
		m_ColorData.resize(size);
	}

	void VoxelColorPalette::Reload()
	{
		m_ColorData.clear();
		AddColor(m_DefaultColor);
		AddColor(m_SelectionColor);
	}

	std::vector<glm::vec4>& VoxelColorPalette::GetColors()
	{
		return m_ColorData;
	}

	float* VoxelColorPalette::GetColorData()
	{
		return (float*)m_ColorData.data();
	}

	bool VoxelColorPalette::AddColor(glm::vec4 color)
	{
		if (m_ColorData.size() < MAX_PALETTE_SIZE)
		{
			// Test color name dosent already exist (This would cause problems for ImGui)
			//int count = -1;
			//for (auto& c : m_Colors)
			//{
			//	if (c.name.substr(0, color.name.length()) == color.name)
			//	{
			//		count++;
			//	}
			//}
			//if (count != -1) { color.name += std::to_string(count); }

			m_ColorData.emplace_back(color);
			return true;
		}
		else
		{
			return false;
		}
	}
}