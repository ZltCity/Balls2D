#pragma once

#include <condition_variable>
#include <vector>
#include <list>

#include "atomic.h"
#include "thread.h"

class Task {
public:
  Task() = default;
  virtual ~Task() = 0;

  virtual void doTask() = 0;

  bool done() const;

  void setDone();
  void resetDone();

private:
  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;

  bool doneFlag;
};

typedef std::shared_ptr<Task> TaskPtr;

class TaskList {
public:
  TaskList() = default;

  void push(TaskPtr &taskPtr);
  TaskPtr pop();

  void wait();

private:
  TaskList(const TaskList &) = delete;
  TaskList &operator=(const TaskList &) = delete;

  std::list<TaskPtr> list;
  AtomicLock entryLock;
  std::condition_variable_any waitCond;
};

class ThreadPool {
public:
  ThreadPool(size_t count = 8);
  ~ThreadPool();

  void pushTask(TaskPtr &taskPtr);

  size_t getCount() const;

private:
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  std::vector<Thread> pool;
  TaskList taskList;
  bool quitFlag;

  void alloc(size_t count);
  void free();
};