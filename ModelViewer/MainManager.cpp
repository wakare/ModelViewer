#include <assert.h>
#include <mutex>
#include "MainManager.h"

std::shared_ptr<MainManager> MainManager::g_pInstance = nullptr;

MainManager::MainManager(): 
	m_pFontRenderer(nullptr),  m_pRenderMgr(nullptr), m_pSceneMgr(nullptr), m_renderType(RenderType::FILLED)
{
	
}

std::shared_ptr<MainManager> MainManager::GetInstance()
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, [&] { g_pInstance = std::shared_ptr<MainManager>(new MainManager()); });
	return g_pInstance;
}

void MainManager::_initFileData(const char* szFileDataPath)
{
	assert(szFileDataPath);

	STLParser stlParser;
	std::shared_ptr<Mesh> pMesh = std::make_shared<Mesh>();

	if (!stlParser.LoadDateFromFile(szFileDataPath))
		return;

	if(!pMesh->InitFromSTLData(stlParser.m_pTriangleData, stlParser.m_nTriangleCount))
		return;

	m_pSceneMgr->AddSceneObject(pMesh);
}

bool MainManager::_initMemberVariable()
{
	m_pSceneMgr = std::make_shared<SceneMgr>();
	assert(m_pSceneMgr);

	m_pFontRenderer = std::make_shared<FontRenderer>();
	assert(m_pFontRenderer);

	m_pRenderMgr = std::make_shared<RenderMgr>(m_currentTransform, RenderWindow::Instance().m_pWindow);
	assert(m_pRenderMgr);

	m_renderType = RenderType::FILLED;

	return true;
}

void MainManager::_initSceneData()
{
	_initFileData("dental.stl");
	m_pSceneMgr->InitSpaceAccelerateStruct();

	m_pSceneMgr->GetMainCamera()->SetCameraPosition(Vector3(0.0f, 0.0f, 20.0f));
	m_pSceneMgr->GetMainCamera()->LookAtTarget(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
	if (m_pSceneMgr->m_pProjTransformStruct == nullptr)
	{
		float fAspect = static_cast<float>(RenderWindow::Instance().m_nWidth) / static_cast<float>(RenderWindow::Instance().m_nHeight);
		m_pSceneMgr->m_pProjTransformStruct = std::make_shared<ProjTransformStruct>(90.0f * PI / 180.0f, fAspect, 0.1f, 1000.0f);
	}

	m_currentTransform.viewTransform = m_pSceneMgr->GetMainCamera()->GetViewTransform();
	m_currentTransform.projTransform = m_pSceneMgr->m_pProjTransformStruct->projTransform;
}

void MainManager::Update()
{
	m_pSceneMgr->Update();
	m_currentTransform.viewTransform = m_pSceneMgr->GetMainCamera()->GetViewTransform();
	m_pFontRenderer->Update();
	m_pRenderMgr->Update();
}

void MainManager::StartRun()
{
	RenderWindow::Instance().Run();
}

void MainManager::ReSize(int nWidth, int nHeight)
{
	m_pSceneMgr->ReSize(nWidth, nHeight);
}

void MainManager::AddString(const char * szString)
{
	if (m_pFontRenderer)
	{
		m_pFontRenderer->AddString(szString);
	}
}

void MainManager::ClearString()
{
	if (m_pFontRenderer)
	{
		m_pFontRenderer->Clear();
	}
}

void MainManager::Render()
{
	if (m_pRenderMgr)
	{
		m_pRenderMgr->Render();
	}
}

void MainManager::MouseClick(double posX, double posY)
{
	m_pSceneMgr->MouseClick(posX, posY);
}

bool MainManager::Init()
{
	// Init base data 
	_initMemberVariable();
	_initSceneData();

	if (!m_pSceneMgr->Init())
	{
		Logger::Log(LogType::ERR, "Init sceneMgr failed!");
		return false;
	}

	m_pFontRenderer->AddString("I Love BZ");

	return true;
}

void MainManager::AfterInit()
{
	m_pSceneMgr->AfterInit();
	m_pFontRenderer->AfterInit();
}

const std::shared_ptr<SceneMgr> MainManager::GetSceneMgr()
{
	return m_pSceneMgr;
}

std::shared_ptr<RenderMgr> MainManager::GetRenderMgr()
{
	return m_pRenderMgr;
}

std::shared_ptr<FontRenderer> MainManager::GetFontRenderer()
{
	return m_pFontRenderer;
}

const SceneTransfrom & MainManager::GetCurrentTransform()
{
	return m_currentTransform;
}
