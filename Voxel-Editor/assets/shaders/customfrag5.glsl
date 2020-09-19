#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;

// Camera
uniform mat4 u_PerspectiveViewMatrix;
uniform float u_CameraRadius;

uniform float u_Time;
uniform usamplerBuffer u_VoxelData;
uniform samplerBuffer u_ColorData;

// Sample the envmap in multiple places and pick the highest valued one. Not really physically accurate if not 1
#define SKY_SAMPLES 1
// How many directions to sample the lighting & BRDF at
// Setting it to 0 disables the envmap lighting and switches to a constant light
#define MAT_SAMPLES 0

// Set this to 1 for a liquid-like animation
#define ANIMATE 0
// Try turning that on and this off to see the animation more clearly
#define CAMERA_MOVEMENT 1

// Enable this to see the exact raymarched model
#define MARCH 0

// The size of the scene. Don't change unless you change the distance function
const float root_size = 3.;
// The resolution, in octree levels. Feel free to play around with this one
const int levels = 8;

// The maximum iterations for voxel traversal. Increase if you see raymarching-like
//	hole artifacts at edges, decrease if it's too slow
const int MAX_ITER = 256;
// Note, though, that the fake AO might look weird if you change it much

// These are parameters for the Mandelbulb, change if you want. Higher is usually slower
const float Power = 4.;
const float Bailout = 1.5;
const int Iterations = 6;


#define PI 3.1415926535
const float IPI = 1./PI;
const float R2PI = sqrt(2./PI);

struct Material {
    vec3 base_color;
    float roughness;
};

float sqr(float x) { return x*x; }
#define saturate(x) clamp(x,0.,1.)

vec2 isect(in vec3 pos, in float size, in vec3 ro, in vec3 rd, out vec3 tmid, out vec3 tmax, inout vec3 normal) {
    vec3 mn = pos - 0.5 * size;
    vec3 mx = mn + size;
    vec3 t1 = (mn-ro) / rd;
    vec3 t2 = (mx-ro) / rd;
    vec3 tmin = min(t1, t2);
    tmax = max(t1, t2);
    normal = -sign(rd) * step(tmin.yzx, tmin.xyz) * step(tmin.zxy, tmin.xyz);
    tmid = (pos-ro)/rd; // tmax;

    return vec2(max(tmin.x, max(tmin.y, tmin.z)), min(tmax.x, min(tmax.y, tmax.z)));
}

struct ST {
    uint parent;
    vec3 pos;
	int scale; // size = root_size * exp2(float(-scale));
    vec3 idx;
    float h;
} stack[levels];

int stack_ptr = 0; // Next open index
void stack_reset() { stack_ptr = 0; }
void stack_push(in ST s) { stack[stack_ptr++] = s; }
ST stack_pop() { return stack[--stack_ptr]; }
bool stack_empty() { return stack_ptr == 0; }

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

