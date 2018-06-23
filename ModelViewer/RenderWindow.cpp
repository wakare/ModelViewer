#include <mutex>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "RenderWindow.h"

// Init static class variable
std::shared_ptr<RenderWindow> RenderWindow::g_pInstance = nullptr;

void RenderWindow::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (g_pInstance.get()) 
		g_pInstance->KeyEvent(key, scancode, action, mods);
}

void RenderWindow::reshape(GLFWwindow* window, int width, int height)
{
	if (g_pInstance.get()) 
		g_pInstance->ResizeEvent(width, height);
}

void RenderWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (g_pInstance.get()) 
		g_pInstance->MouseButtonEvent(button, action, mods);
}

void RenderWindow::mouse_position_callback(GLFWwindow * window, double xPos, double yPos)
{
	if (g_pInstance.get()) 
		g_pInstance->MouseMoveEvent(window, xPos, yPos);
}

void RenderWindow::scroll_callback(GLFWwindow * window, double xOffset, double yOffset)
{
	if (g_pInstance.get()) 
		g_pInstance->MouseScrollCallBack(window, xOffset, yOffset);
}

RenderWindow& RenderWindow::Instance()
{
	static std::once_flag onceFlag;
	std::call_once(onceFlag, [&] { g_pInstance = std::shared_ptr<RenderWindow>(new RenderWindow()); });
	return *g_pInstance;
}

RenderWindow::RenderWindow()
	:m_pWindow(0), m_nWidth(1024), m_nHeight(768), m_pMainCamera(nullptr), m_pMainManager(nullptr)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "Model Viewer", NULL, NULL);

	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(m_pWindow);
		m_pWindow = 0;
	}

	glfwSetFramebufferSizeCallback(m_pWindow, reshape);
	glfwSetMouseButtonCallback(m_pWindow, mouse_button_callback);
	glfwSetCursorPosCallback(m_pWindow, mouse_position_callback);
	glfwSetKeyCallback(m_pWindow, key_callback);
	glfwSetScrollCallback(m_pWindow, scroll_callback);
}

RenderWindow::~RenderWindow()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void RenderWindow::Run()
{
	if (!m_pWindow)
		return;

	m_pMainManager = MainManager::GetInstance();
	m_pMainCamera = m_pMainManager->GetSceneMgr()->GetMainCamera();

	while (!glfwWindowShouldClose(m_pWindow))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Update();
		RenderOneFrame();
		
		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}
}

void RenderWindow::Update()
{
	m_pMainManager->Update();
}

void RenderWindow::RenderOneFrame()
{
	m_pMainManager->RenderOneFrame();
}

void RenderWindow::KeyEvent(int key, int scancode, int action, int mods)
{
	float cameraMoveSpeed = m_pMainCamera->m_fMoveSpeed;
	Vector3 cameraMovement = { 0.0, 0.0, 0.0 };

	// Update keyPressState array
	if (action == GLFW_PRESS)
		m_inputPressState[key] = true;

	if (action == GLFW_RELEASE)
		m_inputPressState[key] = false;

	// Switch WIREFRAME_MODE and FILL_MODE
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (m_pMainManager->m_renderType == RenderType::WIREFRMAE)
			m_pMainManager->m_renderType = RenderType::FILLED;

		else
			m_pMainManager->m_renderType = RenderType::WIREFRMAE;
	}

	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		m_pMainManager->ClearString();
	}

	// Control camera movement
	if (m_inputPressState[GLFW_KEY_UP])
		cameraMovement.fY += cameraMoveSpeed;

	if (m_inputPressState[GLFW_KEY_DOWN])
		cameraMovement.fY -= cameraMoveSpeed;

	if (m_inputPressState[GLFW_KEY_LEFT])
		cameraMovement.fX -= cameraMoveSpeed;

	if (m_inputPressState[GLFW_KEY_RIGHT])
		cameraMovement.fX += cameraMoveSpeed;

	if (m_inputPressState[GLFW_KEY_Z])
		cameraMovement.fZ += cameraMoveSpeed;

	if (m_inputPressState[GLFW_KEY_X])
		cameraMovement.fZ -= cameraMoveSpeed;

	if (cameraMovement.fX != 0.0 || cameraMovement.fY != 0.0f || cameraMovement.fZ != 0.0f)
		m_pMainCamera->SelfMove(cameraMovement);
}

void RenderWindow::ResizeEvent(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pMainManager->ReSize(m_nWidth, m_nHeight);
}

void RenderWindow::MouseButtonEvent(int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		m_inputPressState[button] = true;

	if (action == GLFW_RELEASE)
		m_inputPressState[button] = false;

	// Handle left click
	if (m_inputPressState[GLFW_MOUSE_BUTTON_LEFT])
		m_pMainManager->MouseClick(m_currentMousePosition.PosX, m_currentMousePosition.PosY);
}

void RenderWindow::MouseMoveEvent(GLFWwindow * window, double xPos, double yPos)
{
	static bool bFirstMove = true;
	static double LastPosX = 0.0;
	static double LastPosY = 0.0;

	double deltaMoveX = xPos - LastPosX;
	double deltaMoveY = yPos - LastPosY;

	if (bFirstMove)
	{
		bFirstMove = false;
		return;
	}

	// Handle rotate
	if (m_inputPressState[GLFW_MOUSE_BUTTON_RIGHT])
	{
		if (m_inputPressState[GLFW_KEY_LEFT_CONTROL])
			m_pMainManager->GetSceneMgr()->RotateSceneObject(-deltaMoveX, deltaMoveY);
		else
			m_pMainCamera->SelfRotate(-deltaMoveX, deltaMoveY);
	}

	// Handle move
	if (m_inputPressState[GLFW_MOUSE_BUTTON_MIDDLE])
	{
		Vector3 movement = { 0.0f, 0.0f, 0.0f };
		movement.fX = -static_cast<float>(deltaMoveX) * m_pMainCamera->m_fMoveSpeed;
		movement.fY = static_cast<float>(deltaMoveY) * m_pMainCamera->m_fMoveSpeed;
		m_pMainCamera->SelfMove(movement);
	}

	m_currentMousePosition.PosX = xPos;
	m_currentMousePosition.PosY = yPos;

	LastPosX = xPos;
	LastPosY = yPos;
}

void RenderWindow::MouseScrollCallBack(GLFWwindow * window, double xOffset, double yOffset)
{
	// Mouse scroll in vertical direction
	float fZMove = static_cast<float>(yOffset) * 10.0f;
	m_pMainCamera->SelfMove({ 0.0f, 0.0f, fZMove });
}
