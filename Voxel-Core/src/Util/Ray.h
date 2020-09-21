#pragma once
#include "glm/glm.hpp"

namespace VoxelCore {

	struct RayHit
	{
		bool Collision;
		glm::vec2 t;
		glm::vec3 tmid, tmax;
	};

	struct Ray
	{
		glm::vec3 Origin, Direction;

		Ray(const glm::vec3& origin, const glm::vec3& direction);
		~Ray();
		static RayHit RayAABBCollision(const Ray& ray, const glm::vec3& position, const float scale);
	};

}