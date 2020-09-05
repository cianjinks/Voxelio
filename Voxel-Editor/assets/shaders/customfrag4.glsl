#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;
in mat4 v_MVP;

uniform float u_Time;
uniform usamplerBuffer u_VoxelData;

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
    vec3 origin, direction;
};

const vec4 RED = vec4(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 DEBUG_GREEN = vec4(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 BACKGROUND = vec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);
const vec4 COLORS[3] = vec4[3](RED, DEBUG_GREEN, BACKGROUND);

/**
vec2 isect(in vec3 pos, in float scale, const Ray ray, out vec3 tmid, out vec3 tmin, out vec3 tmax) {

	vec3 boxMin = pos - scale;
	vec3 boxMax = pos + scale;
	vec3 t0 = (boxMin - ray.origin) / ray.direction;
	vec3 t1 = (boxMax - ray.origin) / ray.direction;
	tmin = min(t0, t1);
	tmax = max(t0, t1);
	tmid = (pos - ray.origin) / ray.direction;

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

	return vec2(maxComponent, minComponent);
}**/

vec2 isect(in vec3 pos, in float scale, const Ray ray, out vec3 tmid, out vec3 tmin, out vec3 tmax) {

	vec3 mn = pos - 0.5 * scale;
    vec3 mx = mn + scale;
    vec3 t1 = (mn-ray.origin) / ray.direction;
    vec3 t2 = (mx-ray.origin) / ray.direction;
    tmin = min(t1, t2);
    tmax = max(t1, t2);
    tmid = (pos-ray.origin)/ray.direction; // tmax;

    return vec2(max(tmin.x, max(tmin.y, tmin.z)), min(tmax.x, min(tmax.y, tmax.z)));
}

// Currently assumes a positive direction
/**uint CalculateFirstChildIndex(float tenter, vec3 tmid) {
	uint idx = 0;
	if(tenter > tmid.x) {
		//idx[2]
		idx ^= 4;
	}
	if(tenter > tmid.y) {
		//idx[1]
		idx ^= 2;
	}
	if(tenter > tmid.z) {
		//idx[0]
		idx ^= 1;
	}
	return idx;
}**/

uint CalculateFirstChildIndex(float tenter, vec3 tmid, vec3 rd, out vec3 didx) {
	//return 0u ^ (tenter >= tmid.x ? 1 : 0) ^ (tenter >= tmid.y ? 2 : 0) ^ (tenter >= tmid.z ? 4 : 0);
	didx = mix(-sign(rd), sign(rd), lessThanEqual(tmid, vec3(tenter)));
	return 0u | uint(didx.x > 0.0) << 2 | uint(didx.y > 0.0) << 1 | uint(didx.z > 0.0);
}

/**uint CalculateNextChildIndex(vec3 rd, uint prev_idx, vec3 tmax, vec2 t, out bool exit) {

	int step_mask = 0;
	if (tmax.x <= t.y) step_mask ^= 1;
	if (tmax.y <= t.y) step_mask ^= 2;
	if (tmax.z <= t.y) step_mask ^= 4;

	uint idx = prev_idx ^ step_mask;
	if((idx & step_mask) != 0) {
		exit = true;
	}
	else {
		exit = false;
	}	
	return idx;
}**/

//uint CalculateNextChildIndex(vec3 rd, uint prev_idx, vec3 tmax, vec2 t, out bool exit) {
//	// t.y is the t when we exit the prev_idx voxel
//	// we can compare it to tmax to see which plane is crossed first and modify bits accordingly
//
//	uint idx = prev_idx;
//	exit = false;
//	if(tmax.x == t.y) {
//		if((idx & 4) != 0) 
//		{
//			exit = true;
//		}
//		idx ^= 4;
//	}
//	else if(tmax.y == t.y) {
//		if((idx & 2) != 0) 
//		{
//			exit = true;
//		}
//		idx ^= 2;
//	}
//	else {
//		if((idx & 1) != 0) 
//		{
//			exit = true;
//		}
//		idx ^= 1;
//	}
//	return idx;
//
//
//}


uint CalculateNextChildIndex(vec3 tmax, vec2 t, vec3 didx, vec3 rd, out bool exit) {
	vec3 dold = didx;
    didx = mix(didx, sign(rd), equal(tmax, vec3(t.y)));
	if(dold == didx) {
		exit = true;
	}
	else {
		exit = false;
	}
	return 0u | uint(didx.x > 0.0) << 2 | uint(didx.y > 0.0) << 1 | uint(didx.z > 0.0);

}

struct OcStackElement {
	uint parent;
	uint idx;
	vec3 pos; // center
	float scale;
};

uint GetBranchIndex(uint parent, uint idx) {
	int count = -1;
	for (int i = 0; i <= idx; i++)
	{
		if (((parent >> i) & 0x0001) == 0)
		{
			count++;
		}
	}
	return count;
}

int trace(const Ray ray) {
    
	OcStackElement stack[10];
	int stackptr = 1;

	vec3 currPos = vec3(0.0f); // Center of root voxel
	float scale = 1.0f;	// Scale of root voxel
	uint parent = texelFetch(u_VoxelData, 0).r; // Root voxel child descriptor
	uint idx;

	vec2 t;
	vec3 tmid;
	vec3 tmin;
	vec3 tmax;

	bool canPush = true;

	// t = vec2(t for entry, t for exit). tmid = vec3(mid x plane intersect, mid y plane intersect, mid z plane intersect). tmax = exit point? i think
	t = isect(currPos, scale, ray, tmid, tmin, tmax);

	if(t.x > t.y) {
		return 2;
	}

	// Calculate first child index based on which child the ray entered first:
	vec3 didx;
	//idx = CalculateFirstChildIndex(t.x, tmid, ray.direction, didx);
	bvec3 q = lessThanEqual(tmid, vec3(t.x));
	didx = mix(-sign(ray.direction), sign(ray.direction), q);
    vec3 tq = mix(tmid, tmax, q); // tmax of the resulting voxel
    didx = mix(-didx, didx, greaterThanEqual(tq, vec3(0))); // Don't worry about voxels behind `ro`
	idx = 0u | uint(didx.x > 0.0) << 2 | uint(didx.y > 0.0) << 1 | uint(didx.z > 0.0);

	scale *= 0.5f;
	currPos += scale * POS[idx];

	//stack[stackptr++] = OcStackElement(parent, idx, currPos, scale);

	while(stackptr > 0) {
		// Ray intersection test to get t values:
		t = isect(currPos, scale, ray, tmid, tmin, tmax);

		// I never actually check if an intersction occurs. NAWDOAWODWAODAWOWO
		if(canPush) {
			// If valid bit is set:
			uint voxel_valid_mask = (parent & 0x0000FF00u) >> 8u;
			if((voxel_valid_mask & (1u << idx)) != 0u) {
				// If leaf bit is not set for child:
				uint voxel_leaf_mask = parent & 0x000000FFu;
				if((voxel_leaf_mask & (1u << idx)) == 0u) {
					
					// PUSH

					// Push old voxel onto stack
					stack[stackptr++] = OcStackElement(parent, idx, currPos, scale);

					// Calculate a new child index for the branch
					//idx = CalculateFirstChildIndex(t.x, tmid, ray.direction, didx);
					q = lessThanEqual(tmid, vec3(t.x));
					didx = mix(-sign(ray.direction), sign(ray.direction), q);
					tq = mix(tmid, tmax, q); // tmax of the resulting voxel
					didx = mix(-didx, didx, greaterThanEqual(tq, vec3(0))); // Don't worry about voxels behind `ro`
					idx = 0u | uint(didx.x > 0.0) << 2 | uint(didx.y > 0.0) << 1 | uint(didx.z > 0.0);

					// Calculate this childs position and scale
					scale *= 0.5f;
					currPos += scale * POS[idx];

					// Calculate the childs new parent:
					parent = texelFetch(u_VoxelData, int((parent >> 16) + GetBranchIndex(parent, idx))).r;

					continue; // After PUSH we need to continue to loop start again
				}
				else
				{
					// Voxel is a leaf
					return 0;
				}
			}	
		}
		// Voxel is not valid 
		// ADVANCE

		// Calculate the next siblings index (parent is the same):
		uint old_idx = idx;
		//vec3 dold = didx;
        //didx = mix(didx, sign(ray.direction), equal(tmax, vec3(t.y)));
		//idx = 0u | uint(didx.x > 0.0) << 2 | uint(didx.y > 0.0) << 1 | uint(didx.z > 0.0);

		bool exit = false;
		//idx = CalculateNextChildIndex(ray.direction, idx, tmax, t, exit);
		idx = CalculateNextChildIndex(tmax, t, didx, ray.direction, exit);
		 
		// Increment the position for the new sibling voxel (scale is the same):
		// Need to figure how to do this
		// Attempt 1:
		currPos -= scale * POS[old_idx]; // Undo position of current voxel back to parents
		currPos += scale * POS[idx]; // Redo position calculation for new voxel


		// After ADVANCE we need to determine if it actually did advance to a sibling or exited the voxel it was inside, in which case it would be time to POP
		if(exit) {

			// POP
			// Restore the parent element from the stack
			OcStackElement popElement = stack[stackptr--];
			parent = popElement.parent;
			idx = popElement.idx;
			currPos = popElement.pos;
			scale = popElement.scale;

			// Here we need to advance to the next sibling
			// Done by preventing the same parent being written again
			canPush = false;
		} else canPush = true;
	}
	return 2;
}

vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

void main() {

	// Make cameraDir into a uniform
	vec3 cameraDir = vec3(0.0f, 0.0f, 0.8f);
	vec3 rayOrigin = vec3(0.0f, 0.0f, -2.5f);
	vec3 rayDir = cameraDir + vec3(v_Pos);

	rayOrigin.xz = rotate2d(rayOrigin.xz, u_Time);
   	rayDir.xz = rotate2d(rayDir.xz, u_Time);

	Ray ray;
	ray.origin = rayOrigin;
	ray.direction = rayDir;

	int hit = trace(ray);
	a_Color = COLORS[hit];
	//if(hit)
	//{
	//	a_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//}
	//else
	//{
	//	a_Color = vec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);
	//}
}