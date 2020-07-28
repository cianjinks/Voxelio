#version 410 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_MVP;

out vec4 v_Pos;

void main()
{
	gl_Position = u_MVP * vec4(a_Pos, 1.0f);
	v_Pos =  u_MVP * vec4(a_Pos, 1.0f);
}