#include "voxelpch.h"
#include "Ray.h"

namespace VoxelCore {

	Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
		: Origin(origin), Direction(direction) {}

	Ray::~Ray() {}

	// Based on http://jcgt.org/published/0007/03/04/ efficient slab test
	RayHit Ray::RayAABBCollision(Ray& ray, glm::vec3& position, float scale)
	{
		RayHit hit;

		glm::vec3 bmin = position - scale;
		glm::vec3 bmax = position + scale;

		glm::vec3 invRaydir = 1.0f / ray.Direction;
		glm::vec3 t0 = (bmin - ray.Origin) * invRaydir;
		glm::vec3 t1 = (bmax - ray.Origin) * invRaydir;
		glm::vec3 tmin = min(t0, t1), tmax = max(t0, t1);

		hit.tmid = (position - ray.Origin) * invRaydir;

		hit.normal = -sign(ray.Direction) * step(glm::vec3(tmin.y, tmin.z, tmin.x), glm::vec3(tmin.x, tmin.y, tmin.z)) * step(glm::vec3(tmin.z, tmin.x, tmin.y), glm::vec3(tmin.x, tmin.y, tmin.z));

		float maxComponent = std::max(std::max(tmin.x, tmin.y), tmin.z);
		float minComponent = std::min(std::min(tmax.x, tmax.y), tmax.z);

		hit.Collision = maxComponent <= minComponent;
		hit.t = glm::vec2(maxComponent, minComponent);
		hit.tmax = tmax;

		return hit;
	}

	// Utility Functions:
	glm::vec3 Ray::mix(glm::vec3& x, glm::vec3& y, glm::bvec3& a)
	{
		glm::vec3 result;
		for (int i = 0; i < 3; i++)
		{
			if (a[i])
			{
				result[i] = y[i];
			}
			else
			{
				result[i] = x[i];
			}
		}
		return result;
	}

	glm::bvec3 Ray::lessThanEqual(glm::vec3& a, glm::vec3& b)
	{
		return glm::bvec3(a.x <= b.x, a.y <= b.y, a.z <= b.z);
	}

	glm::bvec3 Ray::equal(glm::vec3& a, glm::vec3& b)
	{
		return glm::bvec3(a.x == b.x, a.y == b.y, a.z == b.z);
	}

	glm::bvec3 Ray::notEqual(glm::vec3& a, glm::vec3& b)
	{
		return glm::bvec3(a.x != b.x, a.y != b.y, a.z != b.z);
	}

	glm::vec3 Ray::sign(glm::vec3& value)
	{
		glm::vec3 result;
		for (int i = 0; i < 3; i++)
		{
			if (value[i] > 0.0f)
			{
				result[i] = 1.0f;
			}
			else if (value[i] < 0.0f)
			{
				result[i] = -1.0f;
			}
			else
			{
				result[i] = 0.0f;
			}
		}
		return result;
	}

	glm::vec3 Ray::step(glm::vec3& edge, glm::vec3& x)
	{
		glm::vec3 result;
		for (int i = 0; i < 3; i++)
		{
			if (x[i] < edge[i])
			{
				result[i] = 0.0f;
			}
			else
			{
				result[i] = 1.0f;
			}
		}
		return result;
	}
}