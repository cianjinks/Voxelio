#pragma once
#include "PerspectiveCamera.h"

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VoxelCore {

	class OrbitalCameraController : public PerspectiveCamera
	{
	private:
		float m_WindowWidth, m_WindowHeight;
		float m_MouseX, m_MouseY;
		bool m_InitialMouseMovement = true;

		float m_CameraRadius = 5.0f;
		glm::vec3 m_CameraPivot = glm::vec3(0.0f, 0.0f, 0.0f);

	private:
		void CalculateMatrices();
		void CalculateCameraPos();

	public:
		OrbitalCameraController(float windowWidth, float windowHeight);
		virtual ~OrbitalCameraController();

		void OnMouseMove(float xpos, float ypos) override;
		void OnMouseScroll(float xoffset, float yoffset) override;
		void HandleInput() override;

		void SetFOV(float fov) override { m_CameraFOV = fov; };
		void SetNearPlane(float nearplane) override { m_NearPlane = nearplane; };
		void SetFarPlane(float farplane) override { m_FarPlane = farplane; };
		void SetPlans(float nearplane, float farplane) override { m_NearPlane = nearplane; m_FarPlane = farplane; };

		float GetFOV() override { return m_CameraFOV; };
		float GetNearPlane() override { return m_NearPlane; };
		float GetFarPlane() override { return m_FarPlane; };

		glm::mat4& GetViewMatrix() override { return m_ViewMatrix; };
		glm::mat4& GetProjectionMatrix() override { return m_ProjectionMatrix; };
		glm::mat4& GetMVPMatrix() override { return m_MVPMatrix; };
		float GetAspectRatio() override { return m_WindowWidth / m_WindowHeight; };
	};
}