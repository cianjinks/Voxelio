#include "voxelpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace VoxelCore {

	OrthographicCamera::OrthographicCamera(float windowWidth, float windowHeight)
		: m_AspectRatio(m_WindowWidth / m_WindowHeight), m_ViewMatrix(glm::mat4(1.0f)), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight)
	{
		CalculateMatrices();
	}

	OrthographicCamera::~OrthographicCamera() {}

	void OrthographicCamera::Resize(float windowWidth, float windowHeight)
	{
		m_WindowWidth = windowWidth;
		m_WindowHeight = windowHeight;
		m_AspectRatio = m_WindowWidth / windowHeight;
		CalculateMatrices();
	}

	void OrthographicCamera::CalculateMatrices()
	{
		m_ProjectionMatrix = glm::ortho(-m_AspectRatio, m_AspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
		//m_ProjectionMatrix = glm::ortho(-(m_AspectRatio / 2), (m_AspectRatio / 2), -(m_AspectRatio / 2), (m_AspectRatio / 2), -1.0f, 1.0f);
		m_MVPMatrix = m_ProjectionMatrix * m_ViewMatrix * glm::mat4(1.0f);
	}
}