// The actual ray tracer, based on https://research.nvidia.com/publication/efficient-sparse-voxel-octrees
bool trace(in vec3 ro, in vec3 rd, out vec2 t, out vec3 pos, out int iter, out float size, out vec3 normal, out uint didx, out uint parentptr) {
    stack_reset();
    
    //-- INITIALIZE --//
    
    int scale = 0;
    size = root_size;
    vec3 root_pos = vec3(0);
    pos = root_pos;
    vec3 tmid;
    vec3 tmax;
    bool can_push = true;
    //float d;
    uint parent = texelFetch(u_VoxelData, 0).r;
    t = isect(pos, size, ro, rd, tmid, tmax, normal);

    if(t.x >= t.y) {
        return false;
    }
    float h = t.y;
    
    // Initial push, sort of
    // If the minimum is before the middle in this axis, we need to go to the first one (-rd)
    vec3 idx = mix(-sign(rd), sign(rd), lessThanEqual(tmid, vec3(t.x)));
    vec3 temp = idx + vec3(1.0f);
    didx = 0u | uint(idx.x > 0.0) << 2 | uint(idx.y > 0.0) << 1 | uint(idx.z > 0.0);
    scale = 1;
    size *= 0.5;
    pos += 0.5 * size * idx;
    
    iter = MAX_ITER;
    while (iter --> 0) { // `(iter--) > 0`; equivalent to `for(int i=128;i>0;i--)`
        t = isect(pos, size, ro, rd, tmid, tmax, normal);
        
        // Voxel exists:
		uint voxel_valid_mask = (parent & 0x0000FF00u) >> 8u;
		if((voxel_valid_mask & (1u << didx)) != 0u) {
           
			uint voxel_leaf_mask = parent & 0x000000FFu;
			if((voxel_leaf_mask & (1u << didx)) != 0u) 
                return true; // Filled leaf
            
            if (can_push) {
                //-- PUSH --//
                
                if (t.y < h) // Don't add this if we would leave the parent voxel as well
                    stack_push(ST(parent, pos, scale, idx, h));
                h = t.y;
                scale++;
                size *= 0.5;
                idx = mix(-sign(rd), sign(rd), lessThanEqual(tmid, vec3(t.x)));
                vec3 temp = idx + vec3(1.0f);
                didx = 0u | uint(idx.x > 0.0) << 2 | uint(idx.y > 0.0) << 1 | uint(idx.z > 0.0);
                pos += 0.5 * size * idx;

                // Get new parent:
                parentptr = (parent >> 16) + GetBranchIndex(parent, didx);
                parent = texelFetch(u_VoxelData, int(parentptr)).r;

                continue;
            }
        }
        
        //-- ADVANCE --//
        
        // Advance for every direction where we're hitting the middle (tmax = tmid)
        vec3 old = idx;
        idx = mix(idx, sign(rd), equal(tmax, vec3(t.y)));
        vec3 temp = idx + vec3(1.0f);
        didx = 0u | uint(idx.x > 0.0) << 2 | uint(idx.y > 0.0) << 1 | uint(idx.z > 0.0);
        pos += mix(vec3(0.), sign(rd), notEqual(old, idx)) * size;
        
        // If idx hasn't changed, we're at the last child in this voxel
        if (idx == old) {
            //-- POP --//
            if (stack_empty() || scale == 0) return false; // We've investigated every voxel on the ray
            
            ST s = stack_pop();
            parent = s.parent;
            pos = s.pos;
            scale = s.scale;
            size = root_size * exp2(float(-scale));
			idx = s.idx;
            h = s.h;
            
            can_push = false; // No push-pop inf loops
        } else can_push = true; // We moved, we're good
    }
    
    return false;
}

// -----------------------------------------------------------------------------------------

// https://shaderjvo.blogspot.com/2011/08/van-ouwerkerks-rewrite-of-oren-nayar.html
vec3 oren_nayar(vec3 from, vec3 to, vec3 normal, Material mat) {
    // Roughness, A and B
    float roughness = mat.roughness;
    float roughness2 = roughness * roughness;
    vec2 oren_nayar_fraction = roughness2 / (roughness2 + vec2(0.33, 0.09));
    vec2 oren_nayar = vec2(1, 0) + vec2(-0.5, 0.45) * oren_nayar_fraction;
    // Theta and phi
    vec2 cos_theta = saturate(vec2(dot(normal, from), dot(normal, to)));
    vec2 cos_theta2 = cos_theta * cos_theta;
    float sin_theta = sqrt((1.-cos_theta2.x) * (1.-cos_theta2.y));
    vec3 light_plane = normalize(from - cos_theta.x * normal);
    vec3 view_plane = normalize(to - cos_theta.y * normal);
    float cos_phi = saturate(dot(light_plane, view_plane));
    // Composition
    float diffuse_oren_nayar = cos_phi * sin_theta / max(cos_theta.x, cos_theta.y);
    float diffuse = cos_theta.x * (oren_nayar.x + oren_nayar.y * diffuse_oren_nayar);
    
    return mat.base_color * diffuse;
}


// These bits from https://simonstechblog.blogspot.com/2011/12/microfacet-brdf.html

float schlick_g1(vec3 v, vec3 n, float k) {
    float ndotv = dot(n, v);
    return ndotv / (ndotv * (1. - k) + k);
}

vec3 brdf(vec3 from, vec3 to, vec3 n, Material mat, float ao) {
    float ior = 1.5;
    
    // Half vector
    vec3 h = normalize(from + to);
    
    // Schlick fresnel
    float f0 = (1.-ior)/(1.+ior);
    f0 *= f0;
    float fresnel = f0 + (1.-f0)*pow(1.-dot(from, h), 5.);
    
    // Beckmann microfacet distribution
    float m2 = sqr(mat.roughness);
    float nh2 = sqr(saturate(dot(n,h)));
    float dist = (exp( (nh2 - 1.)
    	/ (m2 * nh2)
    	))
        / (PI * m2 * nh2*nh2);
    
    // Smith's shadowing function with Schlick G1
    float k = mat.roughness * R2PI;
    float geometry = schlick_g1(from, n, k) * schlick_g1(to, n, k);
    
    return saturate((fresnel*geometry*dist)/(4.*dot(n, from)*dot(n, to))
        + ao*(1.-f0)*oren_nayar(from, to, n, mat));
}


// -----------------------------------------------------------------------------------------


