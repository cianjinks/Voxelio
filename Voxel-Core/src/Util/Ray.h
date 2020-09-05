#pragma once
#include "glm/glm.hpp"

namespace VoxelCore {

	struct Ray
	{
		glm::vec3 Origin, Direction;

		Ray(const glm::vec3& origin, const glm::vec3& direction);
		~Ray();
		static bool RayAABBCollision(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax);
	};
}