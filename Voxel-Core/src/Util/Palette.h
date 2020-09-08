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
	public:
		// For now this just wraps a vector but later it should contain some kind of ImGUI state for the color palette editor
		std::vector<VoxelColor>& GetColors();
	};
}