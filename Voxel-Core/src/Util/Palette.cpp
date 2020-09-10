#include "voxelpch.h"
#include "Palette.h"

namespace VoxelCore {

	std::vector<VoxelColor>& VoxelColorPalette::GetColors()
	{
		return m_Colors;
	}

	std::vector<float>& VoxelColorPalette::GetColorData()
	{
		return m_ColorData;
	}

	void VoxelColorPalette::AddColor(VoxelColor color)
	{
		m_Colors.emplace_back(color);
		m_ColorData.emplace_back(color.color.r);
		m_ColorData.emplace_back(color.color.g);
		m_ColorData.emplace_back(color.color.b);
		m_ColorData.emplace_back(color.color.a);
	}
}