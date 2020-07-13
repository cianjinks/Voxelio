#version 410 core

layout(location = 0) out vec4 a_Color;

in vec3 v_Color;
in vec3 v_Normal;

void main()
{
	a_Color = vec4(v_Color, 1.0f);

}