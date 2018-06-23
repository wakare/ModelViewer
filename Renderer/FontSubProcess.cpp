#include "FontSubProcess.h"
#include "Logger.h"
#include <assert.h>

bool FontSubProcess::_initTexture()
{
	m_pTextureUnit = std::make_shared<GLTextureUnit>(m_nTexWidth, m_nTexHeight, m_pTextureData, m_pShaderMgr->GetShaderProgram());
	if (m_pShaderMgr == nullptr)
		return false;

	if (!m_pTextureUnit->Init())
		return false;

	return true;
}

bool FontSubProcess::_initShaderMgr()
{
	m_pShaderMgr = std::make_shared<ShaderMgr>();
	if (!m_pShaderMgr->Init("Resources/FontShader/VertexShader.glsl", "Resources/FontShader/FragmentShader.glsl"))
		return false;

	if (!m_pShaderMgr->CompileShader())
		return false;

	if (!m_pShaderMgr->LinkProgram())
		return false;

	return true;
}

bool FontSubProcess::Init()
{
	if (!m_bIsInit)
	{
		if (!_initShaderMgr())
		{
			Logger::Log(LogType::ERR, "Init shader failed.");
			return false;
		}

		if (!_initTexture())
		{
			Logger::Log(LogType::ERR, "Init texture failed.");
			return false;
		}
	}

	m_bIsInit = true;
	return true;
}

void FontSubProcess::Render()
{
	if (!m_bEnableRender)
		return;

	// Set renderType
	glPolygonMode(GL_FRONT_AND_BACK, m_nPolygonMode);

	// Set enable and disable
	for (auto enable : m_enableStateVec)
		glEnable(enable);

	for (auto disable : m_disableStateVec)
		glDisable(disable);

	// Set state function
	for (auto closureObj : m_OneVarStateSetFuncVec)
	{
		closureObj.first(closureObj.second.var1);
	}

	for (auto closureObj : m_TwoVarStateSetFuncVec)
	{
		closureObj.first(closureObj.second.var1, closureObj.second.var2);
	}

	auto program = m_pShaderMgr->GetShaderProgram();
	if (program == 0)
	{
		Logger::Log(LogType::ERR, "Not Init shader program");
		return;
	}
	glUseProgram(program);

	// Active texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pTextureUnit->GetTextureUnit());
	glUniform1i(glGetUniformLocation(program, "fontTexture"), 0);

	for (auto Object : m_GLObjectVec)
	{
		auto VAO = Object->GetVAO();
		if (VAO == 0)
		{
			Logger::Log(LogType::ERR, "Not Init VAO");
			return;
		}
		glBindVertexArray(VAO);
		glDrawArrays(Object->GetPrimitiveType(), 0, Object->GetFloatCountPerVertex() * Object->GetVertexCount());

		glBindVertexArray(0);
	}
	glUseProgram(0);

	for (auto enable : m_enableStateVec)
		glDisable(enable);
}
