#version 410 core

layout(location = 0) in vec3 a_Pos;

uniform mat4 u_OrthoMVP;

out vec4 v_Pos;

void main()
{
	gl_Position = u_OrthoMVP * vec4(a_Pos, 1.0f);
	v_Pos = vec4(a_Pos, 1.0f);
}