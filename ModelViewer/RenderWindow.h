#pragma once
#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H
#include <memory>
#include "MainManager.h"

struct GLFWwindow;
class MainManager;

struct MousePosition
{
	double PosX;
	double PosY;
};

class RenderWindow
{
private:
	static std::shared_ptr<RenderWindow> g_pInstance;

	std::shared_ptr<Camera>			m_pMainCamera;
	std::shared_ptr<MainManager>	m_pMainManager;

	MousePosition m_currentMousePosition;

	RenderWindow();
public:
	int		m_nWidth;
	int		m_nHeight;
	bool	m_inputPressState[1024];

	GLFWwindow*	m_pWindow;

	~RenderWindow();
	static RenderWindow& Instance();
	void Run();
protected:
	void Update();
	void Render();

	void KeyEvent(int key, int scancode, int action, int mods);
	void ResizeEvent(int width, int height);
	void MouseButtonEvent(int button, int action, int mods);
	void MouseMoveEvent(GLFWwindow* window, double xPos, double yPos);
	void MouseScrollCallBack(GLFWwindow* window, double xOffset, double yOffset);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void reshape(GLFWwindow* window, int width, int height);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void mouse_position_callback(GLFWwindow* window, double xPos, double yPos);
	static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};
#endif // RENDER_WINDOW_H
