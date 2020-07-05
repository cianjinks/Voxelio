#include "Timestep.h"

#include <GLFW/glfw3.h>

namespace VoxelCore {

	float Timestep::s_DeltaTime = 0.0f;
	float Timestep::s_LastFrame = 0.0f;

	void Timestep::Update()
	{
			float currentFrame = (float)glfwGetTime();
			s_DeltaTime = currentFrame - s_LastFrame;
			s_LastFrame = currentFrame;
	}
}