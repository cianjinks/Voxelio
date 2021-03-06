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
	float tmax;
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

	const uint s_max = 128u;
	const float epsilon = 1.0f / float(1 << s_max);

	OcStackElement stack[s_max + 1]; // Stack of parent voxels (local mem).

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
	float t_min = max(max(2.0f * tx_coef - tx_bias, 2.0f * ty_coef - ty_bias), 2.0f * tz_coef - tz_bias);
	float t_max = min(min(tx_coef - tx_bias, ty_coef - ty_bias), tz_coef - tz_bias);
	float h = t_max;
	t_min = max(t_min, 0.0f);
	t_max = min(t_max, 1.0f);

	// Initialize the current voxel to the first child of the root.
	uint parentptr = 0;
	uint child_descriptor = 0; // invalid until fetched
	uint idx = 0;
	vec3 pos = vec3(1.0f);
	uint scale = s_max - 1;
	float scale_exp2 = 0.5f; // exp2f(scale - s_max)
	if (1.5f * tx_coef - tx_bias > t_min) idx ^= 1, pos.x = 0.0f;
	if (1.5f * ty_coef - ty_bias > t_min) idx ^= 2, pos.y = 0.0f;
	if (1.5f * tz_coef - tz_bias > t_min) idx ^= 4, pos.z = 0.0f;

	// Traverse voxels along the ray as long as the current voxel
	// stays within the octree.
	while(scale < s_max) {

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
		uint child_masks = child_descriptor >> child_shift;

		// This would mean this child is valid:
		if((child_masks & 0x0100) != 0 && t_min <= t_max) {

			// Terminate if the voxel is small enough.
			//if (tc_max * ray_size_coef + ray_size_bias >= scale_exp2)
			//break; // at t_min

			// INTERSECT
			// Intersect active t-span with the cube and evaluate
			// tx(), ty(), and tz() at the center of the voxel.
			float tv_max = min(t_max, tc_max);
			float s_half = scale_exp2 * 0.5f;
			float tx_center = s_half * tx_coef + tx_corner;
			float ty_center = s_half * ty_coef + ty_corner;
			float tz_center = s_half * tz_coef + tz_corner;

			if(t_min <= tv_max) {

				// Terminate if the corresponding bit in the non-leaf mask is not set.
				if((child_masks & 0x0001) == 0) {
					break;
				}

				if(tc_max < h) {
					// NOTE: child_descriptor might be incorrect
					//stack[scale] = OcStackElement(parentptr, t_max);
					stack[scale].parent_idx = parentptr;
					stack[scale].tmax = t_max;
				}
				h = tc_max;

				// Find child descriptor corresponding to current voxel

				uint ofs = child_descriptor >> 16;
				parentptr = ofs + GetBranchIndex(child_descriptor, child_shift);
				//ofs += popc8(child_masks & 0x7F);
				//parentptr += ofs * 2;

				// Select child voxel that the ray enters first.
				idx = 0;
				scale--;
				scale_exp2 = s_half;

				if (tx_center > t_min) idx ^= 1, pos.x += scale_exp2;
				if (ty_center > t_min) idx ^= 2, pos.y += scale_exp2;
				if (tz_center > t_min) idx ^= 4, pos.z += scale_exp2;
				// Update active t-span and invalidate cached child descriptor.
				t_max = tv_max;
				child_descriptor = 0;
				continue;

			}
		}
		// ADVANCE
		// Step along the ray.
		int step_mask = 0;
		if (tx_corner <= tc_max) step_mask ^= 1, pos.x -= scale_exp2;
		if (ty_corner <= tc_max) step_mask ^= 2, pos.y -= scale_exp2;
		if (tz_corner <= tc_max) step_mask ^= 4, pos.z -= scale_exp2;
		// Update active t-span and flip bits of the child slot index.
		t_min = tc_max;
		idx ^= step_mask;
		// Proceed with pop if the bit flips disagree with the ray direction.

		if((idx & step_mask) != 0) {
			// POP
			// Find the highest differing bit between the two positions.
			unsigned int differing_bits = 0;
			if ((step_mask & 1) != 0) differing_bits |= int(pos.x) ^ int(pos.x + scale_exp2);
			if ((step_mask & 2) != 0) differing_bits |= int(pos.y) ^ int(pos.y + scale_exp2);
			if ((step_mask & 4) != 0) differing_bits |= int(pos.z) ^ int(pos.z + scale_exp2);
			scale = (int(float(differing_bits)) >> 23) - 127; // position of the highest bit
			scale_exp2 = float((scale - s_max + 127) << 23); // exp2f(scale - s_max)
			// Restore parent voxel from the stack.
			OcStackElement stackEntry = stack[scale];
			parentptr = stackEntry.parent_idx;
			t_max = stackEntry.tmax;
			// Round cube position and extract child slot index.
			int shx = int(pos.x) >> scale;
			int shy = int(pos.y) >> scale;
			int shz = int(pos.z) >> scale;
			pos.x = float(shx << scale);
			pos.y = float(shy << scale);
			pos.z = float(shz << scale);
			idx = (shx & 1) | ((shy & 1) << 1) | ((shz & 1) << 2);
			// Prevent same parent from being stored again and invalidate cached child descriptor.
			h = 0.0f;
			child_descriptor = 0;
		}
	}

	// Indicate miss if we are outside the octree.
	if (scale >= s_max) {
		return false;
	}
	return true;

	// Undo mirroring of the coordinate system.
	if ((octant_mask & 1) == 0) pos.x = 3.0f - scale_exp2 - pos.x;
	if ((octant_mask & 2) == 0) pos.y = 3.0f - scale_exp2 - pos.y;
	if ((octant_mask & 4) == 0) pos.z = 3.0f - scale_exp2 - pos.z;

	// Output results.
	result.t = t_min;
	result.pos.x = min(max(ray.origin.x + t_min * ray.direction.x, pos.x + epsilon), pos.x + scale_exp2 - epsilon);
	result.pos.y = min(max(ray.origin.y + t_min * ray.direction.y, pos.y + epsilon), pos.y + scale_exp2 - epsilon);
	result.pos.z = min(max(ray.origin.z + t_min * ray.direction.z, pos.z + epsilon), pos.z + scale_exp2 - epsilon);
	result.parent = parentptr;
	result.idx = idx ^ octant_mask ^ 7;
	result.scale = scale;

	//return result;
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
		//a_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//a_Color = calculateLighting(hit);
	}
	//a_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}