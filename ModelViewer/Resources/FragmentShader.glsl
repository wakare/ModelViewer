#version 330 core

out vec4 outputColor;
in vec3 outColor;

void main()
{
	outputColor = vec4(outColor, 1.0);
}