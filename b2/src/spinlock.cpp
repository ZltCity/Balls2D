#include "spinlock.hpp"

namespace b2
{

void SpinLock::lock()
{
	while (this->flag.test_and_set(std::memory_order_acquire))
		;
}

void SpinLock::unlock()
{
	this->flag.clear(std::memory_order_release);
}

bool SpinLock::try_lock()
{
	return !this->flag.test_and_set();
}

} // namespace b2
