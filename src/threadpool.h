#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>

class ThreadPool {
public:
  ThreadPool(size_t threads);
  ~ThreadPool();

  size_t getThreadsCount() const;

private:
  typedef std::shared_ptr<std::mutex>   SharedMutex;
  typedef std::shared_ptr<std::thread>  SharedThread;
  typedef std::function<void ()>        UserRoutine;

  struct ThreadContext {
    size_t      index;
    SharedMutex entryMutex;
    UserRoutine routine;
  };

  std::vector<std::pair<ThreadContext, SharedThread> > threads;

  void alloc(size_t threads);
  void free();
};
