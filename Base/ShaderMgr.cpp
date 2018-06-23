#include "ShaderMgr.h"
#include <fstream>
#include <assert.h>

ShaderMgr::ShaderMgr(const char* szVertexShaderPath, const char* szFragmentShaderPath)
{
	m_szShaderPathVec.clear();
	m_shaderObjects.clear();

	if (!Init(szVertexShaderPath, szFragmentShaderPath))
		return;
}

ShaderMgr::ShaderMgr()
{

}

ShaderMgr::~ShaderMgr()
{
	UnInit();
}

bool ShaderMgr::InitShaderPath(const char* szVertexShaderPath, const char* szFragmentShaderPath)
{
	m_szShaderPathVec.insert(std::make_pair(ShaderType::eVertexShader, szVertexShaderPath));
	m_szShaderPathVec.insert(std::make_pair(ShaderType::eFragmentShader, szFragmentShaderPath));

	return true;
}

bool ShaderMgr::InitShaderObject()
{
	for (auto szShaderPath : m_szShaderPathVec)
	{
		m_shaderObjects.insert(
			std::make_pair(szShaderPath.first, std::make_shared<Shader>(szShaderPath.first, szShaderPath.second)));
	}

	return true;
}

bool ShaderMgr::Init(const char* szVertexShaderPath, const char* szFragmentShaderPath)
{
	if (!InitShaderPath(szVertexShaderPath, szFragmentShaderPath))
		return false;

	if (!InitShaderObject())
		return false;

	return true;
}

bool ShaderMgr::UnInit()
{
	m_shaderObjects.clear();
	return true;
}

bool ShaderMgr::CompileShader()
{
	for (auto shaderPair : m_shaderObjects)
	{
		GLenum shaderType = static_cast<std::underlying_type<ShaderType>::type>(shaderPair.first);
		auto shader = glCreateShader(shaderType);
		GLint success = GL_FALSE;

		const GLchar* ShaderSource = shaderPair.second->m_sShaderText.c_str();
		const GLchar* shaderArray[1] = { ShaderSource };

		glShaderSource(shader, 1, shaderArray, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		m_linkedShaderProgram.push_back(shader);

		if (bIsOpenCompileErrorLog)
		{
			GLchar infoLog[512];

			// Print Compile Error
			if (success == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		}

		assert(success);
		if (!success)
			return false;
	}
	
	return true;
}

bool ShaderMgr::LinkProgram()
{
	m_shaderProgram = glCreateProgram();

	GLint success = GL_FALSE;
	GLchar infoLog[512];

	if (m_linkedShaderProgram.size() <= 0)
		return false;

	// Link shader program
	for (auto shader : m_linkedShaderProgram)
	{
		glAttachShader(m_shaderProgram, shader);
	}
	glLinkProgram(m_shaderProgram);
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);

	if (!bIsOpenCompileErrorLog)
		return success == GL_TRUE;

	// Print Link Error
	if (!success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
	}

	assert(success);
	return success == GL_TRUE;
}

GLuint ShaderMgr::GetShaderProgram()
{
	return m_shaderProgram;
}

void ShaderMgr::SetUniformVariableMatrix4(GLuint shaderProgram, const char * uniformName, Matrix4 transformMatrix)
{
	GLint variableLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(variableLocation, 1, GL_TRUE, transformMatrix.ToFloatArray());
}

void ShaderMgr::SetUniformVariableFloat1(GLuint shaderProgram, const char * uniformName, GLfloat value)
{
	GLint variableLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUseProgram(shaderProgram);
	glUniform1f(variableLocation, value);
}

void ShaderMgr::SetUniformVariableFloat3(GLuint shaderProgram, const char * uniformName, Vector3 value)
{
	GLint variableLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUseProgram(shaderProgram);
	glUniform3f(variableLocation, value.fX, value.fY, value.fZ);
}

void ShaderMgr::SetUniformVariableInt1(GLuint shaderProgram, const char * uniformName, GLboolean value)
{
	GLint variableLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUseProgram(shaderProgram);
	glUniform1i(variableLocation, value);
}


