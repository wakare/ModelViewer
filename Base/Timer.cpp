#include <windows.h>
#include "Timer.h"
#include "GLFW/glfw3.h"

double Timer::g_dblGLFWBeginTime = 0.0;
unsigned long long Timer::g_ullTickCountBeginTime = 0ull;

void Timer::BeginTimer()
{
	g_dblGLFWBeginTime = glfwGetTime();
	if (g_dblGLFWBeginTime == 0.0f)
		g_ullTickCountBeginTime = GetTickCount64();
}

double Timer::EndTimer()
{
	if (g_dblGLFWBeginTime != 0.0f)
	{
		double deltaTime = glfwGetTime() - g_dblGLFWBeginTime;
		g_dblGLFWBeginTime = 0.0f;

		return deltaTime;
	}

	else
	{
		unsigned long long deltaTime = GetTickCount64() - g_ullTickCountBeginTime;
		g_ullTickCountBeginTime = 0ull;

		return (deltaTime / 1000.0f);  
	}
}
