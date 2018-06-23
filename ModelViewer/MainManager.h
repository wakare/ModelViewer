#pragma once
#include "ShaderMgr.h"
#include "SceneMgr.h"
#include "RenderWindow.h"
#include "Logger.h"
#include "STLParser.h"
#include "FontRenderer.h"
#include "RenderMgr.h"

class RenderWindow;

enum class RenderType
{
	WIREFRMAE = 1,
	FILLED = 2
};

class MainManager
{
private:
	static std::shared_ptr<MainManager> g_pInstance;
	
	std::shared_ptr<SceneMgr> m_pSceneMgr;
	std::shared_ptr<FontRenderer> m_pFontRenderer;
	std::shared_ptr<RenderMgr> m_pRenderMgr;
	SceneTransfrom  m_currentTransform;

	MainManager();
	void _initFileData(const char*);
	bool _initMemberVariable();
	void _initSceneData();
public:
	RenderType m_renderType;
	
	static std::shared_ptr<MainManager> GetInstance();

	void MouseClick(double posX, double posY);
	bool Init();
	void AfterInit();
	void Update();
	void Render();
	void StartRun();
	void ReSize(int nWidth, int nHeight);
	void AddString(const char* szString);
	void ClearString();

	const std::shared_ptr<SceneMgr> GetSceneMgr();
	std::shared_ptr<RenderMgr> GetRenderMgr();
	std::shared_ptr<FontRenderer> GetFontRenderer();
	const SceneTransfrom& GetCurrentTransform();
};