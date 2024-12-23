#include "Timer.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	frequency = SDL_GetPerformanceFrequency();
	startTime = SDL_GetPerformanceCounter();
}

double Timer::ReadMs() const
{
	return ((double)(SDL_GetPerformanceCounter() - startTime) / frequency * 1000);
}