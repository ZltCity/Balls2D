#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <functional>
#include <initializer_list>

typedef std::initializer_list<void *>     Arguments;
typedef std::function<void (Arguments &)> ThreadRoutine;

struct ThreadTask {
  ThreadRoutine routine;
  Arguments     args;
};

struct ThreadContext {
  size_t      index;
  std::thread thread;
  bool        quit;
};

template<size_t TCapacity>
class ThreadPool {
public:
  ThreadPool() {
    this->init();
  }
  ~ThreadPool() {
    this->free();
  }

  void pushTask(ThreadRoutine routine, Arguments args) {
    std::unique_lock
      <std::mutex>  lock(this->taskListEntry);
    ThreadTask      task = {
      routine,
      args
    };

    this->taskList.push_back(task);
    this->taskListCV.notify_one();
  }

  size_t getCapacity() {
    return TCapacity;
  }

private:
  std::mutex              taskListEntry;
  std::condition_variable taskListCV;
  std::list
    <ThreadTask>          taskList;
  ThreadContext           threads[TCapacity];

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  void init() {
    auto proxyRoutine = [&](ThreadContext *context) {
      while (!context->quit) {
        std::unique_lock
          <std::mutex>  lock(taskListEntry);
        
        taskListCV.wait(lock);
        //
        ThreadTask  task;
        bool        ready = false;        

        if (taskList.size() > 0) {
          task = taskList.front();
          taskList.pop_front();
          ready = true;
        }
        lock.unlock();

        if (ready)
          task.routine(task.args);
      }
    };

    for (size_t i = 0; i < TCapacity; ++i) {
      threads[i].index  = i;
      threads[i].quit   = false;
      //
      threads[i].thread = std::thread(proxyRoutine, &threads[i]);
    }
  }

  void free() {
    for (size_t i = 0; i < TCapacity; ++i) {
      this->threads[i].quit = true;
      this->taskListCV.notify_all();
      this->threads[i].thread.join();
    }
  }
};
