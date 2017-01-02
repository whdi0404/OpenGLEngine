#include "stdafx.h"
#include "Time.h"


Time::Time()
{
	LARGE_INTEGER freq;
	frequency = QueryPerformanceFrequency(&freq);
	frequency = (double)freq.QuadPart;

	QueryPerformanceCounter(&LI_nowTime);

	startTime = LI_nowTime.QuadPart / frequency;
	lastTime = startTime;
	
	deltaTime = 0;
	timeScale = 1;
}


Time::~Time()
{
}

double Time::GetNowTimeSinceStart()
{
	QueryPerformanceCounter(&LI_nowTime);
	return (LI_nowTime.QuadPart / frequency) - startTime;
}

void Time::Update()
{
	QueryPerformanceCounter(&LI_nowTime);
	deltaTime = (LI_nowTime.QuadPart - LI_lastTime.QuadPart) / frequency;
	LI_lastTime = LI_nowTime;
}
