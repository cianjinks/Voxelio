#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;

uniform float u_Time;
// Camera
uniform mat4 u_PerspectiveViewMatrix;
uniform float u_CameraRadius;

uniform usamplerBuffer u_VoxelData;
uniform samplerBuffer u_ColorData;

#define FLT_MAX 3.402823466e+38

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

struct Hit {
	vec3 voxelpos;
	vec3 normal;
	float tenter;
	vec4 color;
};

struct OcStackElement {
	uint parentptr;
	vec3 pos;
	float scale;
};

bool AABBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray ray, out Hit hit) {

	vec3 t0 = (boxMin - ray.origin) / ray.direction;
	vec3 t1 = (boxMax - ray.origin) / ray.direction;
	vec3 tmin = min(t0, t1), tmax = max(t0, t1);

	hit.normal = -sign(ray.direction) * step(tmin.yzx, tmin.xyz) * step(tmin.zxy, tmin.xyz);

    // Normals
    //vec3 center = (boxMin + boxMax) * 0.5;
    //vec3 p = tmin - center; //Center to hit point vector
    //vec3 divisor = (boxMin - boxMax) * 0.5;
    //float bias = 1.000001;
	//
    //hit.normal = normalize(vec3(
    //                        float(int(p.x / abs(divisor.x) * bias)),
    //                        float(int(p.y / abs(divisor.y) * bias)),
    //                        float(int(p.z / abs(divisor.z) * bias))
    //                      ));

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

	hit.tenter = maxComponent;

	return maxComponent <= minComponent;
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

vec4 rayTrace(Ray ray, out Hit hit) {
	
	vec3 pos = vec3(0.0f);
	float scale = 1.0f;

	OcStackElement stack[10];
	int stackptr = 1;

	vec3 minBox = pos - scale;
	vec3 maxBox = pos + scale;

	Hit rootHit;
	// Check root node intersect
	if(!AABBoxIntersect(minBox, maxBox, ray, rootHit)) return vec4(vec3(0.5f), 1.0f);

	uint parent;
	uint parentptr = 0;
	stack[0] = OcStackElement(parentptr, pos, scale*0.5f);

	while(stackptr-- > 0) {
		parentptr = stack[stackptr].parentptr;
		pos = stack[stackptr].pos;
		scale = stack[stackptr].scale;

		parent = texelFetch(u_VoxelData, int(parentptr)).r;
		uint numBranches = 0;

		// Info for checking closest visible voxel
		Hit hits[8];
		float lowestDistance = FLT_MAX;
		int lowestDistanceIdx = 0;
		bool aHit = false;

		// Loop over every child
		for(int child = 0; child < 8; child++) {

			// Voxel is valid
			uint voxel_valid_mask = (parent & 0x0000FF00u) >> 8u;
			if ((voxel_valid_mask & (1u << child)) != 0u) {
				
				vec3 new_center = pos + scale * POS[child];
				vec3 minBox = new_center - scale;
				vec3 maxBox = new_center + scale;
				
				if (AABBoxIntersect(minBox, maxBox, ray, hits[child])){
				
					// Voxel is a leaf
					uint voxel_leaf_mask = parent & 0x000000FFu;
					if ((voxel_leaf_mask & (1u << child)) != 0u) { 
						
						// Check if this is the closest solid leaf voxel, if so set output hit to it
						hits[child].voxelpos = new_center;
						hits[child].color = getColor(child, parentptr);
						float dist = distance(ray.origin, new_center);
						if(dist < lowestDistance) {
							lowestDistance = dist;
							lowestDistanceIdx = child;
							hit = hits[child];
						}
						aHit = true;
					    //return vec4(1.0f,0.0f,0.0f,1.0f);

					} 
					// Voxel is a branch
					else { 
															// *2 because of storing color indices
						stack[stackptr++] = OcStackElement((parent>>16)+numBranches, new_center, scale*0.5f  );
					    numBranches+=1u;
					}
				}
			}
		}
		// Here we decide whether which hit to output:
		if(aHit) {
			return hit.color;
		}
	}

	return vec4(vec3(0.5f), 1.0f);
}

vec4 calculateLighting(Ray ray, Hit hit) {
    vec3 voxelColor = vec3(1.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
	
    // Diffuse
    vec3 lightPos = vec3(-2.0f, 0.0f, 2.0f);
	vec3 hitPos = ray.origin + hit.tenter*ray.direction;
    vec3 lightDir = normalize(lightPos - hitPos);
    float diff = max(dot(hit.normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
	
    // Result
    vec3 result = (ambient + diffuse) * voxelColor;
    return vec4(result, 1.0f);

	//return vec4(1/hit.voxelpos, 1.0f);
}

vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

vec3 calculateRayPos(vec3 position)
{
	return vec3(vec4(position, 1.0f) * u_PerspectiveViewMatrix);
}

void main() {

	// Make cameraDir into a uniform
	vec3 cameraDir = vec3(0.0, 0.0, -1.0);
    vec3 rayDir = cameraDir + vec3(v_Pos);
	vec3 rayOrigin = vec3(0.0f, 0.0f, u_CameraRadius);
	
	//vec3 cameraDir = vec3(0.0, 0.0, 0.0);
    //vec3 rayDir = vec3(v_Pos);//cameraDir + (v_Pos.x * cameraPlaneU) + (v_Pos.y * cameraPlaneV);
	//vec3 rayOrigin = vec3(0.0f, 0.0f, -u_CameraRadius);

	//rayOrigin.xz = rotate2d(rayOrigin.xz, u_Time);
   	//rayDir.xz = rotate2d(rayDir.xz, u_Time);
	rayOrigin = calculateRayPos(rayOrigin);
	rayDir = calculateRayPos(rayDir);
    //rayDir = normalize(rayDir);

	Ray ray;
	ray.direction = rayDir;
	ray.origin = rayOrigin;

	Hit outHit;
	vec4 color = rayTrace(ray, outHit);
	a_Color = color;
}