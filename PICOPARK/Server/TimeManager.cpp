#include "stdafx.h"

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&prevCount));
}

void TimeManager::Update()
{
	uint64_t currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	deltaTime = (currentCount - prevCount) / static_cast<float>(frequency);

	prevCount = currentCount;

	frameCount++;
	frameTime += deltaTime;

	if (frameTime >= 1.f)
	{
		fps = static_cast<uint32_t>(frameCount / frameTime);

		frameTime = 0.f;
		frameCount = 0;
	}

}
