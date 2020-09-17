#include "voxelpch.h"
#include "Palette.h"

namespace VoxelCore {

	VoxelColorPalette::VoxelColorPalette()
	{
		m_Colors.reserve(MAX_PALETTE_SIZE);
		m_ColorData.reserve(MAX_PALETTE_SIZE * 4);
	}

	std::vector<VoxelColor>& VoxelColorPalette::GetColors()
	{
		return m_Colors;
	}

	std::vector<float>& VoxelColorPalette::GetColorData()
	{
		return m_ColorData;
	}

	bool VoxelColorPalette::AddColor(VoxelColor color)
	{
		if (m_Colors.size() < MAX_PALETTE_SIZE)
		{
			// Test color name dosent already exist (This would cause problems for ImGui)
			int count = -1;
			for (auto& c : m_Colors)
			{
				if (c.name.substr(0, color.name.length()) == color.name)
				{
					count++;
				}
			}
			if (count != -1) { color.name += std::to_string(count); }

			m_Colors.emplace_back(color);
			m_ColorData.emplace_back(color.color.r);
			m_ColorData.emplace_back(color.color.g);
			m_ColorData.emplace_back(color.color.b);
			m_ColorData.emplace_back(color.color.a);
			return true;
		}
		else
		{
			return false;
		}
	}
}