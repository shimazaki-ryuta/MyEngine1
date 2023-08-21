#pragma once
#include <intsafe.h>
class DeltaTime
{
public:
	static DeltaTime* GetInstance();
	static void GameLoopStart();
	static void FrameStart();
	static float GetFramePerSecond();
	static float GetDeltaTime();

private:
	DeltaTime() = default;
	~DeltaTime() = default;
	DeltaTime(const DeltaTime&) = delete;
	DeltaTime operator=(const DeltaTime&) = delete;
	DWORD frameStart;
	DWORD preFrame;
	float deltaTime = 1.0f/60.0f;
};

