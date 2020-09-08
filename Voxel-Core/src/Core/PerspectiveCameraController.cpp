#include "voxelpch.h"
#include "PerspectiveCameraController.h"
#include "Input.h"

namespace VoxelCore {

	PerspectiveCameraController::PerspectiveCameraController(float windowWidth, float windowHeight)
		: m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), m_MouseX(m_WindowWidth / 2), m_MouseY(m_WindowHeight / 2) {}

	PerspectiveCameraController::~PerspectiveCameraController() {}

	void PerspectiveCameraController::OnMouseScroll(float xoffset, float yoffset) {};
	void PerspectiveCameraController::OnMouseMove(float xpos, float ypos)
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

			const float sensitivity = 0.05f;
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

			glm::vec3 direction;
			direction.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
			direction.y = sin(glm::radians(m_CameraPitch));
			direction.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
			m_CameraFront = glm::normalize(direction);
		}
		CalculateMatrices();
	}

	void PerspectiveCameraController::HandleInput()
	{
		if (Input::IsKeyPressed(GLFW_KEY_W)) {
			m_CameraPos += m_CameraSpeed * m_CameraFront;
		}
		if (Input::IsKeyPressed(GLFW_KEY_S)) {
			m_CameraPos -= m_CameraSpeed * m_CameraFront;
		}
		if (Input::IsKeyPressed(GLFW_KEY_A)) {
			m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
		}
		if (Input::IsKeyPressed(GLFW_KEY_D)) {
			m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
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

	void PerspectiveCameraController::Resize(float width, float height)
	{
		m_WindowWidth = width;
		m_WindowHeight = height;
	}

	void PerspectiveCameraController::CalculateMatrices()
	{
		m_ViewMatrix = glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_CameraFOV), GetAspectRatio(), m_NearPlane, m_FarPlane);
		m_MVPMatrix = m_ProjectionMatrix * m_ViewMatrix * glm::mat4(1.0f);
	}
}