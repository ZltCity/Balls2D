#pragma once

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace b2
{

class ThreadPool
{
public:
	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;

	template<typename Task, typename... Arguments>
	auto pushTask(Task task, Arguments... arguments) -> std::future<std::invoke_result_t<Task, Arguments...>>;

	size_t getWorkersCount() const;
	void stop();

	static ThreadPool &getInstance();

private:
	using ThreadPtr = std::unique_ptr<std::thread>;

	ThreadPool();
	~ThreadPool();

	static void workerRoutine(ThreadPool *self);

	std::vector<ThreadPtr> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex tasksLock;
	std::condition_variable alarm;
	std::atomic_bool alive;
};

template<typename Task, typename... Arguments>
auto ThreadPool::pushTask(Task task, Arguments... arguments) -> std::future<std::invoke_result_t<Task, Arguments...>>
{
	using TaskResult = std::invoke_result_t<Task, Arguments...>;
	using Promise = std::promise<TaskResult>;

	std::shared_ptr<Promise> promise = std::make_shared<Promise>();
	std::future<TaskResult> future = promise->get_future();
	std::lock_guard lock(tasksLock);

	tasks.push([promise, task, arguments...]() {
		if constexpr (std::is_void_v<TaskResult>)
		{
			task(arguments...);
			promise->set_value();
		}
		else
			promise->set_value(task(arguments...));
	});
	alarm.notify_one();

	return future;
}

} // namespace b2
