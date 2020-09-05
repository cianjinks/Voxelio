#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;
in mat4 v_MVP;

uniform float u_Time;
uniform usamplerBuffer u_VoxelData;

struct Ray {
	vec3 origin, direction, invDirection;
};

struct Hit {
	bool hit;
	float t;
	vec3 pos;
	uint parent;
	uint idx;
	uint scale;
	vec3 normal;
};

const int c_popc8LUT[256] = int[256](
     0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
     1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
     1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
     1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
     3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
     1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
     3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
     3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
     3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
     4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
);

int popc8(uint mask) {
	return c_popc8LUT[mask & 0xFFu];
}

struct OcStackElement {
	uint parent_idx;
	uint child_idx;
	float tmax;
	float scale;
};

uint GetBranchIndex(uint descriptor, uint index) {
	int count = -1;
		for (int i = 0; i <= index; i++)
		{
			//if (GetLeafMaskBit(i) == 0)
			if(((descriptor >> i) & 0x0001) == 0)
			{
				count++;
			}
		}
	return count;
}

bool traceray(Ray ray) {
	
	Hit result;

	//const int s_max = 23; // Maximum scale (number of float mantissa bits).
	const float ray_coef = 0.00002f;
	const float ray_bias = 0.0f;
	const float epsilon = 1.0f / float(1 << 23);

	OcStackElement stack[11]; // Stack of parent voxels (local mem).
	// Get rid of small ray direction components to avoid division by zero.
	if (abs(ray.direction.x) < epsilon) ray.direction.x = epsilon * sign(ray.direction.x);
	if (abs(ray.direction.y) < epsilon) ray.direction.y = epsilon * sign(ray.direction.y);
	if (abs(ray.direction.z) < epsilon) ray.direction.z = epsilon * sign(ray.direction.z);

	// Precompute the coefficients of tx(x), ty(y), and tz(z).
	// The octree is assumed to reside at coordinates [1, 2].
	float tx_coef = 1.0f / -abs(ray.direction.x);
	float ty_coef = 1.0f / -abs(ray.direction.y);
	float tz_coef = 1.0f / -abs(ray.direction.z);

	float tx_bias = tx_coef * ray.origin.x;
	float ty_bias = ty_coef * ray.origin.y;
	float tz_bias = tz_coef * ray.origin.z;

	// Select octant mask to mirror the coordinate system so
	// that ray direction is negative along each axis.
	int octant_mask = 7;
	if (ray.direction.x > 0.0f) octant_mask ^= 1, tx_bias = 3.0f * tx_coef - tx_bias;
	if (ray.direction.y > 0.0f) octant_mask ^= 2, ty_bias = 3.0f * ty_coef - ty_bias;
	if (ray.direction.z > 0.0f) octant_mask ^= 4, tz_bias = 3.0f * tz_coef - tz_bias;

	// Initialize the active span of t-values.
	float t_min = max(max( tx_coef - tx_bias,  ty_coef - ty_bias),        tz_coef - tz_bias);
	float t_max = min(min(       tx_coef - tx_bias,        ty_coef - ty_bias),        tz_coef - tz_bias);
	float h = t_max;
	t_min = max(t_min, 0.0f);
	t_max = min(t_max, 1.0f);

	// Initialize the current voxel to the first child of the root.

	uint parentptr = 0; //root
	uint child_descriptor = 0; // invalid until fetched
	uint idx = 0;
	vec3 pos = vec3(1.0f);
	float scale = 0.5;
	int stackptr = 1;

	if (1.5f * tx_coef - tx_bias > t_min) idx ^= 1, pos.x = 1.5f;
	if (1.5f * ty_coef - ty_bias > t_min) idx ^= 2, pos.y = 1.5f;
	if (1.5f * tz_coef - tz_bias > t_min) idx ^= 4, pos.z = 1.5f;

	// Traverse voxels along the ray as long as the current voxel
	// stays within the octree.

	while(stackptr > 0) {

		// Fetch child descriptor unless it is already valid.
		if (child_descriptor == 0) {
			child_descriptor = texelFetch(u_VoxelData, int(parentptr)).r;
		}

		// Determine maximum t-value of the cube by evaluating
		// tx(), ty(), and tz() at its corner.
		float tx_corner = pos.x * tx_coef - tx_bias;
		float ty_corner = pos.y * ty_coef - ty_bias;
		float tz_corner = pos.z * tz_coef - tz_bias;
		float tc_max = min(min(tx_corner, ty_corner), tz_corner);
		

		// Process voxel if the corresponding bit in valid mask is set
		// and the active t-span is non-empty.
		uint child_shift = idx ^ octant_mask; // permute child slots based on the mirroring
		uint valid_mask = (child_descriptor & 0x0000FF00) >> 8;
		if((valid_mask & (1u << child_shift)) != 0) {

			// Terminate if the voxel is small enough.
			//if (tc_max * ray_size_coef + ray_size_bias >= scale_exp2)
			//break; // at t_min

			// INTERSECT
			// Intersect active t-span with the cube and evaluate
			// tx(), ty(), and tz() at the center of the voxel.
			float tv_max = min(t_max, tc_max);
			float halfvar = scale * 0.5f;
			float tx_center = halfvar * tx_coef + tx_corner;
			float ty_center = halfvar * ty_coef + ty_corner;
			float tz_center = halfvar * tz_coef + tz_corner;

			uint leaf_mask = (child_descriptor & 0x000000FF);
			// Terminate if the corresponding bit in the non-leaf mask is not set.
			if ((leaf_mask & (1u << child_shift)) != 0) {
			    return true;
				break; // at t_min (overridden with tv_min).
			}
			//if((child_masks & 0x0001) != 0) {
			//	result.hit = true;
			//	//break;
			//	return true;
			//}

			// PUSH
			// Write current parent to the stack.
			if (tc_max < h) {
				stack[stackptr++] = OcStackElement(parentptr, child_shift, t_max, scale);
			}
			h = tc_max;

			// Find child descriptor corresponding to the current voxel.
			//int ofs = (unsigned int)child_descriptor.x >> 17; // child pointer
			//if ((child_descriptor.x & 0x10000) != 0) // far
			//ofs = parent[ofs * 2]; // far pointer
			//ofs += popc8(child_masks & 0x7F);
			//parent += ofs * 2;
			uint index = child_descriptor >> 16;
			parentptr = index + GetBranchIndex(child_descriptor, child_shift);

			// Select child voxel that the ray enters first.
			idx = 0;
			scale = halfvar;
			if (tx_center > t_min) idx ^= 1, pos.x += scale;
			if (ty_center > t_min) idx ^= 2, pos.y += scale;
			if (tz_center > t_min) idx ^= 4, pos.z += scale;

			// Update active t-span and invalidate cached child descriptor.
			t_max = tv_max;
			child_descriptor = 0;
			continue;
		}

		// ADVANCE
		// Step along the ray.
		int step_mask = 0;
		if (tx_corner <= tc_max) step_mask ^= 1, pos.x -= scale;
		if (ty_corner <= tc_max) step_mask ^= 2, pos.y -= scale;
		if (tz_corner <= tc_max) step_mask ^= 4, pos.z -= scale;

		// Update active t-span and flip bits of the child slot index.
		t_min = tc_max;
		idx ^= step_mask;

		// Proceed with pop if the bit flips disagree with the ray direction.
		if ((idx & step_mask) != 0)
		{
			// POP

			// Restore parent voxel from the stack.
			OcStackElement stackEntry = stack[stackptr--];
			parentptr = stackEntry.parent_idx;
			t_max = stackEntry.tmax;
			scale = stackEntry.scale;
			idx = stackEntry.child_idx;

			// Prevent same parent from being stored again and invalidate cached child descriptor.
			h = 0.0f;
			child_descriptor = 0;
		}
	}
	
	// Undo mirroring of the coordinate system.
	if ((octant_mask & 1) == 0) pos.x = 3.0f - scale - pos.x;
	if ((octant_mask & 2) == 0) pos.y = 3.0f - scale - pos.y;
	if ((octant_mask & 4) == 0) pos.z = 3.0f - scale - pos.z;

	//result.hit = false;
	return false;
}

vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

vec4 calculateLighting(inout Hit hit) {
    /**vec3 voxelColor = vec3(1.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightPos = vec3(2.0f, 0.0f, 0.0f);
    vec3 lightDir = normalize(lightPos - hit.pos);
    float diff = max(dot(hit.normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // Result
    vec3 result = (ambient + diffuse) * voxelColor;
    return vec4(result, 1.0f);**/

	return vec4(1.0f / hit.pos, 1.0f);
}

void main() {
	vec3 cameraDir = vec3(0.0f, 0.0f, 0.8f);
	vec3 rayOrigin = vec3(0.0f, 0.0f, -4.5f);
	vec3 rayDir = cameraDir + vec3(v_Pos);

	rayOrigin.xz = rotate2d(rayOrigin.xz, u_Time);
   	rayDir.xz = rotate2d(rayDir.xz, u_Time);

	Ray ray;
	ray.origin = rayOrigin;
	ray.direction = rayDir;
	ray.invDirection = 1 / rayDir;

	bool trace = traceray(ray);
	
	if(!trace) {
		//a_Color = vec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f);
		a_Color = vec4(0.0f);
	}
	else {
		a_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//a_Color = calculateLighting(hit);
	}
	//a_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}