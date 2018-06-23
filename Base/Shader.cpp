#include "Shader.h"
#include <fstream>
#include <assert.h>

void Shader::UnInit()
{
	m_eShaderType = ShaderType::eInvaildShader;
	m_sShaderText.clear();
}

bool Shader::Init(ShaderType eShaderType, const char* szShaderPath)
{
	assert(szShaderPath);

	m_eShaderType = eShaderType;

	std::ifstream shaderReadStream;
	shaderReadStream.open(szShaderPath, std::ios::in);
	if (shaderReadStream.bad())
	{
		std::cout << "[ERROR] Open shader file failed, szShaderPath = " << szShaderPath << std::endl;
		return false;
	}

	std::string szSourceText((std::istreambuf_iterator<char>(shaderReadStream)),
		std::istreambuf_iterator<char>());
	m_sShaderText = szSourceText;

	shaderReadStream.close();
	return true;
}

Shader::Shader(ShaderType eShaderType, const char* szShaderPath)
{
	bool bResult = Init(eShaderType, szShaderPath);
	assert(bResult);
}