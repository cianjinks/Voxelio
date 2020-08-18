// Majority of ray trace code from: https://www.shadertoy.com/view/WlXXWf
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
const vec3 POS[8] = vec3[8](NNN, PNN, NPN, PPN, NNP, PNP, NPP, PPP);


struct Ray { vec3 o, d, invDir; };

struct Hit {
    vec3 hitPos;
    float tmin; //Distance to entry
    float tmax; //Distance to exit
	vec3 normal; //Normal
};

bool AABBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray ray, out Hit hit) {

	vec3 t0 = (boxMin - ray.o) * ray.invDir;
	vec3 t1 = (boxMax - ray.o) * ray.invDir;
	vec3 tmin = min(t0, t1), tmax = max(t0, t1);

    hit.hitPos = tmin; // MAYBE WRONG (can calculate from t values)

    //hit.normal = -sign(ray.d) * step(tmin.yzx, tmin.xyz) * step(tmin.zxy, tmin.xyz);

    // Normals
    vec3 center = (boxMin + boxMax) * 0.5;
    vec3 p = tmin - center; //Center to hit point vector
    vec3 divisor = (boxMin - boxMax) * 0.5;
    float bias = 1.000001;

    hit.normal = normalize(vec3(
                            float(int(p.x / abs(divisor.x) * bias)),
                            float(int(p.y / abs(divisor.y) * bias)),
                            float(int(p.z / abs(divisor.z) * bias))
                          ));

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

    hit.tmin = minComponent;
    hit.tmax = maxComponent;

	return maxComponent <= minComponent;
}


vec4 calculateLighting(inout Hit hit) {
    vec3 voxelColor = vec3(1.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightPos = vec3(2.0f, 0.0f, 0.0f);
    vec3 lightDir = normalize(lightPos - hit.hitPos);
    float diff = max(dot(hit.normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // Result
    vec3 result = (ambient + diffuse) * voxelColor;
    return vec4(result, 1.0f);
}

vec4 trace(Ray ray, inout Hit hit) {
	vec3 center = vec3(0.0f);
    float scale = 1.0f;
	vec3 minBox = center - scale;
	vec3 maxBox = center + scale;
    struct Stack {
		uint index;
		vec3 center;
		float scale;
	};
    Stack stack[10];
    int stackPos = 1;
    if (!AABBoxIntersect(minBox, maxBox, ray, hit)) return vec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f); // The blue color of the background
    uint index = 0u;
    scale *= 0.5f;
    stack[0] = Stack( 0u, center, scale);
    while(stackPos-- > 0) {
        center = stack[stackPos].center;
		index = stack[stackPos].index;
		scale = stack[stackPos].scale;
        uint voxel_node = texelFetch(u_VoxelData, int(index)).r;
        uint voxel_group_offset = voxel_node >> 16;
        uint voxel_child_mask = (voxel_node & 0x0000FF00u) >> 8u;
        uint voxel_leaf_mask = voxel_node & 0x000000FFu;
        uint accumulated_offset = 0u;

        // Problem here is it loops over every voxel and just picks the first one it encounters which has a collision.
        // However this may not necessarily be the first collision.
        // Therefore we need some way of testing if it is
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
                return calculateLighting(hit);
            } else { //not empty and not a leaf (branch)
            	stack[stackPos++] = Stack(voxel_group_offset+accumulated_offset, new_center, scale*0.5f   );
                accumulated_offset+=1u;
            }
        }
    }
	return vec4(173.0f / 255.0f, 216.0f / 255.0f, 230.0f / 255.0f, 1.0f); // The blue color of the background
}
vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

void main() {
	vec3 cameraDir = vec3(0.0, 0.0, 0.8);
    //vec3 rayDir = cameraDir + v_Pos.x * cameraPlaneU + v_Pos.y * cameraPlaneV;
    vec3 rayDir = cameraDir + vec3(v_Pos);
    vec3 rayPos = vec3(0.0, 0.0, -4.5);
	//vec3 rayPos = vec3(0.0, 0.25 * sin(u_Time * 2.7), -3.4);
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