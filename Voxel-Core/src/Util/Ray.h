#pragma once
#include "glm/glm.hpp"

namespace VoxelCore {

	struct RayHit
	{
		bool Collision;
		glm::vec2 t;
		glm::vec3 tmid, tmax;
		glm::vec3 normal;
	};

	struct Ray
	{
		glm::vec3 Origin, Direction;

		Ray(const glm::vec3& origin, const glm::vec3& direction);
		~Ray();

		static RayHit RayAABBCollision(Ray& ray, glm::vec3& position, float scale);
		// Utility functions:
		static glm::vec3 mix(glm::vec3& a, glm::vec3& b, glm::bvec3& t);
		static glm::bvec3 lessThanEqual(glm::vec3& a, glm::vec3& b);
		static glm::bvec3 equal(glm::vec3& a, glm::vec3& b);
		static glm::bvec3 notEqual(glm::vec3& a, glm::vec3& b);
		static glm::vec3 sign(glm::vec3& value);
		static glm::vec3 step(glm::vec3& edge, glm::vec3& x);
	};

}