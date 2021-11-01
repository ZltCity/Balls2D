#include "timer.hpp"

namespace b2
{

Timer::Timer() : last(clock::now())
{}

float Timer::getDeltaMs(bool reset)
{
	using Duration = std::chrono::duration<float, std::milli>;

	clock::time_point now = clock::now();
	Duration delta = now - this->last;

	if (reset)
		this->last = now;

	return delta.count();
}

uint64_t Timer::getTimestamp()
{
	return clock::now().time_since_epoch().count();
}

} // namespace b2-core
