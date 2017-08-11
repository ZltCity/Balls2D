#include "threadpool.h"

class ProxyWorker : public Worker {
public:
  ProxyWorker(TaskList &taskList, bool &quitFlag)
    : taskList(taskList), quitFlag(quitFlag) {}

  void exec() {
    while (!this->quitFlag) {
      TaskPtr taskPtr(nullptr);

      this->taskList.wait();
      taskPtr = this->taskList.pop();

      if (!taskPtr)
        continue;

      taskPtr->doTask();
      taskPtr->setDone();
    }
  }

private:
  ProxyWorker(const ProxyWorker &) = delete;
  ProxyWorker &operator=(const ProxyWorker &) = delete;

  TaskList &taskList;
  bool &quitFlag;
};

Task::~Task() {}

bool Task::done() const {
  return this->doneFlag;
}

void Task::setDone() {
  this->doneFlag = true;
}

void Task::resetDone() {
  this->doneFlag = false;
}

void TaskList::push(TaskPtr &taskPtr) {
  std::unique_lock<AtomicLock> lock(this->entryLock);

  this->list.push_back(taskPtr);
  this->waitCond.notify_one();
}

TaskPtr TaskList::pop() {
  std::unique_lock<AtomicLock> lock(this->entryLock);
  TaskPtr taskPtr(nullptr);

  if (this->list.size() > 0) {
    taskPtr = this->list.front();
    this->list.pop_front();
  }

  lock.unlock();

  return taskPtr;
}

void TaskList::wait() {
  std::unique_lock<AtomicLock> lock(this->entryLock);

  this->waitCond.wait(lock);
}

ThreadPool::ThreadPool(size_t count)
  : quitFlag(false) {
  this->alloc(count);
}

ThreadPool::~ThreadPool() {
  this->free();
}

void ThreadPool::pushTask(TaskPtr &taskPtr) {
  this->taskList.push(taskPtr);
}

size_t ThreadPool::getCount() const {
  return this->pool.size();
}

void ThreadPool::alloc(size_t count) {
  this->pool.resize(count);
  //
  for (size_t i = 0; i < count; ++i) {
    Thread thread(WorkerPtr(new ProxyWorker(this->taskList, this->quitFlag)));

    this->pool[i] = std::move(thread);
  }
}

void ThreadPool::free() {
  this->quitFlag = true;
  //
  for (size_t i = 0; i < this->getCount(); ++i)
    this->pool[i].wait();
}
