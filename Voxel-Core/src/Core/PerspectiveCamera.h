#pragma once
#include "Timestep.h"
#include <glm/glm.hpp>

namespace VoxelCore {

	class PerspectiveCamera
	{
	public:
		virtual ~PerspectiveCamera() = default;
		virtual void OnMouseMove(float xpos, float ypos) = 0;
		virtual void OnMouseScroll(float xoffset, float yoffset) = 0;
		virtual void HandleInput() = 0;

		virtual void Resize(float width, float height) = 0;

		virtual void SetFOV(float fov) = 0;
		virtual void SetNearPlane(float nearpane) = 0;
		virtual void SetFarPlane(float farplane) = 0;
		virtual void SetPlans(float nearplane, float farplane) = 0;

		virtual float GetFOV() = 0;
		virtual float GetNearPlane() = 0;
		virtual float GetFarPlane() = 0;

		virtual glm::vec3& GetCameraPos() = 0;

		virtual float GetAspectRatio() = 0;
		virtual glm::mat4& GetViewMatrix() = 0;
		virtual glm::mat4& GetProjectionMatrix() = 0;
		virtual glm::mat4& GetMVPMatrix() = 0;
	};
}