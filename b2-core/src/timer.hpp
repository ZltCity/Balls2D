#pragma once

#include <chrono>

namespace b2
{

class Timer
{
public:
	Timer();

	float getDeltaMs(bool reset = true);

private:
	using clock = std::chrono::high_resolution_clock;

	clock::time_point last;
};

} // namespace b2
