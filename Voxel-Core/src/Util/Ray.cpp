#include "voxelpch.h"
#include "Ray.h"

namespace VoxelCore {

	Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
		: Origin(origin), Direction(direction) {}

	Ray::~Ray() {}

	// Based on http://jcgt.org/published/0007/03/04/ efficient slab test
	RayHit Ray::RayAABBCollision(const Ray& ray, const glm::vec3& position, const float scale)
	{
		RayHit hit;

		glm::vec3 bmin = position - scale;
		glm::vec3 bmax = position + scale;

		glm::vec3 invRaydir = 1.0f / ray.Direction;
		glm::vec3 t0 = (bmin - ray.Origin) * invRaydir;
		glm::vec3 t1 = (bmax - ray.Origin) * invRaydir;
		glm::vec3 tmin = min(t0, t1), tmax = max(t0, t1);

		hit.tmid = (position - ray.Origin) * invRaydir;

		float maxComponent = std::max(std::max(tmin.x, tmin.y), tmin.z);
		float minComponent = std::min(std::min(tmax.x, tmax.y), tmax.z);

		hit.Collision = maxComponent <= minComponent;
		hit.t = glm::vec2(maxComponent, minComponent);
		hit.tmax = tmax;

		return hit;
	}
}