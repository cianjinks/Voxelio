#pragma once
#include "VoxelCore.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PerspectiveCameraController : public VoxelCore::PerspectiveCamera
{
private:
	float m_WindowWidth, m_WindowHeight;
	float m_MouseX, m_MouseY;
	bool m_InitialMouseMovement = true;

private:
	void CalculateMatrices();

public:
	PerspectiveCameraController(float windowWidth, float windowHeight);
	virtual ~PerspectiveCameraController();

	void OnMouseMove(float xpos, float ypos) override;
	void HandleInput() override;

	void SetFOV(float fov) override { m_CameraFOV = fov; };
	void SetNearPlane(float nearplane) override { m_NearPlane = nearplane; };
	void SetFarPlane(float farplane) override { m_FarPlane = farplane; };
	void SetPlans(float nearplane, float farplane) override { m_NearPlane = nearplane; m_FarPlane = farplane; };

	float GetFOV() override { return m_CameraFOV; };
	float GetNearPlane() override { return m_NearPlane; };
	float GetFarPlane() override { return m_FarPlane; };

	const glm::mat4& GetViewMatrix() const override { return m_ViewMatrix; };
	const glm::mat4& GetProjectionMatrix() const override { return m_ProjectionMatrix; };
	float GetAspectRatio() override { return m_WindowWidth / m_WindowHeight; };
};