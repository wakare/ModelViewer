#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 vertexColor;

out vec3 outColor;
out vec3 outNormal;
out vec3 viewDirection;

void main()
{
	vec4 viewPosition = viewMatrix * modelMatrix * vec4(position.x, position.y, position.z, 1.0);
	gl_Position = projectionMatrix * viewPosition;
	outColor = vertexColor;
	outNormal = normal;
	viewDirection = normalize(-viewPosition).xyz;
}