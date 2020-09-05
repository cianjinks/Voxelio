#pragma once
#include <glm/glm.hpp>

namespace VoxelCore {

	struct VoxelColor
	{
		glm::vec4 color;
		std::string name;
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