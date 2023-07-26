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
	explicit ThreadPool(size_t workerCount = std::thread::hardware_concurrency());
	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;
	~ThreadPool();

	template<typename Task, typename... Arguments>
	[[nodiscard]] inline auto pushTask(Task task, Arguments &&...arguments)
		-> std::future<std::invoke_result_t<Task, Arguments...>>;

	[[nodiscard]] inline size_t getWorkersCount() const;

private:
	using ThreadPtr = std::unique_ptr<std::thread>;

	[[nodiscard]] inline std::function<void()> popTask();

	static void workerRoutine(ThreadPool *self);

	std::vector<ThreadPtr> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex tasksLock, aliveLock;
	std::atomic_flag alarm;
	bool alive;
};

template<typename Task, typename... Arguments>
auto ThreadPool::pushTask(Task task, Arguments &&...arguments) -> std::future<std::invoke_result_t<Task, Arguments...>>
{
	using TaskResult = std::invoke_result_t<Task, Arguments...>;

	auto promise = std::make_shared<std::promise<TaskResult>>();
	auto future = promise->get_future();

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
	alarm.test_and_set();
	alarm.notify_one();

	return future;
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

} // namespace b2
