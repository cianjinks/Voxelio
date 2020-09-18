#pragma once
#include "PerspectiveCamera.h"

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VoxelCore {

	class PerspectiveCameraController : public PerspectiveCamera
	{
	private:
		float m_WindowWidth, m_WindowHeight;
		float m_MouseX, m_MouseY;
		bool m_InitialMouseMovement = true;

		glm::vec3 m_CameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float m_CameraYaw = -90.0f;
		float m_CameraPitch = 0.0f;
		float m_CameraFOV = 60.0f;
		float m_CameraSpeed = 2.5f * Timestep::GetDeltaTime();

		float m_NearPlane = 0.1f;
		float m_FarPlane = 100.0f;

		// Matrices
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_MVPMatrix = glm::mat4(1.0f);

	private:
		void CalculateMatrices();

	public:
		PerspectiveCameraController(float windowWidth, float windowHeight);
		virtual ~PerspectiveCameraController();

		void OnMouseMove(float xpos, float ypos) override;
		void OnMouseScroll(float xoffset, float yoffset) override;
		void HandleInput() override;

		void Resize(float width, float height) override;

		void SetFOV(float fov) override { m_CameraFOV = fov; };
		void SetNearPlane(float nearplane) override { m_NearPlane = nearplane; };
		void SetFarPlane(float farplane) override { m_FarPlane = farplane; };
		void SetPlanes(float nearplane, float farplane) override { m_NearPlane = nearplane; m_FarPlane = farplane; };

		float GetFOV() override { return m_CameraFOV; };
		float GetNearPlane() override { return m_NearPlane; };
		float GetFarPlane() override { return m_FarPlane; };

		glm::vec3& GetCameraPos() override { return m_CameraPos; };

		glm::mat4& GetViewMatrix() override { return m_ViewMatrix; };
		glm::mat4& GetProjectionMatrix() override { return m_ProjectionMatrix; };
		glm::mat4& GetMVPMatrix() override { return m_MVPMatrix; };
		float GetAspectRatio() override { return m_WindowWidth / m_WindowHeight; };
	};
}