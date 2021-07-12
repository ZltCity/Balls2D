#pragma once

#include <atomic>

namespace b2
{

class SpinLock
{
public:
	SpinLock() = default;
	SpinLock(const SpinLock &) = delete;

	SpinLock &operator=(const SpinLock &) = delete;

	void lock();
	void unlock();
	bool try_lock();

private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

} // namespace b2
