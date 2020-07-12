#include "voxelpch.h"
#include "OrbitalCameraController.h"
#include "Input.h"

namespace VoxelCore {

	OrbitalCameraController::OrbitalCameraController(float windowWidth, float windowHeight, float radius)
		: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_MouseX(m_WindowWidth / 2), m_MouseY(m_WindowHeight / 2), 
		  m_CameraRadius(radius), m_CameraPos(glm::vec3(0.0f, 0.0f, radius)) {}

	OrbitalCameraController::~OrbitalCameraController() {}

	void OrbitalCameraController::OnMouseScroll(float xoffset, float yoffset) 
	{
		const float sensitivity = 0.35f;
		yoffset *= sensitivity;
		
		VX_CORE_INFO("Y Offset {:.3f}", yoffset);

		m_CameraRadius -= yoffset;
		CalculateCameraPos();
	};

	void OrbitalCameraController::OnMouseMove(float xpos, float ypos)
	{
		if (VoxelCore::Input::GetInputMode(GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			if (m_InitialMouseMovement)
			{
				m_MouseX = xpos;
				m_MouseY = ypos;
				m_InitialMouseMovement = false;
			}

			float xoffset = xpos - m_MouseX;
			float yoffset = m_MouseY - ypos; // Y coordinate is top to bottom in glfw
			m_MouseX = xpos;
			m_MouseY = ypos;

			const float sensitivity = 0.15f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			m_CameraYaw += xoffset;
			m_CameraPitch += yoffset;

			if (m_CameraPitch > 89.0f) {
				m_CameraPitch = 89.0f;
			}
			if (m_CameraPitch < -89.0f) {
				m_CameraPitch = -89.0f;
			}
			CalculateCameraPos();
		}
		else
		{
			m_InitialMouseMovement = true;
		}
		// Unecessary as it does it every frame via HandleInput()
		// CalculateMatrices();
	}

	void OrbitalCameraController::HandleInput()
	{
		if (Input::IsKeyPressed(GLFW_KEY_W)) {
			// Dosen't work correctly
			m_CameraPos += m_CameraUp * m_CameraSpeed;
			m_CameraPivot += m_CameraUp * m_CameraSpeed;
		}
		if (Input::IsKeyPressed(GLFW_KEY_S)) {
			// Dosen't work correctly
			m_CameraPos -= m_CameraUp * m_CameraSpeed;
			m_CameraPivot -= m_CameraUp * m_CameraSpeed;
		}
		if (Input::IsKeyPressed(GLFW_KEY_A)) {
			m_CameraPos -= glm::normalize(glm::cross(m_CameraPivot - m_CameraPos, m_CameraUp)) * m_CameraSpeed;
			m_CameraPivot -= glm::normalize(glm::cross(m_CameraPivot - m_CameraPos, m_CameraUp)) * m_CameraSpeed;
		}
		if (Input::IsKeyPressed(GLFW_KEY_D)) {
			m_CameraPos += glm::normalize(glm::cross(m_CameraPivot - m_CameraPos, m_CameraUp)) * m_CameraSpeed;
			m_CameraPivot += glm::normalize(glm::cross(m_CameraPivot - m_CameraPos, m_CameraUp)) * m_CameraSpeed;
		}

		if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		{
			m_CameraSpeed = 5.0f * Timestep::GetDeltaTime();
		}
		else
		{
			m_CameraSpeed = 2.5f * Timestep::GetDeltaTime();
		}
		CalculateMatrices();
	}

	void OrbitalCameraController::CalculateMatrices()
	{
		m_ViewMatrix = glm::lookAt(m_CameraPos, m_CameraPivot, m_CameraUp);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_CameraFOV), GetAspectRatio(), m_NearPlane, m_FarPlane);
		m_MVPMatrix = m_ProjectionMatrix * m_ViewMatrix * glm::mat4(1.0f);
	}

	void OrbitalCameraController::CalculateCameraPos()
	{
		m_CameraPos.x = -m_CameraRadius * cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
		m_CameraPos.y = -m_CameraRadius * sin(glm::radians(m_CameraPitch));
		m_CameraPos.z = -m_CameraRadius * sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
		m_CameraPos += m_CameraPivot;
	}
}
