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
  //  ¬ыводит из ожидани€ все потоки, которые ждут задачи.
  void resumeAll();
  //  ¬ыполнение одной задачи из списка. ‘ункци€ переводит поток в режим ожидани€, если задач нет.
  friend void doTask(TaskList &taskList);

private:
  TaskList(const TaskList &) = delete;
  TaskList &operator=(const TaskList &) = delete;

  void wait(std::unique_lock<std::mutex> &lock);

  std::list<TaskPtr> list;
  //AtomicLock entryLock;
  //  –еализаци€ со стандартным мьютексом вы итоге получаетс€ быстрее из-за того что потоки правильно усыпл€ютс€ и не забивают €дра бесполезной работой.
  std::mutex entryLock;
  std::condition_variable waitCond;
};

class ThreadPool {
public:
  ThreadPool();
  ~ThreadPool();

  void alloc(size_t count);

  void pushTask(TaskPtr &taskPtr);

  size_t getCount() const;

private:
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  std::vector<Thread> pool;
  TaskList taskList;
  bool quitFlag;

  void free();
};