#include <assert.h>
#include "TrianglePrimitive.h"
#include "FontRenderer.h"
#include "Logger.h"
#include "GLFontObjectImpl.h"
#include "MainManager.h"

FontRenderer::FontRenderer()
	:m_nTextFieldWidth(200), m_nTextFieldHeight(400), m_nFontWidth(10), m_nFontHeight(15), m_nLineHeight(11), 
	m_nCurrentLinesCount(0), m_nWindowWidth(800), m_nWindowHeight(600), m_bIsOpenLogRender(false)
{
	if (!Init())
	{
		Logger::Log(LogType::ERR, "FontRenderer init failed.");
		return;
	}
}

bool FontRenderer::Init()
{
	if (!m_importer.LoadFontFile("Resources/Font/ASCII.bmp"))
	{
		Logger::Log(LogType::ERR, "Load texture failed");
	}

	m_pFontSubProcess = std::make_shared<FontSubProcess>(m_importer.m_nTextureWidth, m_importer.m_nTextureHeight, m_importer.m_pTextureData);
	assert(m_pFontSubProcess);

	m_pFontSubProcess->EnableOrDisableGLState(GL_DEPTH, true);
	m_pFontSubProcess->AddStateFuncClosure(std::make_pair(glDepthFunc, FuncVar(GL_ALWAYS, 0)));
	m_pFontSubProcess->EnableOrDisableGLState(GL_BLEND, true);
	m_pFontSubProcess->AddStateFuncClosure(std::make_pair(glBlendFunc, FuncVar(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)));
	return true;
}

void FontRenderer::AfterInit()
{
	MainManager::GetInstance()->GetRenderMgr()->RegisterSubProcess(RenderSubProcessLevel::LEVEL_2, m_pFontSubProcess);
}

void FontRenderer::AddString(std::string sText)
{
	FontMesh fontMesh(static_cast<int>(sText.length()) * 2, m_importer.m_nTextureWidth, m_importer.m_nTextureHeight, m_importer.m_pTextureData);

	for (int i = 0; i < sText.length(); i++)
	{
		auto& temp = sText[i];
		int nIndex = static_cast<int>(temp);
		assert(0 <= nIndex && nIndex < 256);
		auto fontData = m_importer.m_fontVec[nIndex];

		int nStartX = m_nFontWidth * i;
		int nEndX = nStartX + m_nFontWidth;

		float fStartX = static_cast<float>(nStartX) / m_nWindowWidth;
		float fEndX = static_cast<float>(nEndX) / m_nWindowWidth;

		fStartX = 2 * fStartX - 1.0f;
		fEndX = 2 * fEndX - 1.0f;

		int nStartY = m_nCurrentLinesCount * m_nLineHeight;
		int nEndY = nStartY + m_nFontHeight;

		float fStartY = static_cast<float>(nStartY) / m_nWindowHeight;
		float fEndY = static_cast<float>(nEndY) / m_nWindowHeight;

		fStartY = 1.0f - 2 * fStartY;
		fEndY = 1.0f - 2 * fEndY;

		std::shared_ptr<TrianglePrimitive> triangle1 = std::make_shared<TrianglePrimitive>();
		std::shared_ptr<TrianglePrimitive> triangle2 = std::make_shared<TrianglePrimitive>();

		triangle1->m_vertices[0][0] = fStartX;
		triangle1->m_vertices[0][1] = fEndY;
		triangle1->m_vertices[0][2] = 0;
		triangle1->m_UVs[0][0] = fontData.Umin;
		triangle1->m_UVs[0][1] = fontData.Vmax;

		triangle1->m_vertices[1][0] = fEndX;
		triangle1->m_vertices[1][1] = fStartY;
		triangle1->m_vertices[1][2] = 0;
		triangle1->m_UVs[1][0] = fontData.Umax;
		triangle1->m_UVs[1][1] = fontData.Vmin;

		triangle1->m_vertices[2][0] = fStartX;
		triangle1->m_vertices[2][1] = fStartY;
		triangle1->m_vertices[2][2] = 0;
		triangle1->m_UVs[2][0] = fontData.Umin;
		triangle1->m_UVs[2][1] = fontData.Vmin;

		triangle2->m_vertices[0][0] = fStartX;
		triangle2->m_vertices[0][1] = fEndY;
		triangle2->m_vertices[0][2] = 0;
		triangle2->m_UVs[0][0] = fontData.Umin;
		triangle2->m_UVs[0][1] = fontData.Vmax;

		triangle2->m_vertices[1][0] = fEndX;
		triangle2->m_vertices[1][1] = fEndY;
		triangle2->m_vertices[1][2] = 0;
		triangle2->m_UVs[1][0] = fontData.Umax;
		triangle2->m_UVs[1][1] = fontData.Vmax;

		triangle2->m_vertices[2][0] = fEndX;
		triangle2->m_vertices[2][1] = fStartY;
		triangle2->m_vertices[2][2] = 0;
		triangle2->m_UVs[2][0] = fontData.Umax;
		triangle2->m_UVs[2][1] = fontData.Vmin;

		fontMesh.m_p3DObjectVec.push_back(triangle1);
		fontMesh.m_p3DObjectVec.push_back(triangle2);
	}

	m_fontMeshVec.push_back(fontMesh);

	auto& primitiveData = fontMesh.GetPrimitiveData();
	std::shared_ptr<GLFontObjectImpl> pFontObject = std::make_shared<GLFontObjectImpl>
		(primitiveData.m_nVertexCount, primitiveData.m_nFloatCountPerVertex, primitiveData.m_pfVertexData);
	
	if (!pFontObject->Init())
	{
		assert(false);
		Logger::Log(LogType::ERR, "font object init failed.");
		return;
	}

	m_pFontSubProcess->AddGLObject(pFontObject);

	m_nCurrentLinesCount++;
}

void FontRenderer::Update()
{
	m_pFontSubProcess->SetEnableRender(m_bIsOpenLogRender);
	
	for (auto fontMesh : m_fontMeshVec)
	{
		fontMesh.Update();
	}
}
void FontRenderer::Clear()
{
	m_fontMeshVec.clear();
	m_pFontSubProcess->ClearGLObjects();
	m_nCurrentLinesCount = 0;
}

void FontRenderer::OpenLog()
{
	m_bIsOpenLogRender = true;
}

void FontRenderer::CloseLog()
{
	m_bIsOpenLogRender = false;
}

void FontRenderer::OpenOrCloseLog()
{
	if (m_bIsOpenLogRender)
		CloseLog();
	else
		OpenLog();
}
