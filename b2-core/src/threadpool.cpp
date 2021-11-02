#include <b2/exception.hpp>

#include "logger.hpp"
#include "threadpool.hpp"

namespace b2
{

ThreadPool::ThreadPool(size_t workerCount) : workers(workerCount), alarm(false), alive(true)
{
	_assert(!workers.empty(), 0x620a358f);

	info("Threads count: %d", workers.size());

	for (ThreadPtr &thread : workers)
		thread = std::make_unique<std::thread>(workerRoutine, this);
}

ThreadPool::~ThreadPool()
{
	assert(alive.load());

	{
		std::lock_guard lock(aliveLock);

		alive = false;
		alarm.test_and_set();
		alarm.notify_all();
	}

	for (ThreadPtr &thread : workers)
		thread->join();
}

size_t ThreadPool::getWorkersCount() const
{
	return workers.size();
}

std::function<void()> ThreadPool::popTask()
{
	std::lock_guard lock(tasksLock);

	if (tasks.empty())
		return {};

	auto task = std::move(tasks.front());

	tasks.pop();

	return task;
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
