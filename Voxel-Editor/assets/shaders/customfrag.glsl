#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;

uniform float u_Time;
uniform usamplerBuffer u_VoxelData;

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38

const vec3 PPP = vec3(1, 1, 1);
const vec3 PNP = vec3(1, -1, 1);
const vec3 PNN = vec3(1, -1, -1);
const vec3 NPN = vec3(-1, 1, -1);
const vec3 NNN = vec3(-1, -1, -1);
const vec3 NNP = vec3(-1, -1, 1);
const vec3 NPP = vec3(-1, 1, 1);
const vec3 PPN = vec3(1, 1, -1);
const vec3 POS[8] = vec3[8](NNN, PNN, NPN, PPN, NNP, PNP, NPP, PPP);

struct Ray {
	vec3 origin, direction, invDirection;
};

vec2 AABBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray ray) {

	vec3 t0 = (boxMin - ray.origin) * ray.invDirection;
	vec3 t1 = (boxMax - ray.origin) * ray.invDirection;
	vec3 tmin = min(t0, t1), tmax = max(t0, t1);

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

	return vec2(maxComponent, minComponent);
}

vec4 CalculateChildPosScale(vec3 currentPos, float currentScale, uint index) {
	return vec4(currentPos * POS[index], currentScale / 2);
}

bool IsChildValid(uint parent, uint idx) {
	uint validmask = parent & 0x0000FF00u;
	return (validmask & (1u << (8 + idx))) != 0u;
}

bool IsChildLeaf(uint parent, uint idx) {
	uint leafmask = parent & 0x000000FFu;
	bool isLeaf = (leafmask & (1u << idx)) != 0u;
	return (isLeaf && IsChildValid(parent, idx));
}

// NOTE: Need to make certain this works
uint FindChildDescriptor(uint parent, uint idx) {
	uint indexmask = 0xFFFF0000u;
	uint index = (parent & indexmask) >> 16;

	int count = -1;
	for (int i = 0; i <= idx; i++)
	{
		if (!IsChildLeaf(parent, i))
		{
			count++;
		}
	}
	return texelFetch(u_VoxelData, int(index + count)).r; 
}

// Currently returns tmin because thats what the pseudocode wants
float raycast(const Ray ray) {

    float scale = 1.0f;
	vec3 currCenter = vec3(0.0f);
	vec3 minBox = currCenter - scale;
	vec3 maxBox = currCenter + scale;

	vec2 t = vec2(0.0f, 1.0f);
	vec3 tmid;
	tmid = (currCenter - ray.origin) * ray.invDirection;
	t = AABBoxIntersect(minBox, maxBox, ray);
	float h = t.y;

	uint parent  = texelFetch(u_VoxelData, 0).r; // parent = root
	uint idx = SelectChild(t.x, tmid); // The initial child index is obtained by comparing t.x against tmid. 
									   // tmid calculation should be moved within this function so it will take in (center, ray.origin, ray.invdir, t.x)

	vec4 ps = CalculateChildPosScale(currCenter, scale, idx);
	currCenter = ps.xyz;
	scale = ps.w;

	while(true) {
		vec2 tc = AABBoxIntersect(currCenter - scale, currCenter + scale, ray);
		if(IsChildValid(parent, idx) && t.x <= t.y) {
			if(t.x < 0) return t.x; // This checks if the voxel is small enough to be ignored. However I just use 0 for now so everything is rendered.

			/**INTERSECT**/
			vec2 tv = tc; // This correspomds to line 12 of pseudocode and may be incorrect. To me it seems tc will always be within t so why cant I just do this?
			if(tv.x <= tv.y) {
				if(IsChildLeaf(parent, idx)) return tv.x;
				// Line 19 needs to go here
				h = tc.y;
				parent = FindChildDescriptor(parent, idx);
				idx = SelectChild(currCenter, ray.origin, ray.invDirection, tv.x);
				t = tv;

				vec4 ps = CalculateChildPosScale(currCenter, scale, idx);
				currCenter = ps.xyz;
				scale = ps.w;
				continue;
			}
		}
		/**ADVANCE**/
		vec3 oldpos = currCenter;
		// Here we step along the ray by "scale" which will produce our new cubes position and index. Line 29
		t.x = tc.y;
		if(idx update disagrees with ray) {

		}
	}
}

void main() {

	// Make cameraDir into a uniform
	vec3 cameraDir = vec3(-1.0f, 0.0f, 0.0f);
	vec3 rayOrigin = vec3(4.0f, 0.0f, 0.0f);
	vec3 rayDir = cameraDir + vec3(v_Pos);

	Ray ray;
	ray.origin = rayOrigin;
	ray.direction = rayDir;
	ray.invDirection = 1 / rayDir;
}