#include "threadpool.hpp"
#include "exception.hpp"
#include "logger.hpp"

namespace b2
{

ThreadPool::ThreadPool() : workers(std::thread::hardware_concurrency()), alive(true)
{
	_assert(workers.size(), 0x620a358f);

	info("Threads count: %d", workers.size());

	for (ThreadPtr &thread : workers)
		thread = std::make_unique<std::thread>(workerRoutine, this);
}

ThreadPool::~ThreadPool()
{
	alive.store(false);

	for (ThreadPtr &thread : workers)
		thread->join();
}

size_t ThreadPool::getWorkersCount() const
{
	return workers.size();
}

ThreadPool &ThreadPool::getInstance()
{
	static ThreadPool instance;

	return instance;
}

void ThreadPool::workerRoutine(ThreadPool *self)
{
	while (self->alive.load())
	{
		std::unique_lock lock(self->tasksLock);

		if (self->tasks.size() > 0)
		{
			auto task = std::move(self->tasks.front());

			self->tasks.pop();
			lock.unlock();
			task();
		}
		else
			self->alarm.wait(lock);
	}
}

} // namespace b2
