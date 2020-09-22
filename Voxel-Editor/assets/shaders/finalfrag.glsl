#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;

uniform float u_Time;
uniform int u_MaxOctreeNodes;
uniform mat4 u_PerspectiveViewMatrix;
uniform float u_CameraRadius;

uniform usamplerBuffer u_VoxelData;
uniform samplerBuffer u_ColorData;

struct Ray
{
	vec3 Origin, Direction;
};

struct OctreeStackElement
{
	uint node;
	vec3 position;
	float size;
	vec3 index;
	float h;
};

struct RayHit
{
	bool Collision;
	vec2 t;
	vec3 tmid, tmax;
	vec3 normal;
};

int get2DIndex(vec3 index)
{
	for (int i = 0; i < 3; i++)
	{
		if (index[i] == -1.0f)
		{
			index[i] = 0.0f;
		}
	}
	return int(index.x + (2 * index.y) + (4 * index.z));
}

int GetBranchIndex(uint data, int index) {
	int count = -1;
	for (int i = 0; i <= index; i++)
	{
		if (((data >> i) & 0x0001) == 0)
		{
			count++;
		}
	}
	return count;
}

int GetValidMaskBit(uint data, int index)
{
	uint clearmask = 0xFFFFFFFE;
	uint bit = data >> (8 + index);
	return int(bit & ~clearmask);
}

int GetLeafMaskBit(uint data, int index)
{
	uint clearmask = 0xFFFFFFFE;
	uint bit = data >> index;
	return int(bit & ~clearmask);
}

vec4 getColor(uint voxelIndex, uint parentptr)
{
	uvec3 colorIndices = texelFetch(u_VoxelData, int(parentptr)).gba;
	uint colorIndex = 0;

	uint clearmask = 0x00000FFF;
	switch (voxelIndex)
	{
		case 0:
		{
			colorIndex = colorIndices.x >> 20;
			break;
		}
		case 1:
		{
			colorIndex = (colorIndices.x >> 8) & clearmask;
			break;
		}
		case 2:
		{
			// Last 8 bits of m_ColorIndex1 are first 8 bits of color index
			uint firstEight = (colorIndices.x & 0x000000FF) << 4;
			// First 4 bits of m_ColorIndex2 are last 4 bits of color index
			uint lastFour = (colorIndices.y & 0xF0000000) >> 28;

			colorIndex = firstEight | lastFour;
			break;
		}
		case 3:
		{
			colorIndex = (colorIndices.y >> 16) & clearmask;
			break;
		}
		case 4:
		{
			colorIndex = (colorIndices.y >> 4) & clearmask;
			break;
		}
		case 5:
		{
			// Last 4 bits of m_ColorIndex2 are first 4 bits of color index
			uint firstFour = (colorIndices.y & 0x0000000F) << 8;
			// First 8 bits of m_ColorIndex3 are last 8 bits of color index
			uint lastEight = (colorIndices.z & 0xFF000000) >> 24;

			colorIndex = firstFour | lastEight;
			break;
		}
		case 6:
		{
			colorIndex = (colorIndices.z >> 12) & clearmask;
			break;
		}
		case 7:
		{
			colorIndex = colorIndices.z & clearmask;
			break;
		}
		default:
		{
			colorIndex = 0;
			break;
		}

	}
	return texelFetch(u_ColorData, int(colorIndex)).rgba;
}

RayHit RayAABBCollision(in Ray ray, in vec3 position, in float scale)
{
		RayHit hit;

		vec3 bmin = position - scale;
		vec3 bmax = position + scale;

		vec3 invRaydir = 1.0f / ray.Direction;
		vec3 t0 = (bmin - ray.Origin) * invRaydir;
		vec3 t1 = (bmax - ray.Origin) * invRaydir;
		vec3 tmin = min(t0, t1), tmax = max(t0, t1);

		hit.tmid = (position - ray.Origin) * invRaydir;

		// Normal Calculation
		hit.normal = -sign(ray.Direction) * step(tmin.yzx, tmin.xyz) * step(tmin.zxy, tmin.xyz);

		float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
		float minComponent = min(min(tmax.x, tmax.y), tmax.z);

		hit.Collision = maxComponent <= minComponent;
		hit.t = vec2(maxComponent, minComponent);
		hit.tmax = tmax;

		return hit;
}

