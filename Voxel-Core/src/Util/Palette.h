#pragma once
#include <glm/glm.hpp>

namespace VoxelCore {

	struct VoxelColor
	{
		std::string name;
		glm::vec4 color;

		VoxelColor(const std::string& name, float r, float g, float b, float a)
			: color(r, g, b, a), name(name) {};
	};

	class VoxelColorPalette
	{
	private:
		std::vector<VoxelColor> m_Colors;
		std::vector<float> m_ColorData;

	public:
		VoxelColor m_DefaultColor = VoxelColor("Default Color", 0.75f, 0.75f, 0.75f, 1.0f);
		VoxelColor m_SelectionColor = VoxelColor("Selection Color", 1.0f, 1.0f, 0.0f, 1.0f);

	private:

	public:
		// For now this just wraps a vector but later it should contain some kind of ImGUI state for the color palette editor
		VoxelColorPalette();
		std::vector<VoxelColor>& GetColors();
		std::vector<float>& GetColorData();
		bool AddColor(VoxelColor color);

		static constexpr int MAX_PALETTE_SIZE = 4096;
	};
}