// By Dave_Hoskins https://www.shadertoy.com/view/4djSRW
vec3 hash33(vec3 p3)
{
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+19.19);
    return fract((p3.xxy + p3.yxx)*p3.zyx);

}

vec3 shade(vec3 ro, vec3 rd, vec2 t, int iter, vec3 pos) {
    #if MARCH
    
    /*
    // The normal here isn't accurate, the surface is too high-frequency
    float e = 0.0001;
    vec3 eps = vec3(e,0.0,0.0);
	vec3 n = normalize( vec3(
           dist(pos+eps.xyy) - dist(pos-eps.xyy),
           dist(pos+eps.yxy) - dist(pos-eps.yxy),
           dist(pos+eps.yyx) - dist(pos-eps.yyx) ) );
	*/
    
    // This pretends the Mandelbulb is actually a sphere, but it looks okay w/ AO.
    vec3 n = normalize(pos);
    // And this isn't accurate even for a sphere, but it ensures the edges are visible.
    n = faceforward(n,-rd,-n);
    
    #else
    
    // The largest component of the vector from the center to the point on the surface,
    //	is necessarily the normal.
    vec3 n = (ro+rd*t.x - pos);
    n = sign(n) * (abs(n.x) > abs(n.y) ? // Not y
        (abs(n.x) > abs(n.z) ? vec3(1., 0., 0.) : vec3(0., 0., 1.)) :
    	(abs(n.y) > abs(n.z) ? vec3(0., 1., 0.) : vec3(0., 0., 1.)));
    
    #endif
    
    Material mat = Material(vec3(1.,.9,.7), 0.9);

    #if MAT_SAMPLES
    vec3 acc = vec3(0.);
    int j;
    for (j = 0; j < MAT_SAMPLES; j++) {
        vec3 lightDir;
        vec3 lightCol = vec3(0.);
        for (int i = 0; i < SKY_SAMPLES; i++) {
            vec3 d = hash33(0.2*pos+0.5*n+float(i+j*SKY_SAMPLES));
            d = normalize(d);
            vec3 c = sky(d);
            if (length(c) > length(lightCol)) {
                lightCol = c;
                lightDir = d;
            }
        }
        acc +=
            2.*pow(lightCol, vec3(2.2)) * brdf(lightDir, -rd, n, mat, (float(iter)/float(MAX_ITER)));
    }
    return acc / float(j);
	#else
    vec3 lightDir = reflect(rd,n);
    return 2.*brdf(lightDir, -rd, n, mat, (float(iter)/float(MAX_ITER)));
    #endif
}


vec2 rotate2d(vec2 v, float a) {
	float sinA = sin(a);
	float cosA = cos(a);
	return vec2(v.x * cosA - v.y * sinA, v.y * cosA + v.x * sinA);	
}

vec3 calculateLighting(vec3 ro, vec3 rd, vec3 normal, float tenter) {
    vec3 voxelColor = vec3(1.0f, 0.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
	
    // Diffuse
    vec3 lightPos = vec3(-2.0f, 0.0f, 2.0f);
	vec3 hitPos = ro + tenter*rd;
    vec3 lightDir = normalize(lightPos - hitPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
	
    // Result
    vec3 result = (ambient + diffuse) * voxelColor;
    return result;

	//return vec4(1/hit.voxelpos, 1.0f);
}

vec3 getColor(uint voxelIndex, uint parentptr)
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
	return texelFetch(u_ColorData, int(colorIndex)).rgb;
}

void main() {
	vec3 cameraDir = vec3(0.0, 0.0, -1.0);
    vec3 rayDir = cameraDir + vec3(v_Pos);
	vec3 rayOrigin = vec3(0.0f, 0.0f, u_CameraRadius);

	rayOrigin = vec3(vec4(rayOrigin, 1.0f) * u_PerspectiveViewMatrix);
   	rayDir = vec3(vec4(rayDir, 1.0f) * u_PerspectiveViewMatrix);
    rayDir = normalize(rayDir);
    
    vec2 t;
    vec3 pos;
    float size;
    int iter;
    vec3 normal;
    uint didx;
    uint parentptr;
    
    //vec3 col = trace(rayOrigin, rayDir, t, pos, iter, size, normal, didx, parentptr) ? shade(rayOrigin, rayDir, t, iter, pos) : vec3(0.5f);
    vec3 col = trace(rayOrigin, rayDir, t, pos, iter, size, normal, didx, parentptr) ? getColor(didx, parentptr) : vec3(0.5f);
    //vec3 col = trace(rayOrigin, rayDir, t, pos, iter, size, normal) ? calculateLighting(rayOrigin, rayDir, normal, t.x) : vec3(0.5f);

    a_Color = vec4(col,1.0);
}