vec4 shading(in RayHit hit, vec4 color)
{
	//if(hit.normal.x < 0 || hit.normal.y < 0 || hit.normal.z < 0)
	//{
	//	return vec4(vec3(color * 0.75), 1.0f);
	//}
	//return color;

	if(hit.normal.z > 0)
	{
		// Front Face:
		return vec4(vec3(color * 0.93), 1.0f);
	}
	if(hit.normal.x > 0)
	{
		// Right Face:
		return vec4(vec3(color * 0.84), 1.0f);
	}
	if(hit.normal.y > 0)
	{
		// Top Face:
		return vec4(vec3(color * 1.03), 1.0f);
	}
	if(hit.normal.x <= 0)
	{
		// Left Face:
		return vec4(vec3(color * 0.7), 1.0f);
	}
	if(hit.normal.z <= 0)
	{
		// Back Face
		return vec4(vec3(color * 0.78), 1.0f);
	}
	if(hit.normal.y <= 0)
	{
		// Bottom Face
		return vec4(vec3(color * 0.86), 1.0f);
	}
	return color;
}

vec4 rayTrace(Ray ray)
{
	vec3 pos = vec3(0.0f);
	float size = 1.0f;
	bool canPush = true;

	// Initialize stack
	OctreeStackElement OctreeStack[64];
	int stackptr = 0;

	RayHit hit = RayAABBCollision(ray, pos, size);
	float h = hit.t.y;

	if(!hit.Collision)
	{
		return vec4(vec3(0.5f), 1.0f);
	}

	// Inital push (Obtains the index, position and scale of the first octant voxel upon entering the cube)
	vec3 index = mix(-sign(ray.Direction), sign(ray.Direction), lessThanEqual(hit.tmid, vec3(hit.t.x)));
	size *= 0.5;
	pos += size * index;

	// Retrieve this initial octant voxel
	int childIndex = get2DIndex(index);
	uint parentptr = 0;
	uint node = texelFetch(u_VoxelData, int(parentptr)).r; // Root

	// while(true) for now but we may want some way to make sure it always breaks
	while(true)
	{
		// Run another collision test for new octant
		hit = RayAABBCollision(ray, pos, size);

		// Check if the voxel exists
		if(GetValidMaskBit(node, childIndex) == 1)
		{
			// If it is a leaf and exists it means we have hit a voxel
			if(GetLeafMaskBit(node, childIndex) == 1)
			{
				//return vec4(0.75f, 0.75f, 0.75f, 1.0f);
				return shading(hit, getColor(uint(childIndex), parentptr));
			}
			else if(canPush)
			{
				// PUSH
				if (hit.t.y < h)
				{
					OctreeStack[stackptr++] = OctreeStackElement(node, pos, size, index, h);
				}
				h = hit.t.y;

				index = mix(-sign(ray.Direction), sign(ray.Direction), lessThanEqual(hit.tmid, vec3(hit.t.x)));
				size *= 0.5;
				pos += size * index;

				// Retrieve next node from the array
				parentptr = (node >> 16) + childIndex;//GetBranchIndex(node, childIndex);
				node = texelFetch(u_VoxelData, int(parentptr)).r;
				childIndex = get2DIndex(index);
				continue;
			}
		}

		// ADVANCE
		vec3 oldIndex = index;
		index = mix(index, sign(ray.Direction), equal(hit.tmax, vec3(hit.t.y)));
		pos += mix(vec3(0), sign(ray.Direction), notEqual(oldIndex, index)) * (2.0f * size);

		childIndex = get2DIndex(index);

		if (index == oldIndex)
		{
			// POP
			if (stackptr <= 0)
			{ 
				// Didn't hit anything along this ray
				return vec4(vec3(0.5f), 1.0f);
			}

			OctreeStackElement element = OctreeStack[--stackptr];
			node = element.node;
			pos = element.position;
			size = element.size;
			index = element.index;
			h = element.h;

			canPush = false;
		}
		else { canPush = true; }

	}
	return vec4(vec3(0.5f), 1.0f);
}

void main()
{
	// Generate Ray
	vec3 cameraDir = vec3(0.0, 0.0, -1.0);
    vec3 rayDir = cameraDir + vec3(v_Pos);
	vec3 rayOrigin = vec3(0.0f, 0.0f, u_CameraRadius);

	Ray ray;
	ray.Origin = vec3(vec4(rayOrigin, 1.0f) * u_PerspectiveViewMatrix);
	ray.Direction = vec3(vec4(rayDir, 1.0f) * u_PerspectiveViewMatrix);

	a_Color = rayTrace(ray);
}