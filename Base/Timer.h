#pragma once

class Timer 
{
private:
	static double g_dblGLFWBeginTime;
	static unsigned long long g_ullTickCountBeginTime;

public:
	static void BeginTimer();
	static double EndTimer();
};

