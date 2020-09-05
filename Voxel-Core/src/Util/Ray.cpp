#include "voxelpch.h"
#include "Ray.h"

namespace VoxelCore {

	Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
		: Origin(origin), Direction(direction) {}

	Ray::~Ray() {}

	// Based on http://jcgt.org/published/0007/03/04/ efficient slab test
	bool Ray::RayAABBCollision(const Ray& ray, const glm::vec3& bmin, const glm::vec3& bmax)
	{
		glm::vec3 invRaydir = 1.0f / ray.Direction;
		glm::vec3 t0 = (bmin - ray.Origin) * invRaydir;
		glm::vec3 t1 = (bmax - ray.Origin) * invRaydir;
		glm::vec3 tmin = min(t0, t1), tmax = max(t0, t1);

		float maxComponent = std::max(std::max(tmin.x, tmin.y), tmin.z);
		float minComponent = std::min(std::min(tmax.x, tmax.y), tmax.z);

		return maxComponent <= minComponent;
	}
}