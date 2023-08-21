#include "DeltaTime.h"
#include<Windows.h>
#pragma comment(lib, "winmm.lib")
DeltaTime* DeltaTime::GetInstance()
{
	static DeltaTime instance;
	return &instance;

}

void DeltaTime::GameLoopStart()
{
	GetInstance()->frameStart =timeGetTime();
	GetInstance()->preFrame = timeGetTime();
}

void DeltaTime::FrameStart()
{
	GetInstance()->preFrame = GetInstance()->frameStart;
	GetInstance()->frameStart = timeGetTime();
	
	GetInstance()->deltaTime = float(GetInstance()->frameStart - GetInstance()->preFrame)/1000.0f;
}

float DeltaTime::GetDeltaTime()
{
	return GetInstance()->deltaTime;
}

float DeltaTime::GetFramePerSecond()
{
	return 1.0f/float(GetInstance()->deltaTime);
}