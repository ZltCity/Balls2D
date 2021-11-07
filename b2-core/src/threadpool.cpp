#include <cassert>

#include <b2/logger.hpp>

#include "threadpool.hpp"

namespace b2
{

ThreadPool::ThreadPool(size_t workerCount) : workers(workerCount), alarm(false), alive(true)
{
	info(fmt::format("Threads count: {}", workers.size()));

	for (ThreadPtr &thread : workers)
		thread = std::make_unique<std::thread>(workerRoutine, this);
}

ThreadPool::~ThreadPool()
{
	assert(alive);

	{
		std::lock_guard lock(aliveLock);

		alive = false;
		alarm.test_and_set();
		alarm.notify_all();
	}

	for (ThreadPtr &thread : workers)
		thread->join();
}

void ThreadPool::workerRoutine(ThreadPool *self)
{
	while (true)
	{
		auto task = self->popTask();

		if (task)
			task();
		else
		{
			self->alarm.wait(false);

			std::lock_guard lock(self->aliveLock);

			if (!self->alive)
				return;

			self->alarm.clear();
		}
	}
}

} // namespace b2
