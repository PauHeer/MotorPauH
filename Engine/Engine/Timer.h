#pragma once

#include "SDL2/SDL.h"

class Timer
{
public:
	Timer();

	void Start();
	double ReadMs() const;

private:
	Uint64 startTime;
	Uint64 frequency;
};