#version 410 core
layout(location = 0) out vec4 a_Color;

in vec4 v_Pos;

uniform float u_Time;
uniform usamplerBuffer u_VoxelData;

struct Ray {
	vec3 origin, direction, invDirection;
};

struct Node {
	uint parent, index;
};

bool AABBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray ray) {

	vec3 t0 = (boxMin - ray.origin) * ray.invDirection;
	vec3 t1 = (boxMax - ray.origin) * ray.invDirection;
	vec3 tmin = min(t0, t1), tmax = max(t0, t1);

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

	return maxComponent <= minComponent;
}

bool IsNodeLeaf(Node node) {
	uint voxel_leaf_mask = node.parent & 0x000000FFu;
	return (voxel_leaf_mask & (1u << node.index)) != 0u;
}

uint GetLeafMaskBit(Node node, int index)
{
	uint clearmask = 0x00000001;
	uint bit = node.parent >> index;
	return (bit & clearmask);
}

uint GetBranchIndex(Node node)
{
	int count = -1;
	for (int i = 0; i <= node.index; i++)
	{
		if (GetLeafMaskBit(node, i) == 0)
		{
			count++;
		}
	}
	return count;
}

uint GetChildNode(Node node) {
	uint clearmask = 0xFFFF0000;
	uint index = (node.parent & clearmask) >> 16;
	index += GetBranchIndex(node);
	return texelFetch(u_VoxelData, int(index)).r;
}

uint FirstIndex(float tenter, vec3 tm) {
	uint idx = 0;
	if(tenter > tm.x) {
		idx |= 1 << 2;
	}
	if(tenter > tm.y) {
		idx |= 1 << 1;
	}
	if(tenter > tm.z) {
		idx |= 1;
	}
	return idx;
}

// This is gonna have to be modified to handle parent voxel with children as indices
bool traceray(Node node, vec3 t0, vec3 t1) {
	if(t1.x < 0 || t1.y < 0 || t1.z < 0) {
		return false;
	}
	float tenter = max(max(t0.x, t0.y), t0.z);
	if(IsNodeLeaf(node)) {
		// Here we can process this node if it is a leaf
		return true;
	}
	vec3 tm = (t0 + t1) / 2;
	uint childidx = FirstIndex(tenter, t0);
	Node childNode;

	bool exitnode = false;
	while(!exitnode) {
		childNode.parent = GetChildNode(node);
		childNode.index = childidx;
		// Select child T values
		
	}
	return false;
}

vec4 raycast(const Ray ray) {
	
	float boxScale = 1.0f;
	vec3 boxCenter = vec3(0.0f);
	vec3 boxMin = boxCenter - boxScale;
	vec3 boxMax = boxCenter + boxScale;

	// T0 and T1 of root node
	vec3 t0 = (boxMin - ray.origin) * ray.invDirection;
	vec3 t1 = (boxMax - ray.origin) * ray.invDirection;

	// AABBoxIntersect test
	vec3 tmin = min(t0, t1), tmax = max(t0, t1);

	float maxComponent = max(max(tmin.x, tmin.y), tmin.z);
	float minComponent = min(min(tmax.x, tmax.y), tmax.z);

	// Ray hit root node (test result)
	if(!(maxComponent <= minComponent)) {
		return vec4(0.0f); // return black if no hit
	}
	else {
	    Node rootNode;
		rootNode.parent = texelFetch(u_VoxelData, 0).r;
		float tenter = max(max(t0.x, t0.y), t0.z);
		rootNode.index = FirstIndex(tenter, t0);

		traceray(rootNode, t0, t1); 
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