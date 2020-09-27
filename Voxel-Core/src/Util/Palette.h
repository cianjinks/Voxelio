#pragma once
#include <glm/glm.hpp>

namespace VoxelCore {

	class VoxelColorPalette
	{
	private:
		std::vector<glm::vec4> m_ColorData;

	public:
		glm::vec4 m_DefaultColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
		glm::vec4 m_SelectionColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	private:

	public:
		// For now this just wraps a vector but later it should contain some kind of ImGUI state for the color palette editor
		VoxelColorPalette();
		void Replace(size_t size);
		void Reload();

		std::vector<glm::vec4>& GetColors();
		float* GetColorData();
		bool AddColor(glm::vec4 color);

		static constexpr int MAX_PALETTE_SIZE = 4096;
	};
}