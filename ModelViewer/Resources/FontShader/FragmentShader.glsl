#version 330 core

out vec4 outputColor;
in vec2 texCoord;
uniform sampler2D fontTexture;

void main()
{
	vec4 texColor = texture(fontTexture, texCoord);
	if (texColor.w > 0.0)
		outputColor = texColor;
	else
		outputColor = vec4(0.0, 0.0, 0.0, 0.0);
	//outputColor = texColor;
}