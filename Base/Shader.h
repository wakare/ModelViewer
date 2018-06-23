#pragma once
#include <Gl/glew.h>
#include <iostream>
enum class ShaderType
{
	eVertexShader = GL_VERTEX_SHADER,
	eFragmentShader = GL_FRAGMENT_SHADER,
	eInvaildShader = -1
};

class Shader
{
public:
	ShaderType m_eShaderType;
	std::string m_sShaderText;

	Shader(ShaderType eShaderType, const char* szShaderPath);
	bool Init(ShaderType eShaderType, const char* szShaderPath);
	void UnInit();
};