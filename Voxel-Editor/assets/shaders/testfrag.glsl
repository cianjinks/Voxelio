#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;

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
const vec3 POS[8] = vec3[8](PNN, PNP, PPN, PPP, NNN, NNP, NPN, NPP);

struct Ray { vec3 o, d, invDir; };

struct Hit {
	vec3 p;
	float t;	// solution to p=o+t*d
	float tmax;    //distance to exit point?
	float tmin;    // distance to enter point?
	vec3 n;		// normal
};

bool AABBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray r, out Hit hit) {
	vec3 t0 = (boxMin - r.o) * r.invDir;
	vec3 t1 = (boxMax - r.o) * r.invDir;
	vec3 tmin = min(t0, t1), tmax = max(t0, t1);

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

	return maxComponent <= minComponent;
}

vec4 trace(Ray ray, inout Hit hit) {
	vec3 center = vec3(0.0f);
    float scale = 1.0f;
	vec3 minBox = center - scale;
	vec3 maxBox = center + scale;
	vec4 f = vec4(1.0f);
    struct Stack {
		uint index;
		vec3 center;
		float scale;
	};
    Stack stack[10];
    int stackPos = 1;
    if (!AABBoxIntersect(minBox, maxBox, ray, hit)) return f;
    uint index = 0u;
    scale *= 0.5f;
    stack[0] = Stack( 0u, center, scale);
    while(stackPos-- > 0) {
        f = vec4(0.1f);
        center = stack[stackPos].center;
		index = stack[stackPos].index;
		scale = stack[stackPos].scale;
        uint voxel_node = texelFetch(u_VoxelData, int(index)).r;
        uint voxel_group_offset = voxel_node >> 16;
        uint voxel_child_mask = (voxel_node & 0x0000FF00u) >> 8u;
        uint voxel_leaf_mask = voxel_node & 0x000000FFu;
        uint accumulated_offset = 0u;
        for (uint i = 0u; i < 8u; ++i) {
            bool empty = (voxel_child_mask & (1u << i)) == 0u;
            bool is_leaf = (voxel_leaf_mask & (1u << i)) != 0u;
            if (empty){ //empty
                continue;
            }
            
            vec3 new_center = center + scale * POS[i];
            vec3 minBox = new_center - scale;
            vec3 maxBox = new_center + scale;
            

            if (!AABBoxIntersect(minBox, maxBox, ray, hit)){
                if(!is_leaf){
                   accumulated_offset +=1u;
                }
                continue;
            }
            if (is_leaf){ //not empty, but a leaf
                return vec4(1.0f,0.0f,0.0f,1.0f);
            } else { //not empty and not a leaf
            	stack[stackPos++] = Stack(voxel_group_offset+accumulated_offset, new_center, scale*0.5f   );
            	f.z += 0.4f;
                accumulated_offset+=1u;
            }
        }
    }
	return f;
}
vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

void main() {
	vec3 cameraDir = vec3(0.0, 0.0, 0.8);
	vec3 cameraPlaneU = vec3(1.0, 0.0, 0.0);
	vec3 cameraPlaneV = vec3(0.0, 1.0, 0.0);
    vec3 rayDir = cameraDir + v_Pos.x * cameraPlaneU + v_Pos.y * cameraPlaneV;
	vec3 rayPos = vec3(0.0, 0.25 * sin(u_Time * 2.7), -3.4);
    rayPos.xz = rotate2d(rayPos.xz, u_Time);
   	rayDir.xz = rotate2d(rayDir.xz, u_Time);
	Ray ray;
	Hit hit;
	ray.o = rayPos;
	ray.d = rayDir;
	ray.invDir = 1.0f/rayDir;
	vec4 color = trace(ray, hit);
    if (length(color) > 0.5f) {
       a_Color = color;
	}
}