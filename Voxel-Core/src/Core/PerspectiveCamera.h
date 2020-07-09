#pragma once
#include "Timestep.h"
#include <glm/glm.hpp>

namespace VoxelCore {

	class PerspectiveCamera
	{
	protected:
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

	public:
		virtual ~PerspectiveCamera() = default;
		virtual void OnMouseMove(float xpos, float ypos) = 0;
		virtual void OnMouseScroll(float xoffset, float yoffset) = 0;
		virtual void HandleInput() = 0;

		virtual void SetFOV(float fov) = 0;
		virtual void SetNearPlane(float nearpane) = 0;
		virtual void SetFarPlane(float farplane) = 0;
		virtual void SetPlans(float nearplane, float farplane) = 0;

		virtual float GetFOV() = 0;
		virtual float GetNearPlane() = 0;
		virtual float GetFarPlane() = 0;

		virtual float GetAspectRatio() = 0;
		virtual glm::mat4& GetViewMatrix() = 0;
		virtual glm::mat4& GetProjectionMatrix() = 0;
		virtual glm::mat4& GetMVPMatrix() = 0;
	};
}