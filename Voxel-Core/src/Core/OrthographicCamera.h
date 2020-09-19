#pragma once
#include <glm/glm.hpp>

namespace VoxelCore {

	class OrthographicCamera
	{
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_MVPMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_WindowWidth, m_WindowHeight;
		float m_AspectRatio;

	private:
		void CalculateMatrices();

	public:
		OrthographicCamera(float windowWidth, float windowHeight);
		~OrthographicCamera();

		void Resize(float windowWidth, float windowHeight);

		glm::vec3& GetCameraPos() { return m_Position; };
		glm::mat4& GetViewMatrix() { return m_ViewMatrix; };
		glm::mat4& GetProjectionMatrix() { return m_ProjectionMatrix; };
		glm::mat4& GetMVPMatrix() { return m_MVPMatrix; };
		float GetAspectRatio() { return m_AspectRatio; };

	};
}